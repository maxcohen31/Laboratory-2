#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>
#include <stdbool.h>
#include <errno.h>

#include "queue.h"

/*
 * Bounded buffer con semantica Mesa e fairness FIFO separata:
 * - FIFO dei risvegli dei produttori in attesa di spazio
 * - FIFO dei risvegli dei consumatori in attesa di dati
 *
 * L'idea e' quella mostrata nelle slide di teoria:
 * ogni thread che deve sospendersi crea una propria condition variable privata,
 * la inserisce nella coda FIFO della sua classe (produttori o consumatori)
 * e poi attende in un ciclo while.
 *
 * Dopo il risveglio il thread deve sempre ricontrollare due condizioni:
 * 1) la condizione funzionale del buffer (non pieno / non vuoto)
 * 2) il fatto di essere in testa alla propria coda FIFO
 *
 *
 * NOTA: 
 * Le signal sulle condition variable NON sono accodate.
 * Se piu' eventi avvengono prima che un thread risvegliato
 * riprenda l'esecuzione, alcune signal possono essere "perse".
 *
 * Per questo motivo questa implementazione usa una
 * propagazione esplicita del risveglio.
 */

/* -------------------------------------------------------------------------- */
/* Strutture dati                                                             */
/* -------------------------------------------------------------------------- */

/*
 * Ogni thread che deve sospendersi alloca una waiter_t sul proprio stack.
 * La struttura contiene una condition variable privata su cui il thread verra'
 * risvegliato in modo mirato.
 */
typedef struct waiter {
  pthread_cond_t cv;   /* CV privata del thread sospeso */
} waiter_t;

/*
 * Buffer circolare condiviso.
 *
 * Oltre allo stato classico del bounded buffer, manteniamo due code FIFO:
 * - next_put: produttori sospesi in attesa di spazio libero
 * - next_get: consumatori sospesi in attesa di un elemento disponibile
 */
typedef struct {
  int *buf;            /* array degli elementi */
  int size;            /* capacita' del buffer */
  int front;           /* indice del prossimo elemento da prelevare */
  int last;            /* indice della prossima posizione libera */
  int nelem;           /* numero di elementi presenti nel buffer */

  pthread_mutex_t lock; /* protegge TUTTO lo stato del buffer */

  Queue_t *next_put;    /* coda FIFO dei produttori sospesi */
  Queue_t *next_get;    /* coda FIFO dei consumatori sospesi */
} circular_buffer_t;

/* Stato globale del programma di esempio. */
typedef struct {
  circular_buffer_t cb;

  int total_puts;
  int produced_count;
  int consumed_count;

  pthread_mutex_t count_lock;
} shared_state_t;

/* Argomento passato ai thread. */
typedef struct {
  int id;
  shared_state_t *shared;
} thread_arg_t;

/* -------------------------------------------------------------------------- */
/* Funzioni di utilita' per i waiter                                          */
/* -------------------------------------------------------------------------- */

/* Inizializza la CV privata del thread corrente. */
static int waiter_init(waiter_t *w) {
  return pthread_cond_init(&w->cv, NULL);
}

/* Distrugge la CV privata del thread corrente. */
static void waiter_destroy(waiter_t *w) {
  pthread_cond_destroy(&w->cv);
}

/*
 * Il thread corrente si sospende sulla propria CV privata.
 * pthread_cond_wait() rilascia atomicamente cb->lock e sospende il thread.
 * Al risveglio ritorna solo dopo avere riacquisito cb->lock.
 */
static void waiter_wait(waiter_t *w, circular_buffer_t *cb) {
  pthread_cond_wait(&w->cv, &cb->lock);
}

/* Risveglia il thread associato alla waiter privata. */
static void waiter_signal(waiter_t *w) {
  pthread_cond_signal(&w->cv);
}

/* -------------------------------------------------------------------------- */
/* Inizializzazione e distruzione del bounded buffer                          */
/* -------------------------------------------------------------------------- */

int cb_init(circular_buffer_t *cb, int size) {
  if (cb == NULL || size <= 0) {
    errno = EINVAL;
    return -1;
  }

  cb->buf = malloc((size_t)size * sizeof(int));
  if (cb->buf == NULL) {
    return -1;
  }

  cb->size = size;
  cb->front = 0;
  cb->last = 0;
  cb->nelem = 0;
  cb->next_put = NULL;
  cb->next_get = NULL;

  if (pthread_mutex_init(&cb->lock, NULL) != 0) {
    free(cb->buf);
    cb->buf = NULL;
    return -1;
  }

  cb->next_put = initQueue();
  if (cb->next_put == NULL) {
    pthread_mutex_destroy(&cb->lock);
    free(cb->buf);
    cb->buf = NULL;
    return -1;
  }

  cb->next_get = initQueue();
  if (cb->next_get == NULL) {
    deleteQueue(cb->next_put);
    pthread_mutex_destroy(&cb->lock);
    free(cb->buf);
    cb->buf = NULL;
    cb->next_put = NULL;
    return -1;
  }

  return 0;
}

