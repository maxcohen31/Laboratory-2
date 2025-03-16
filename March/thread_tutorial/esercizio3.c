/* 
    Scrivere un programma C che prenda come ingresso - da linea di comando - N interi compresi tra 0 e 10. 
	N è una costante, e.g. 5. Il programma crea N thread:
	a) Ciascun thread va in sospensione per un numero di secondi pari al valore del corrispondente parametro e poi stampa il suo indice.
	b) Al risveglio ciascun thread stampa un messaggio: "Thread #: Woken up!".
	c) Dopodichè i thread si sincronizzano, ossia aspettano l'ultimo thread. 
	d) Dopo essersi sincronizzati, ciascun thread stampa un messaggio di chiusura ("Thread #: closing...").
	Si implementi il punto c prima con i semafori classici (semafori di sincronizzazione) e poi con le variabili condition.
	Nota: la funzione sleep(int n) sospende il thread corrente per un numero n di secondi.
*/

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>


#define N 5

// #define CONDVAR 1

#ifdef CONDVAR 
#warning "Using cond. variable"
#else
#warning "Using semaphores"
#endif


// struct representing a generic thread 
typedef struct 
{
    pthread_mutex_t *mu;
#ifndef CONDVAR
    sem_t *semaphore;
#else
    pthread_cond_t *cv;
#endif 
    int *readyThreads;
    int sleepTime;
} thread;

// thread body
void *routine(void *args)
{
    thread *t = (thread*)args;
    // acquiring lock
    pthread_mutex_lock(t->mu);
    sleep(t->sleepTime);
    printf("Thread [%d]: Woken up!\n", gettid());
    (*t->readyThreads)++;
#ifdef CONDVAR
    if (*t->readyThreads == N)
    {
        pthread_cond_broadcast(t->cv);
    }
    while (*t->readyThreads < N)
    {
        pthread_cond_wait(t->cv, t->mu);
    }
#else
    if (*t->readyThreads == N)
    {
        // unlock all threads
        for (int i = 0; i < N; ++i)
        {
            sem_post(t->semaphore);
        }
    }
    pthread_mutex_unlock(t->mu);
    // waiting for the threads to be ready
    sem_wait(t->semaphore);
    pthread_mutex_lock(t->mu);
#endif

    printf("Thread [%d]: Closing...\n", gettid());
    pthread_mutex_unlock(t->mu);
    return NULL;
}


int main(int argc, char **argv)
{
    if (argc != N + 1)
    {
        printf("Usage: <program executable> <time1> <time2> .... <timeN>\n");
        exit(1);
    }

    pthread_t threads[N];
    thread thd[N];
    sem_t sem;
    int readyThreads = 0;
    pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
    sem_init(&sem, 0, 0);
#ifdef CONDVAR
    pthread_cond_t condVar = PTHREAD_COND_INITIALIZER;

#endif /* ifdef CONDVAR */

    for (int j = 0; j < N; ++j)
    {
        // check for valid sleep time
        int sleepTime = atoi(argv[j + 1]);
        if (sleepTime < 0 || sleepTime > 10)
        {
            fprintf(stderr, "Invalid sleep time\n");
            exit(2);
        }
        // populate thd array
        thd[j].mu = &mutex;
#ifndef CONDVAR
        thd[j].semaphore = &sem;
#else
        thd[j].cv = &condVar;
#endif
        thd[j].readyThreads = &readyThreads;
        thd[j].sleepTime = sleepTime;
    }

    for (int i = 0; i < N; ++i)
    {
        if (pthread_create(&threads[i], NULL, &routine, (void*)&thd[i]) != 0)
        {
            perror("Error creating thread\n");
        }
    }

    for (int i = 0; i < N; ++i)
    {
        if (pthread_join(threads[i], NULL) != 0)
        {
            perror("Error joining threads\n");
        }
    }

#ifdef CONDVAR
    pthread_cond_destroy(&condVar);
#else
    sem_destroy(&sem);
#endif 
    pthread_mutex_destroy(&mutex);
    
    return 0;
}
