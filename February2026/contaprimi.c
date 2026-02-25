#include "xerrori.h"
#include <math.h>

/* Basic thread usage: the workload can be shared between multiple threads */

/* Struct shared among threads */
typedef struct 
{
    int start;
    int end;
    int partial_sum;
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

/* Function passed to pthread_create */
void *tbody(void *v)
{
    dati *d = (dati*)v;
    int primes = 0;
    fprintf(stderr, "Counting primes between %d and %d\n", d->start, d->end);
    
    for (int i = d->start; i < d->end; i++)
    {
        if (is_prime(i)) primes++;
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

    /* Create auxiliary threads */
    pthread_t t[p];
    dati d[p]; /* p struct array */
    for (int i = 0; i < p; i++)
    {
        int n = m / p; /* Interval checked by a single thread */
        d[i].start = n * i;
        d[i].end = (i == p-1) ? m : n * (i+1);
        xpthread_create(&t[i], NULL, &tbody, &d[i], __LINE__, __FILE__);
    }

    int sum = 0;
    for (int i = 0; i < p; i++)
    {
        xpthread_join(t[i], NULL, __LINE__, __FILE__);
        sum += d[i].partial_sum;
    }
    
    /* Return result */
    printf("Primes between 1 and %d (excluded): %d\n", m, sum);
    return 0;
}