void cb_destroy(circular_buffer_t *cb) {
  if (cb == NULL) return;

  deleteQueue(cb->next_get);
  deleteQueue(cb->next_put);
  pthread_mutex_destroy(&cb->lock);
  free(cb->buf);

  cb->next_get = NULL;
  cb->next_put = NULL;
  cb->buf = NULL;
}

/*
 * put bloccante con fairness FIFO tra i produttori.
 *
 * Un produttore deve sospendersi in due casi:
 * 1) il buffer e' pieno
 * 2) esiste gia' almeno un altro produttore in attesa
 *
 * Il caso (2) serve a rispettare l'ordine FIFO dei produttori: se un produttore
 * era gia' in coda, i nuovi arrivati non possono scavalcarlo, anche se nel
 * frattempo si libera uno slot.
 */
void cb_put(circular_buffer_t *cb, int item) {
  waiter_t self;

  pthread_mutex_lock(&cb->lock);

  if (length(cb->next_put) > 0 || cb->nelem == cb->size) {
    if (waiter_init(&self) != 0) {
      fprintf(stderr, "Errore: pthread_cond_init fallita in cb_put\n");
      pthread_mutex_unlock(&cb->lock);
      exit(EXIT_FAILURE);
    }

    // Inserisco la mia CV privata in fondo alla coda FIFO dei produttori.
    if (push(cb->next_put, &self) != 0) {
      fprintf(stderr, "Errore: push su next_put fallita\n");
      waiter_destroy(&self);
      pthread_mutex_unlock(&cb->lock);
      exit(EXIT_FAILURE);
    }

    /*
     * Semantica Mesa:
     * dopo ogni risveglio il thread deve sempre ricontrollare:
     * - che il buffer non sia pieno
     * - di essere il primo produttore autorizzato a procedere
     */
    do {
      waiter_wait(&self, cb);
    } while (cb->nelem == cb->size || top(cb->next_put) != &self);

    /*
     * Ora sono il primo produttore e c'e' spazio.
     * Posso uscire dalla coda dei produttori in attesa.
     */
    (void)pop(cb->next_put);

    /*
     * Staffetta intra-classe:
     * se dopo la mia uscita dalla coda c'e' ancora spazio nel buffer,
     * posso risvegliare subito il prossimo produttore in FIFO.
     *
     * Questo evita che piu' signal "utili" vadano perse tutte verso
     * lo stesso thread che era rimasto in testa alla coda.
     * Se piu' eventi si verificano (es. piu' put consecutivi),
     * piu' signal possono essere inviate allo stesso thread
     * ancora in testa alla coda, prima che questo si risvegli
     * e si rimuova dalla coda.
     *
     * Per evitare che altri thread restino bloccati,
     * il thread che si risveglia deve propagare il risveglio
     * al successivo nella coda FIFO (staffetta).
     */
    if (cb->nelem < cb->size) {
      waiter_t *next_prod = (waiter_t *)top(cb->next_put);
      if (next_prod != NULL) {
        waiter_signal(next_prod);
      }
    }

    waiter_destroy(&self);
  }

  /* Inserimento vero e proprio nel buffer circolare. */
  cb->buf[cb->last] = item;
  cb->last = (cb->last + 1) % cb->size;
  cb->nelem++;

  /*
   * Dopo un put il buffer non e' piu' vuoto.
   * Se ci sono consumatori in attesa, risveglio il primo consumatore.
   */
  if (cb->nelem > 0) {
    waiter_t *next_cons = (waiter_t *)top(cb->next_get);
    if (next_cons != NULL) {
      waiter_signal(next_cons);
    }
  }

  pthread_mutex_unlock(&cb->lock);
}

/*
 * get bloccante con fairness FIFO tra i consumatori.
 *
 * Un consumatore deve sospendersi in due casi:
 * 1) il buffer e' vuoto
 * 2) esiste gia' almeno un altro consumatore in attesa
 *
 * Anche qui il caso (2) evita che un consumatore appena arrivato scavalchi
 * un consumatore piu' vecchio gia' sospeso.
 */
