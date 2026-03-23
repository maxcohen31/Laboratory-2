#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>   // usleep
#include <stdbool.h>
#include <string.h>
#include <errno.h>


// tipo del buffer circolare condiviso con operazioni bloccanti put/get.
typedef struct {
  int *buf;  // array degli elementi
  int size;  // capacita' del buffer
  int front; // indice del prossimo elemento da prelevare
  int last;  // indice della prossima posizione libera
  int nelem; // n. di elementi presenti nel buffer
  pthread_mutex_t lock; // protegge l'accesso al buffer
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

  return 0;
}

// distrugge il buffer circolare
void cb_destroy(circular_buffer_t *cb) {
  pthread_mutex_destroy(&cb->lock);
  free(cb->buf);
}

// prova a inserire un elemento nel buffer.
// ritorna true se l'inserimento riesce, false altrimenti
bool cb_tryput(circular_buffer_t *cb, int item) {
    bool result = false;

    pthread_mutex_lock(&cb->lock);

    if (cb->nelem < cb->size) {
        cb->buf[cb->last] = item;
        cb->last = (cb->last + 1) % cb->size;
        cb->nelem++;
        result = true;
    }

    pthread_mutex_unlock(&cb->lock);

    return result;
}

// prova a prelevare un elemento dal buffer.
// ritorna true se il prelievo riesce, altrimenti false
bool cb_tryget(circular_buffer_t *cb, int *item) {
    bool result = false;

    pthread_mutex_lock(&cb->lock);

    if (cb->nelem > 0) {
        *item = cb->buf[cb->front];
        cb->front = (cb->front + 1) % cb->size;
        cb->nelem--;
        result = true;
    }

    pthread_mutex_unlock(&cb->lock);

    return result;
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
 * L'inserimento nel buffer è NON bloccante:
 * se il buffer è pieno, il produttore riprova finche' 
 * non riesce ad inserire, facendo attesa attiva mitigata
 * da una breve pausa (usleep)
 */

void *producer_thread(void *arg) {
    thread_arg_t *targ = (thread_arg_t *)arg;
    shared_state_t *shared = targ->shared;
    int id = targ->id;

    while (1) {
        int item;
	// prenoto il prossimo item da produrre
        pthread_mutex_lock(&shared->count_lock);
        if (shared->produced_count >= shared->total_puts) {
            pthread_mutex_unlock(&shared->count_lock);
            break;
        }
        item = shared->produced_count;
        shared->produced_count++;
        pthread_mutex_unlock(&shared->count_lock);

	// provo ad inserire, se il buffer e' pieno
	// riprovo dopo una breve pausa
        while (!cb_tryput(&shared->cb, item)) {
	  // usleep(1000); // perche' 1000 e non 500 o 2000?
	  sched_yield();   // equivalente a thread_yield()
        }

        printf("PROD %d -> put %d\n", id, item);
    }

    printf("PROD %d terminato\n", id);
    return NULL;
}

/*
 * Thread consumatore.
 *
 * Un consumatore continua a prelevare finché il numero totale di item
 * consumati non raggiunge total_puts.
 *
 * Tuttavia bisogna fare attenzione:
 * - se il buffer è vuoto, tryget fallisce
 * - non possiamo terminare subito, perché potrebbero arrivare altri item
 *
 * Per evitare il problema usiamo questo approccio:
 * 1. controlla se sono già stati consumati tutti gli item attesi
 * 2. prova un tryget
 * 3. se riesce, incrementa consumed_count
 * 4. se fallisce, aspetta un po' e riprova
 */
void *consumer_thread(void *arg) {
    thread_arg_t *targ = (thread_arg_t *)arg;
    shared_state_t *shared = targ->shared;
    int id = targ->id;

    while (1) {
        int item;

	// Prima controlliamo se devo uscire
        pthread_mutex_lock(&shared->count_lock);
        if (shared->consumed_count >= shared->total_puts) {
            pthread_mutex_unlock(&shared->count_lock);
            break;
        }
        pthread_mutex_unlock(&shared->count_lock);

	// provo a prelevare
        if (cb_tryget(&shared->cb, &item)) {
            pthread_mutex_lock(&shared->count_lock);
	    shared->consumed_count++;
	    printf("CONS %d <- got %d\n", id, item);
            pthread_mutex_unlock(&shared->count_lock);
        } else {
	  // buffer vuoto, riprovo tra poco
	  // usleep(500); // perche' questo valore?
	  // sched_yield();   // equivalente a thread_yield()
        }
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

    shared.total_puts = total_puts;  // numero totale di inserimenti
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
        cons_args[i].id = i;
        cons_args[i].shared = &shared;

        if (pthread_create(&consumers[i], NULL, consumer_thread, &cons_args[i]) != 0) {
            fprintf(stderr, "Errore nella creazione del consumatore %d\n", i);
            return EXIT_FAILURE;
        }
    }
    // creo i produttori
    for (int i = 0; i < num_producers; i++) {
        prod_args[i].id = i;
        prod_args[i].shared = &shared;

        if (pthread_create(&producers[i], NULL, producer_thread, &prod_args[i]) != 0) {
            fprintf(stderr, "Errore nella creazione del produttore %d\n", i);
            return EXIT_FAILURE;
        }
    }
    // attendo che terminino i produttori
    for (int i = 0; i < num_producers; i++) {
        pthread_join(producers[i], NULL);
    }
    // attendo che terminino i consumatori
    for (int i = 0; i < num_consumers; i++) {
        pthread_join(consumers[i], NULL);
    }

    printf("\nESECUZIONE TERMINATA\n");
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
