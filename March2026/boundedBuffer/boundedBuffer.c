#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <stdbool.h>
#include <errno.h>

// tipo del buffer circolare condiviso con operazioni bloccanti put/get.
typedef struct {
  int *buf;  // array degli elementi
  int size;  // capacita' del buffer
  int front; // indice del prossimo elemento da prelevare
  int last;  // indice della prossima posizione libera
  int nelem; // n. di elementi presenti nel buffer

  pthread_mutex_t lock;       // protegge tutto lo stato del buffer
  pthread_cond_t not_full;    // usata dai produttori per attendere spazio libero
  pthread_cond_t not_empty;   // usata dai consumatori per attendere che ci siano dati
} circular_buffer_t;

// Tipo dello stato globale del programma
typedef struct {
  circular_buffer_t cb;  // buffer circolare

  // contatori (non fanno parte del buffer, sono per la gestione del programma di esempio)
  int total_puts;     // numero totale di item che devono essere prodotti
  int produced_count; // quanti item sono stati assegnati ai produttori
  int consumed_count; // quanti item sono stati effettivamente consumati

  pthread_mutex_t count_lock; // protegge l'accesso ai contatori 
} shared_state_t;


// tipo dell'argomento passato a ciascun thread
typedef struct {
  int id;
  shared_state_t *shared;
} thread_arg_t;

// inizializza il buffer circolare
int cb_init(circular_buffer_t *cb, int size) {
  cb->buf = malloc(size * sizeof(int));
  if (cb->buf == NULL) {
    return -1;
  }

  cb->size = size;
  cb->front = 0;
  cb->last = 0;
  cb->nelem = 0;

  if (pthread_mutex_init(&cb->lock, NULL) != 0) {
    free(cb->buf);
    return -1;
  }

  if (pthread_cond_init(&cb->not_full, NULL) != 0) {
    pthread_mutex_destroy(&cb->lock);
    free(cb->buf);
    return -1;
  }

  if (pthread_cond_init(&cb->not_empty, NULL) != 0) {
    pthread_cond_destroy(&cb->not_full);
    pthread_mutex_destroy(&cb->lock);
    free(cb->buf);
    return -1;
  }

  return 0;
}

// distrugge il buffer circolare
void cb_destroy(circular_buffer_t *cb) {
  pthread_cond_destroy(&cb->not_empty);
  pthread_cond_destroy(&cb->not_full);
  pthread_mutex_destroy(&cb->lock);
  free(cb->buf);
}

/*
 * put bloccante.
 *
 * Se il buffer è pieno, il thread produttore si blocca su not_full.
 * Quando c'è spazio, inserisce l'item e segnala not_empty perché
 * ora potrebbe esserci un consumatore che può procedere.
 */
void cb_put(circular_buffer_t *cb, int item) {
  pthread_mutex_lock(&cb->lock);

  // quando il thread si risveglia, deve ricontrollare la condizione.
  while (cb->nelem == cb->size) {
    pthread_cond_wait(&cb->not_full, &cb->lock);
  }
  
  cb->buf[cb->last] = item;
  cb->last = (cb->last + 1) % cb->size;
  cb->nelem++;

  // dopo un inserimento il buffer non e' piu' vuoto e segnalo un
  // eventuale consumatore in attesa
  pthread_cond_signal(&cb->not_empty);

  pthread_mutex_unlock(&cb->lock);
}

/*
 * get bloccante.
 *
 * Se il buffer è vuoto, il thread consumatore si blocca su not_empty.
 * Quando c'è almeno un elemento, lo preleva e segnala not_full perché
 * ora potrebbe esserci un produttore che può procedere.
 */
int cb_get(circular_buffer_t *cb) {
  int item;

  pthread_mutex_lock(&cb->lock);

  // quando il thread si risveglia, deve ricontrollare la condizione.
  while (cb->nelem == 0) {
    pthread_cond_wait(&cb->not_empty, &cb->lock);
  }

  item = cb->buf[cb->front];
  cb->front = (cb->front + 1) % cb->size;
  cb->nelem--;

  // dopo aver prelevato, il buffer non e' piu' pieno, segnalo
  pthread_cond_signal(&cb->not_full);

  pthread_mutex_unlock(&cb->lock);

  return item;
}

