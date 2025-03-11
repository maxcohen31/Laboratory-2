#include "xerrori.h"

#define QUI, __LINE__,__FILE__

typedef struct  
{
    pthread_cond_t *cv;
    pthread_mutex_t *mu;
    int MB;
} heap;

// simula allocazione con spazio limitato
void richiedi(heaè *h, int n)
{
    xpthred_mutex_lock(h->mu, QUI);
    fprintf(stderr, "%2d richiesti: %3d\n", gettid()%100, n);
    
    while (n>h->MB)
    {
        fprintf(stderr, "%2d negati: %3d\n", gettid()%100, n);
        xpthread_cond_wait(h->cv, h->mu, QUI);
    }

    h->MB -= n;
    fprintf(stderr, "%2d assegnati: %3d. rimanenti %4d\n", gettid()%100, n, h->MB);
    xpthread_mutex_unlock(h->mu, QUI);
}

// deallocazione
void libera(heap *h, int n) 
{
    xpthread_mutex_lock(h->mu,QUI);
    h->MB += n;
    xpthread_cond_broadcast(h->cv,QUI);
    fprintf(stderr,"%2d Restituiti: %3d. Rimanenti: %4d\n\n", gettid()%100,
            n,h->MB);
    xpthread_mutex_unlock(h->mu,QUI); 
}


// codice thread tipo 1, chiede 10,20,...,50
void *tipo1(void *v) 
{
    heap *h = (heap *) v;
    for (int i = 1; i <= 5; i++) 
    {
        int m = 10*i;
        richiedi(h,m);
        sleep(1);
        libera(h,m);
    }
    return NULL;
}

// codice thread tipo 2, chiede 15,25,...,55
void *tipo2(void *v) 
{
    heap *h = (heap *) v;
    for(int i = 1;i <= 5; i++) 
    {
        int m = 10*i+5;
        richiedi(h,m);
        sleep(1);
        libera(h,m);
    }
    return NULL;
}


int main(int argc, char *argv[])
{
  // controlla numero argomenti
  if (argc != 3) 
    {
        printf("Uso: %s mem numT\n",argv[0]);
        return 1;
    }

    int mem = atoi(argv[1]);
    int nt = atoi(argv[2]);
    assert(nt>1);

    // inizializza heap 
    pthread_cond_t c = PTHREAD_COND_INITIALIZER;
    pthread_mutex_t m = PTHREAD_MUTEX_INITIALIZER;
    heap h;
    h.cv = &c; h.mu = &m;
    h.MB = mem;
    
    // esegue i thread
    pthread_t t[nt];
    // esegue un thread tipo1
    xpthread_create(&t[0],NULL,&tipo1,&h,QUI);
    for (int i = 1; i<nt; i++)
        // esegue nt-1 thread di tipo 2
        xpthread_create(&t[i],NULL,&tipo2,&h,QUI);

    // attende terminazione thread e termina
    for (int i = 0; i < nt; i++)
    {
        xpthread_join(t[i],NULL,QUI);
    }
    xpthread_cond_destroy(&c,QUI);
    xpthread_mutex_destroy(&m,QUI);
    
    fprintf(stderr,"Terminato con memoria: %d\n", h.MB);
    return 0;
}

