#define _GNU_SOURCE
#include <sched.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <assert.h>
#include <stdatomic.h>

/*
    confronto di diversi metodi per evitare race conditions 
    nell'aggiornamento di una variabile intera condivisa 

    Se viene definito USE_MUTEX  la race condition
    viene evitata con un mutex

    Se viene definito USE_ATOMIC_SUM la race condition
    viene evitata meorizzando la somma in una variabile atomic_long
    che quindi può essere aggiornata in maniera atomica 
    con la funzione atomic_fetch_add

    Altrimenti la race condition viene evitata usando un 
    spinlock, che è una sorta di semplice mutex con attesa attiva:

    la funzione atomic_flag_test_and_set(volatile atomic_flag *obj)
    pone a true la variabile *obj e restituisce il precedente valore

    Il ciclo 

       while(atomic_flag_test_and_set(d->lock)) {}
         sezione critica
       atomic_flag_clear(&lock);

    effettua un busy waiting: attende che la d->lock diventi false
    e immediatamente lo pone a true (quindi altri thread devono attendere
    che si esca dalla sezione critica)

    il makefile crea gli eseguibili per tutte e tre le versioni:
    muspinlock.out  atspinlock.out e spinlock.out
    
    Gli esperimenti mostrano che l'uso della variabile atomic è 
    piu veloce del mutex che è piu veloce dello spinlock, 
    però lo spinlock diventa molto più veloce se nel body
    del while() si mette una chiamata a sched_yield()

    Viene utilizzata una pthread barrier per far partire tutti 
    i thread allo stesso momento allo scopo di massimizzare la
    probabilità di accessi simultanei alla variabile condivisa
*/

typedef struct 
{
#if defined (USE_MUTEX)
    long *somma;                /* Somma non atomica + mutex */
    pthread_mutex_t *mu;
#elif defined (USE_ATOMIC_SUM) 
    atomic_long *somma;         /* Somma atomica */
#else 
    long *somma;
    volatile atomic_flag *lock; /* Spinlock */
#endif
    pthread_barrier_t *barrier; /* Barriera per far partire i thread contemporaneamente */
    int num_somme;              /* Numero di somme da eseguire */
} dati;

void *tbody(void *args)
{
    dati *a = (dati*)args;
    pthread_barrier_wait(a->barrier);
    for (int i = 0; i < a->num_somme; i++)
    {
#if defined (USE_MUTEX)
        pthread_mutex_lock(a->mu);
        *(a->somma) += i;
        pthread_mutex_unlock(a->mu);
#elif defined (USE_ATOMIC_SUM)
        atomic_fetch_add(a->somma, i);                              /* Somma atomica */
#else /* Spinlock */
        while (atomic_flag_test_and_set(a->lock)) sched_yield();   /* Aggiungendo questa chiamata la spinlock è molto più veloce */
        *(a->somma) += i;                                           /* *(a->somma) non + atomico */
        atomic_flag_clear(a->lock);
#endif
    }
    pthread_exit(NULL);
}


int main(int argc, char **argv)
{
    if (argc != 3)
    {
        fprintf(stderr, "Usage:\t%s <thread_num> <num_somme>\n", argv[0]);
        exit(1);
    }

    int num_thread = atoi(argv[1]);
    int num_somme = atoi(argv[2]);

    pthread_t th[num_thread];
    dati d;
#if defined (USE_MUTEX)
    long somma = 0;
    d.somma = &somma;
    pthread_mutex_t mu = PTHREAD_MUTEX_INITIALIZER;
    d.mu = &mu;
#elif defined (USE_ATOMIC_SUM)
    atomic_long somma = ATOMIC_VAR_INIT(0);
    d.somma = &somma;
#else
    atomic_flag lock = ATOMIC_FLAG_INIT;
    d.lock = &lock;
    long somma = 0;
    d.somma = &somma;
#endif
    pthread_barrier_t b;
    pthread_barrier_init(&b, NULL, num_thread);
    d.barrier = &b;
    d.num_somme = num_somme;

    for (int i = 0; i < num_thread; i++)
    { if (pthread_create(&th[i], NULL, tbody, &d) != 0)
        {
            perror("pthread_create() error");
            exit(EXIT_FAILURE);
        }
    }

    for (int i = 0; i < num_thread; i++) pthread_join(th[i], NULL);

    pthread_barrier_destroy(&b);
    
    printf("Somma: %ld\n", somma);
    printf("Valore atteso: %ld\n", ((long)num_somme*(num_somme-1)/2)*num_thread);

    return 0;
}


