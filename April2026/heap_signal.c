/*
 *  Modificare il programma heap.c (March2026/heap.c) aggiungendo un thread gestore di segnali tale che quando viene premuto il tasto ctrl-C 
 *  viene fatta un'allocazione di un blocco di 7 unità di memoria che deve essere mantenuto allocato per 5 secondi. 
 *  Il programma deve essere fatto in modo che successivi invii di ctrl-C effettuino successive allocazioni di 7 unità di memoria 
 *  anche se i precedenti blocchi da 7 non sono ancora stati deallocati 
 *  (naturalmente l'allocazione può avvenire solo se la memoria è disponibile altrimenti il thread deve rimanere in attesa).
 */

#include "xerrori.h"
#include <assert.h>
#include <bits/types/siginfo_t.h>
#include <pthread.h>
#include <signal.h>
#include <unistd.h>

typedef struct 
{
    pthread_cond_t *cv;
    pthread_mutex_t *mu;
    int MB;               /* Memory avaiable */
    sigset_t set;         /* Set of signals */
    int keep_going;
} heap;


/* Simulates allocation with limited space */
void allocate(heap *hp, int n)
{
    xpthread_mutex_lock(hp->mu, __LINE__, __FILE__);
    fprintf(stderr, "[%2d] Asked: %3d\n", gettid()%100, n);
    while (n > hp->MB)
    {
        fprintf(stderr, "[%2d] Negated: %3d\n", gettid()%100, n);
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
    fprintf(stderr, "[%2d] Freed: %3d. Remaining: %4d\n", gettid()%100, n, hp->MB);
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

/* ========================== NEW THREADS ========================= */ 

/* Thread worker */
void *worker(void *a)
{
    heap *h = (heap*)a;
    allocate(h, 7);
    sleep(5);
    deallocate(h, 7);
    pthread_exit(NULL);
}

/* Every Ctrl-C call it creates a worker to allocate 7MB of memory */
void *thread_signal(void *a)
{
    heap *h = (heap*)a;
    while (true)
    {
        int s; /* Received signal */
        int e = sigwait(&h->set, &s);
        if (e != 0) xtermina("sigwait() error", __LINE__, __FILE__);

        printf("Signal %d received from process %d\n", s, getpid());
        
        /* Handling Ctrl-C  */
        if (s == SIGINT)
        {
            /* For each time Ctrl-C is pressed we create a worker thread */ 
            pthread_t w;
            xpthread_create(&w, NULL, worker, h, __LINE__, __FILE__);
            if (pthread_detach(w) != 0) xtermina("pthread_detach() error", __LINE__, __FILE__);
            fprintf(stderr, "[%2d] Worker thread created\n", gettid()%100);
        }
        if (s == SIGUSR1)
        {
            /* Exit the main loop */
            h->keep_going = 0;
        }
    }
    pthread_exit(NULL);
}

int main(int argc, char **argv)
{
    if (argc != 2)
    {
        printf("Usage:\t %s <mem>\n", argv[0]);
    }
    
    int memory = atoi(argv[1]);
    assert(memory > 0);

    /* Signal handling */
    sigset_t set;
    sigemptyset(&set);
    sigaddset(&set, SIGINT);    /* Blocking SIGINT */
    sigaddset(&set, SIGUSR1);   /* Blocking SIGUSR1 */

    if (pthread_sigmask(SIG_BLOCK, &set, NULL) == -1) xtermina("pthread_sigmask() error", __LINE__, __FILE__);
    int c = 1;

    /* Heap initialization */
    pthread_cond_t cv = PTHREAD_COND_INITIALIZER;
    pthread_mutex_t mu = PTHREAD_MUTEX_INITIALIZER;
    heap hp;
    hp.cv = &cv;
    hp.mu = &mu;
    hp.MB = memory;
    hp.set = set;
    hp.keep_going = c;


    printf("My pid is: %d\n", getpid());
    printf("To allocate 7MB press Ctrl-C\n");
    printf("To terminate: kill -USR1 %d\n", getpid());

    pthread_t th;
    xpthread_create(&th, NULL, thread_signal, &hp, __LINE__, __FILE__);
    if (pthread_detach(th) != 0) xtermina("pthread_detach error", __LINE__, __FILE__);

    while (hp.keep_going != 0)
    {
        sleep(15);
        puts("Loop1 ended\n");
    }
        
    xpthread_cond_destroy(&cv, __LINE__, __FILE__);
    xpthread_mutex_destroy(&mu, __LINE__, __FILE__);

    fprintf(stderr, "Terminated with %d memory", hp.MB);
    return 0;
}