int cb_get(circular_buffer_t *cb) {
  int item;
  waiter_t self;

  pthread_mutex_lock(&cb->lock);

  if (length(cb->next_get) > 0 || cb->nelem == 0) {
    if (waiter_init(&self) != 0) {
      fprintf(stderr, "Errore: pthread_cond_init fallita in cb_get\n");
      pthread_mutex_unlock(&cb->lock);
      exit(EXIT_FAILURE);
    }

    /* Mi accodo in fondo alla coda FIFO dei consumatori sospesi. */
    if (push(cb->next_get, &self) != 0) {
      fprintf(stderr, "Errore: push su next_get fallita\n");
      waiter_destroy(&self);
      pthread_mutex_unlock(&cb->lock);
      exit(EXIT_FAILURE);
    }

    /*
     * Semantica Mesa:
     * al risveglio devo sempre ricontrollare:
     * - che il buffer non sia vuoto
     * - di essere il primo consumatore autorizzato a procedere
     */
    do {
      waiter_wait(&self, cb);
    } while (cb->nelem == 0 || top(cb->next_get) != &self);

    /*
     * Ora sono il primo consumatore e c'e' almeno un elemento.
     * Posso uscire dalla coda dei consumatori in attesa.
     */
    (void)pop(cb->next_get);

    /*
     * Staffetta intra-classe:
     * se dopo la mia uscita dalla coda ci sono ancora elementi nel buffer,
     * posso risvegliare subito il prossimo consumatore in FIFO.
     *
     * Questo evita che, ad esempio, piu' put consecutive segnalino sempre
     * lo stesso consumatore ancora in testa alla coda.
     */
    if (cb->nelem > 0) {
      waiter_t *next_cons = (waiter_t *)top(cb->next_get);
      if (next_cons != NULL) {
        waiter_signal(next_cons);
      }
    }

    waiter_destroy(&self);
  }

  /* Prelievo vero e proprio dal buffer circolare. */
  item = cb->buf[cb->front];
  cb->front = (cb->front + 1) % cb->size;
  cb->nelem--;

  /*
   * Dopo un get il buffer non e' piu' pieno.
   * Se ci sono produttori in attesa, risveglio il primo produttore.
   */
  if (cb->nelem < cb->size) {
    waiter_t *next_prod = (waiter_t *)top(cb->next_put);
    if (next_prod != NULL) {
      waiter_signal(next_prod);
    }
  }

  pthread_mutex_unlock(&cb->lock);
  return item;
}

/* Utility: stampa dello stato del buffer. */
void cb_print_state(circular_buffer_t *cb, const char *prefix) {
  pthread_mutex_lock(&cb->lock);
  printf("%s [front=%d, last=%d, nelem=%d, waiting_put=%lu, waiting_get=%lu]\n",
         prefix,
         cb->front,
         cb->last,
         cb->nelem,
         length(cb->next_put),
         length(cb->next_get));
  pthread_mutex_unlock(&cb->lock);
}


static void short_pause_usecs(long usecs) {
  struct timespec ts;
  ts.tv_sec = usecs / 1000000;
  ts.tv_nsec = (usecs % 1000000) * 1000;
  nanosleep(&ts, NULL);
}

// producer
void *producer_thread(void *arg) {
  thread_arg_t *targ = (thread_arg_t *)arg;
  shared_state_t *shared = targ->shared;
  int id = targ->id;

  while (1) {
    int item;

    pthread_mutex_lock(&shared->count_lock);
    if (shared->produced_count >= shared->total_puts) {
      pthread_mutex_unlock(&shared->count_lock);
      break;
    }
    item = shared->produced_count;
    shared->produced_count++;
    pthread_mutex_unlock(&shared->count_lock);

    cb_put(&shared->cb, item);
    printf("PROD %d -> put %d\n", id, item);

    short_pause_usecs(1000);
  }

  printf("PROD %d terminato\n", id);
  return NULL;
}

// consumer
void *consumer_thread(void *arg) {
  thread_arg_t *targ = (thread_arg_t *)arg;
  shared_state_t *shared = targ->shared;
  int id = targ->id;

  while (1) {
    int item = cb_get(&shared->cb);

    if (item == -1) {
      break;
    }

    pthread_mutex_lock(&shared->count_lock);
    shared->consumed_count++;
    pthread_mutex_unlock(&shared->count_lock);

    printf("CONS %d <- got %d\n", id, item);
    short_pause_usecs(500);
  }

  printf("CONS %d terminato\n", id);
  return NULL;
}