// utility, stampa dello stato del buffer
void cb_print_state(circular_buffer_t *cb, const char *prefix) {
  pthread_mutex_lock(&cb->lock);

  printf("%s [front=%d, last=%d, nelem=%d]\n",
	 prefix, cb->front, cb->last, cb->nelem);

  pthread_mutex_unlock(&cb->lock);
}

/*
 * Thread produttore.
 *
 * L'inserimento nel buffer è bloccante:
 * se il buffer è pieno, il produttore aspetta.
 */
void *producer_thread(void *arg) {
  thread_arg_t *targ = (thread_arg_t *)arg;
  shared_state_t *shared = targ->shared;
  int id = targ->id;

  while (1) {
    int item;

    pthread_mutex_lock(&shared->count_lock);
    if (shared->produced_count >= shared->total_puts) { // finito esco....
      pthread_mutex_unlock(&shared->count_lock);        // ... devo rilasciare la lock
      break;
    }
    item = shared->produced_count;
    shared->produced_count++;
    pthread_mutex_unlock(&shared->count_lock);

    cb_put(&shared->cb, item);  // tutta la gestione della sincronizzazione è nella cb_put
    printf("PROD %d -> put %d\n", id, item);

    // piccola pausa per rendere l'interleaving "visibile" durante l'esecuzione
    usleep(1000);
  }

  printf("PROD %d terminato\n", id);
  return NULL;
}

/*
 * Thread consumatore.
 *
 * Il consumatore deve terminare quando il numero totale di item
 * consumati raggiunge total_puts.
 *
 * Tuttavia c'è un problema classico:
 * se un consumatore entra in cb_get quando non arriveranno più dati,
 * rischia di restare bloccato per sempre.
 *
 * Per evitare ciò, usiamo una tecnica semplice: 
 * dopo la terminazione di tutti i produttori, il main inserisce
 * nel buffer un valore speciale per ogni consumatore.
 *
 * Il valore speciale, per questo esempio, è -1.
 */
void *consumer_thread(void *arg) {
  thread_arg_t *targ = (thread_arg_t *)arg;
  shared_state_t *shared = targ->shared;
  int id = targ->id;

  while (1) {
    // prelevo un valore dal buffer
    int item = cb_get(&shared->cb);

    if (item == -1) { // devo uscire
      break;
    }

    // aggiorno i contatori globali (in mutua esclusione)
    pthread_mutex_lock(&shared->count_lock);
    shared->consumed_count++;
    pthread_mutex_unlock(&shared->count_lock);

    printf("CONS %d <- got %d\n", id, item);
   
    usleep(500);
  }

  printf("CONS %d terminato\n", id);
  return NULL;
}

// parsing di un intero positivo
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
    fprintf(stderr,
	    "Esempio: %s 2 3 5 20\n",
	    argv[0]);
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

  shared.total_puts = total_puts;   // numero totale di inserimenti
  shared.produced_count = 0;
  shared.consumed_count = 0;

  if (pthread_mutex_init(&shared.count_lock, NULL) != 0) {
    fprintf(stderr, "Errore nell'inizializzazione del mutex dei contatori\n");
    cb_destroy(&shared.cb);
    return EXIT_FAILURE;
  }

  producers = malloc(num_producers * sizeof(pthread_t));
  consumers = malloc(num_consumers * sizeof(pthread_t));
  prod_args = malloc(num_producers * sizeof(thread_arg_t));
  cons_args = malloc(num_consumers * sizeof(thread_arg_t));

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

  // creo i consumatori
  for (int i = 0; i < num_consumers; i++) {
    cons_args[i].id = i;             // id logico del thread 
    cons_args[i].shared = &shared;   // stato globale

    if (pthread_create(&consumers[i], NULL, consumer_thread, &cons_args[i]) != 0) {
      fprintf(stderr, "Errore nella creazione del consumatore %d\n", i);
      return EXIT_FAILURE;
    }
  }
  
  // creo i produttori
  for (int i = 0; i < num_producers; i++) {
    prod_args[i].id = i;             // id logico del thread
    prod_args[i].shared = &shared;   // stato globale

    if (pthread_create(&producers[i], NULL, producer_thread, &prod_args[i]) != 0) {
      fprintf(stderr, "Errore nella creazione del produttore %d\n", i);
      return EXIT_FAILURE;
    }
  }
  // attendiamo la terminazione dei produttori
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
