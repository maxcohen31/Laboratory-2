#include "xerrori.h"
#define QUI __LINE__, __FILE__

/* ******************* Data structures ***********************/

typedef struct 
{
    int tot;              /* Semaphore's counter */
    pthread_cond_t cond;
    pthread_mutex_t mu;
} zem;

/* Heap */
typedef struct 
{
    //pthread_cond_t *cv;
    pthread_mutex_t *mu;
    zem *sem;
    int MB;
} heap;

/* Sets the value of the semaphore to q */
void zem_init(zem *z, int q)
{
    assert(q > 0);
    xpthread_cond_init(&z->cond, NULL, __LINE__, __FILE__);
    z->tot = q;
    xpthread_mutex_init(&z->mu, NULL, __LINE__, __FILE__);
}

/* Decrements the value of the semaphore. It cannot be negative. 
 * This is the equivalent to sem_wait (P operation of Dijkstra) */
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
    pthread_cond_signal(&z->cond);
    pthread_mutex_unlock(&z->mu);
}

/* ************************** Heap operations **************************** */
void allocate(heap *hp, int n)
{
    printf("[%2d] requested %3d\n", gettid()%100, n);
    zem_down(hp->sem, n);
    xpthread_mutex_lock(hp->mu, QUI);
    hp->MB -= n;
    xpthread_mutex_unlock(hp->mu, QUI);
    printf("Allocated %d MB\n", n);
}

void deallocate(heap *hp, int n)
{
    xpthread_mutex_lock(hp->mu, QUI);
    hp->MB += n;
    xpthread_mutex_unlock(hp->mu, QUI);
    zem_up(hp->sem, n);
    printf("[%2d] Freed %d MB\n", gettid()%100, n);
}

/* ********************** Thread bodies *********************** */

/* First thread. It asks for 10, 20, ...,  MB */
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
    zem sem;
    zem_init(&sem, memory);

    hp.mu = &mu;
    hp.sem = &sem;
    hp.MB = memory;

    pthread_t th[thread_num];
    xpthread_create(&th[0], NULL, &type_one_thread, &hp, __LINE__, __FILE__); /* One thread of type one */
    for (int i = 1; i < thread_num; i++)
    {
        xpthread_create(&th[i], NULL, &type_two_thread, &hp, __LINE__, __FILE__); /* One thread of type two */
    }

    /* Threads termination */
    for (int i = 0; i < thread_num; i++)
    {
        xpthread_join(th[i], NULL, __LINE__, __FILE__);
    }

    xpthread_cond_destroy(&cv, __LINE__, __FILE__);
    xpthread_mutex_destroy(&mu, __LINE__, __FILE__);

    fprintf(stderr, "Terminated with %d memory\n", hp.MB);
 
    return 0;
}
