/*
 *  Gestione dello heap mediante variabile di condizione
 */

#include "xerrori.h"
#include <pthread.h>
#include <unistd.h>

typedef struct 
{
    pthread_cond_t *cv;
    pthread_mutex_t *mu;
    int MB;               /* Memory avaiable */
} heap;


/* Simulates allocation with limited space */
void allocate(heap *hp, int n)
{
    xpthread_mutex_lock(hp->mu, __LINE__, __FILE__);
    fprintf(stderr, "[%2d] Asked: %3d\n", gettid()%100, n);
    while (n > hp->MB)
    {
        fprintf(stderr, "[%2d] Negated: %3d", gettid()%100, n);
        xpthread_cond_wait(hp->cv, hp->mu, __LINE__, __FILE__);
    }
    hp->MB -= n;
    fprintf(stderr, "[%2d] Allocated: %3d. Remaining: %4d\n\n", gettid()%100, n, hp->MB);
    xpthread_mutex_unlock(hp->mu, __LINE__, __FILE__); 
}

/* Simulates the free function */
void deallocate(heap *hp, int n)
{
    xpthread_mutex_lock(hp->mu, __LINE__, __FILE__);
    hp->MB += n;
    xpthread_cond_broadcast(hp->cv, __LINE__, __FILE__);
    fprintf(stderr, "[%2d] Freed: %3d. Remaining: %4d", gettid()%100, n, hp->MB);
    xpthread_mutex_unlock(hp->mu, __LINE__, __FILE__);
}

/* First thread. It asks for 10, 20, ...,  MB*/
void *type_one_thread(void *v)
{
    heap *h = (heap*)v;
    for (int i = 1; i <= 5; i++)
    {
        int m = 10 * i;
        allocate(h, m);
        sleep(1);
        deallocate(h, m);
    }
    pthread_exit(NULL);
}

void *type_two_thread(void *v)
{
    heap *h = (heap*)v;
    for (int i = 1; i <= 5; i++)
    {
        int m = (10 * i) + 5;
        allocate(h, m);
        sleep(1);
        deallocate(h, m);
    }
    pthread_exit(NULL);
}

int main(int argc, char **argv)
{
    if (argc != 3)
    {
        printf("Usage:\t %s <mem> <thread_num>", argv[0]);
    }
    
    int memory = atoi(argv[1]);
    int thread_num = atoi(argv[2]);
    assert (thread_num > 1);

    /* Heap initialization */
    pthread_cond_t cv = PTHREAD_COND_INITIALIZER;
    pthread_mutex_t mu = PTHREAD_MUTEX_INITIALIZER;
    heap hp;
    hp.cv = &cv;
    hp.mu = &mu;
    hp.MB = memory;

    pthread_t th[thread_num];
    xpthread_create(&th[0], NULL, &type_one_thread, &hp, __LINE__, __FILE__); /* One thread of type one */
    for (int i = 1; i < thread_num; i++)
    {
        xpthread_create(&th[0], NULL, &type_two_thread, &hp, __LINE__, __FILE__); /* One thread of type two */
    }

    /* Threads termination */
    for (int i = 0; i < thread_num; i++)
    {
        xpthread_join(th[i], NULL, __LINE__, __FILE__);
    }

    xpthread_cond_destroy(&cv, __LINE__, __FILE__);
    xpthread_mutex_destroy(&mu, __LINE__, __FILE__);

    fprintf(stderr, "Terminated with %d memory", hp.MB);
    return 0;
}
