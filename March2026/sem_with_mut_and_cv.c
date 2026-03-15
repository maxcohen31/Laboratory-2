/*
 *  Implementazione di un semaforo con mutex e variabili di condizione.
 *  L'operazione di decremento si blocca se il valore del semaforo diventa meno di zero.
 */

#include "xerrori.h"

/* Semaphore */
typedef struct 
{
    int tot;              /* Semaphore's counter */
    pthread_cond_t cond;
    pthread_mutex_t mu;
} zem;


/* Sets the value of the semaphore to q */
void zem_init(zem *z, int q)
{
    assert(q > 0);
    xpthread_cond_init(&z->cond, NULL, __LINE__, __FILE__);
    z->tot = q;
    xpthread_mutex_init(&z->mu, NULL, __LINE__, __FILE__);
}

/* Decrements the value of the semaphore. It cannot negative. 
 * This is the equivalent to sem_wait (P operationg of Dijkstra) */
void zem_down(zem *z, int q) 
{
    assert(q > 0);
    pthread_mutex_lock(&z->mu);
    while ((z->tot-q) < 0) pthread_cond_wait(&z->cond, &z->mu);
    z->tot -= q;
    pthread_mutex_unlock(&z->mu);
}

/* This is the equivalent to sem_post (V operation of Dijkstra) */
void zem_up(zem *z, int q) 
{
    assert(q > 0);
    pthread_mutex_lock(&z->mu);
    z->tot += q;
    pthread_cond_broadcast(&z->cond);
    pthread_mutex_unlock(&z->mu);
}
