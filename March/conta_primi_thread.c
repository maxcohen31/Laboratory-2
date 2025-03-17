#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <pthread.h>

#include "xerrori.h"

bool is_prime(int n)
{
    int start = 2;
    if (n <= 2)
    {
        return false;
    }

    while (sqrt(n) > start)
    {
        if (start % 2 == 0)
        {
            // start is even
            return false;
        }
        start++;
    }
    return true;

}


typedef struct _dati 
{
    int start;
    int end;
    int somme_parziali;
} dati;


void *tbody(void *v)
{
    dati *d = (dati *) v; // casting di v al tipo dati
    int primi = 0;
    fprintf(stderr, "Conto i primi tra %d e %d\n", d->start, d->end);
    // searching for the primes in the assigned range
    for (int j = d->start; j < d->end; ++j)
    {
        if (is_prime(j))
        {
            primi++;
        }
    }
    fprintf(stderr, "Thread starting from %d is terminated", d->start);
    d->somme_parziali = primi;
    pthread_exit(NULL);
}


int main(int argc, char **argv)
{
    if (argc != 3)
    {
        fprintf(stderr, "Usage: %s m num_threads", argv[0]);
    }

    // converting input
    int m = atoi(argv[1]); // numbers of threads
    if (m < 1) 
    {
        termina("Invalid limit");
    }
    
    int p = atoi(argv[2]);
    if (p <= 0)
    {
        termina("Invalid number of threads");
    }
    
    pthread_t t[p]; // array of thread pointers
    dati d[p]; // array of p structs passed to p threads
    int somma = 0;

    for (int i = 0; i < p; ++i)
    {
        int n = m/p; // numbers verified by each thread
        d[i].start = n*i;
        d[i].end = (i == p) ? m : n * (i + 1);
        xpthread_create(&t[i], NULL, &tbody, &d[i], __LINE__, __FILE__);
    }
    // waiting for the threads to end
    for (int i = 0; i < p; ++i)
    {
        xpthread_join(t[i], NULL, __LINE__, __FILE__);
        somma += d[i].somme_parziali;
    }

    printf("Number of primes between 1 and %d (excluded): %d", m, somma);

    return 0;
}