int parse_positive_int(const char *s, const char *name) {
  char *endptr = NULL;
  long val;

  errno = 0;
  val = strtol(s, &endptr, 10);

  if (errno != 0 || endptr == s || *endptr != '\0' || val <= 0) {
    fprintf(stderr, "Valore non valido per %s: %s\n", name, s);
    exit(EXIT_FAILURE);
  }

  if (val > 100000000) {
    fprintf(stderr, "Valore troppo grande per %s: %s\n", name, s);
    exit(EXIT_FAILURE);
  }

  return (int)val;
}

int main(int argc, char *argv[]) {
  int num_producers;
  int num_consumers;
  int buffer_size;
  int total_puts;

  pthread_t *producers = NULL;
  pthread_t *consumers = NULL;
  thread_arg_t *prod_args = NULL;
  thread_arg_t *cons_args = NULL;

  shared_state_t shared;

  if (argc != 5) {
    fprintf(stderr,
            "Uso: %s <num_produttori> <num_consumatori> <buffer_size> <total_puts>\n",
            argv[0]);
    fprintf(stderr, "Esempio: %s 2 3 5 20\n", argv[0]);
    return EXIT_FAILURE;
  }

  num_producers = parse_positive_int(argv[1], "num_produttori");
  num_consumers = parse_positive_int(argv[2], "num_consumatori");
  buffer_size   = parse_positive_int(argv[3], "buffer_size");
  total_puts    = parse_positive_int(argv[4], "total_puts");

  if (cb_init(&shared.cb, buffer_size) != 0) {
    fprintf(stderr, "Errore nell'inizializzazione del buffer\n");
    return EXIT_FAILURE;
  }

  shared.total_puts = total_puts;
  shared.produced_count = 0;
  shared.consumed_count = 0;

  if (pthread_mutex_init(&shared.count_lock, NULL) != 0) {
    fprintf(stderr, "Errore nell'inizializzazione del mutex dei contatori\n");
    cb_destroy(&shared.cb);
    return EXIT_FAILURE;
  }

  producers = malloc((size_t)num_producers * sizeof(pthread_t));
  consumers = malloc((size_t)num_consumers * sizeof(pthread_t));
  prod_args = malloc((size_t)num_producers * sizeof(thread_arg_t));
  cons_args = malloc((size_t)num_consumers * sizeof(thread_arg_t));

  if (producers == NULL || consumers == NULL ||
      prod_args == NULL || cons_args == NULL) {
    fprintf(stderr, "Errore di allocazione memoria\n");
    free(producers);
    free(consumers);
    free(prod_args);
    free(cons_args);
    pthread_mutex_destroy(&shared.count_lock);
    cb_destroy(&shared.cb);
    return EXIT_FAILURE;
  }

  cb_print_state(&shared.cb, "Stato iniziale buffer:");

  for (int i = 0; i < num_consumers; i++) {
    cons_args[i].id = i;
    cons_args[i].shared = &shared;

    if (pthread_create(&consumers[i], NULL, consumer_thread, &cons_args[i]) != 0) {
      fprintf(stderr, "Errore nella creazione del consumatore %d\n", i);
      return EXIT_FAILURE;
    }
  }

  for (int i = 0; i < num_producers; i++) {
    prod_args[i].id = i;
    prod_args[i].shared = &shared;

    if (pthread_create(&producers[i], NULL, producer_thread, &prod_args[i]) != 0) {
      fprintf(stderr, "Errore nella creazione del produttore %d\n", i);
      return EXIT_FAILURE;
    }
  }

  for (int i = 0; i < num_producers; i++) {
    pthread_join(producers[i], NULL);
  }

  // inseriamo un valore speciale nel buffer per ogni consumatore
  for (int i = 0; i < num_consumers; i++) {
    cb_put(&shared.cb, -1);
  }
  // attendiamo la terminazione dei consumatori
  for (int i = 0; i < num_consumers; i++) {
    pthread_join(consumers[i], NULL);
  }

  printf("\n\n");
  printf("  prodotti  = %d\n", shared.produced_count);
  printf("  consumati = %d\n", shared.consumed_count);

  cb_print_state(&shared.cb, "Stato finale buffer:");

  free(producers);
  free(consumers);
  free(prod_args);
  free(cons_args);

  pthread_mutex_destroy(&shared.count_lock);
  cb_destroy(&shared.cb);

  return EXIT_SUCCESS;
}
