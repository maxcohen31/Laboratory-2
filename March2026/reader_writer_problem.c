/*
 * Problema lettori/scrittori
 *
 * Questa soluzione senza wpending è unfair per gli scrittori
 * che potrebbero essere messi in attesa indefinita se 
 * continuano ad arrivare lettori.
 *
 * */

#include "xerrori.h"
#include <assert.h>
#include <pthread.h>
#include <unistd.h>
#define QUI __LINE__,__FILE__

typedef struct 
{
    int readers;
    bool writing;
    pthread_cond_t cond;
    pthread_mutex_t mu;
} rw;

/* Inizializza rw */
void rw_init(rw *z)
{
    z->readers = 0;
    z->writing = false;
    xpthread_cond_init(&z->cond, NULL, QUI);
    xpthread_mutex_init(&z->mu, NULL, QUI);
}

/* Inizio uso da parte di un reader */
void read_lock(rw *z)
{
    fprintf(stderr, "%2d richiesta read\n", gettid()%100);
    pthread_mutex_lock(&z->mu);
    /* Se si elimina il test su z->wpending si ha la soluzione unfair per gli scrittori */
    while (z->writing == true) pthread_cond_wait(&z->cond, &z->mu);
    z->readers++;
    pthread_mutex_unlock(&z->mu);
}

/* Fine uso da parte di un reader */
void read_unlock(rw *z)
{
    fprintf(stderr, "%2d read completata\n", gettid()%100);
    pthread_mutex_lock(&z->mu);
    assert(z->readers > 0); /* Ci deve essere almeno un lettore (me stesso) */
    assert(!z->writing); /* Non ci devono essere writer  */
    z->readers--;
    if (z->readers == 0) pthread_cond_signal(&z->cond); /* Segnala un solo writer */
    pthread_mutex_unlock(&z->mu);
}

/* Inizio uso da parte di un writer */
void write_lock(rw *z)
{
    fprintf(stderr, "%2d richiesta write\n", gettid()%100);
    pthread_mutex_lock(&z->mu);
    while (z->writing || z->readers > 0) pthread_cond_wait(&z->cond, &z->mu);
    z->writing = true;
    pthread_mutex_unlock(&z->mu);
}

void write_unlock(rw *z)
{
    fprintf(stderr, "%2dwrite completata\n", gettid()%100);
    pthread_mutex_lock(&z->mu);
    assert(z->writing);
    z->writing = false;
    pthread_cond_broadcast(&z->cond);
    pthread_mutex_unlock(&z->mu);
} 


void *lettore(void *arg)
{
  rw *z = (rw *)arg;
  read_lock(z);
  sleep(1);
  read_unlock(z);
  return NULL;
}

void *scrittore(void *arg)
{
  rw *z = (rw *)arg;
  write_lock(z);
  sleep(1);
  write_unlock(z);
  return NULL;
}



int main(int argc, char *argv[])
{
    rw z;
    rw_init(&z);
    // thread che usano la rw
    pthread_t t[20];
    int i=0;

    xpthread_create(&t[i++],NULL, &lettore, &z, QUI);
    xpthread_create(&t[i++],NULL, &lettore, &z, QUI);
    xpthread_create(&t[i++],NULL, &scrittore, &z, QUI);
    xpthread_create(&t[i++],NULL, &lettore, &z, QUI);
    xpthread_create(&t[i++],NULL, &lettore, &z, QUI);
    xpthread_create(&t[i++],NULL, &lettore, &z, QUI);
    xpthread_create(&t[i++],NULL, &lettore, &z, QUI);
    xpthread_create(&t[i++],NULL, &lettore, &z, QUI);
    xpthread_create(&t[i++],NULL, &lettore, &z, QUI);
    xpthread_create(&t[i++],NULL, &scrittore, &z, QUI);
    xpthread_create(&t[i++],NULL, &lettore, &z, QUI);
    xpthread_create(&t[i++],NULL, &lettore, &z, QUI);
    xpthread_create(&t[i++],NULL, &lettore, &z, QUI);
    xpthread_create(&t[i++],NULL, &lettore, &z, QUI);

    /* attendo tutti i thread altrimenti terminano tutti */
    for(int j=0; j<i; j++) xpthread_join(t[j],NULL,QUI);

    assert(z.writing==false);
    assert(z.readers==0);
    return 0;
}
