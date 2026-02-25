#include "xerrori.h"
#include <math.h>

/* Program to build a prime table using multiple threads
 * The following code was obtained by modifying contaprimi.c 
 * It shows how to use a mutex variable to gain access to a shared data */

/* Struct shared among threads */
typedef struct 
{
    int start;
    int end;
    int partial_sum;
    int *table;              /* prime table */
    int *inserted;           /* counts the primes inserted into table */ 
    pthread_mutex_t *pmutex; /* mutex shared among threads */
} dati;

/* Take an integer an returns true if prime, false otherwise */
bool is_prime(int n)
{
    if (n < 2) return false;
    int x = 2;
    while (x <= sqrt(n))
    {
        if (n % x == 0) return false;
        x++;
    }
    return true;
}

/* Function to pass to pthread_create */
void *tbody(void *v)
{
    dati *d = (dati*)v;
    int primes = 0;
    
    xpthread_mutex_lock(d->pmutex, __LINE__, __FILE__);
    for (int i = d->start; i < d->end; i++)
    {
        if (is_prime(i)) primes++;
        /* Critical section: each thread access the table one at a time */
        xpthread_mutex_lock(d->pmutex, __LINE__, __FILE__);
        d->table[*(d->inserted)] = i;
        d->inserted++;
        xpthread_mutex_unlock(d->pmutex, __LINE__, __FILE__);
    }
    fprintf(stderr, "The thread started from %d has finished!\n", d->start);
    d->partial_sum = primes;
    pthread_exit(NULL);
}

int main(int argc, char **argv)
{
    if (argc != 3)
    {
        fprintf(stderr, "Usage:\n\t%s m num_threads\n", argv[0]);
        exit(1);
    }
    
    /* Input conversion */
    int m = atoi(argv[1]);
    if (m < 1) termina("Prime limit not valid");
    int p = atoi(argv[2]);
    if (p < 1) termina("Thread number not valid");

    /* Mutex init */
    pthread_mutex_t tab_mutex;
    xpthread_mutex_init(&tab_mutex, NULL, __LINE__, __FILE__);

    /* Create auxiliary threads */
    pthread_t t[p];
    dati d[p]; /* p struct array */

    int *p_table = malloc(m*sizeof(int));
    if (p_table == NULL) xtermina("Memory allocation failed", __LINE__, __FILE__);
    int p_inserted = 0;

    for (int i = 0; i < p; i++)
    {
        int n = m / p; /* Interval checked by a single thread */
        d[i].start = n * i;
        d[i].end = (i == p-1) ? m : n * (i+1);
        d->table = p_table;
        d->pmutex = &tab_mutex;
        d->inserted = &p_inserted;
        xpthread_create(&t[i], NULL, &tbody, &d[i], __LINE__, __FILE__);
    }

    xpthread_mutex_destroy(&tab_mutex, __LINE__, __FILE__);

    int tab_sum = 0;
    for (int i = 0; i < p; i++)
    {
        xpthread_join(t[i], NULL, __LINE__, __FILE__);
        p_inserted++;
        tab_sum += d[i].partial_sum;
    }
    
    /* Return result */
    printf("Primes inserted: %d", p_inserted);
    printf("Primes between 1 and %d (excluded): %d\n", m, tab_sum);
    return 0;
}


