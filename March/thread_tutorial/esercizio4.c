/*  
      Scrivere un programma C che crea 5 thread produttori e 5 thread cosumatori:
 
    • I thread condividono una coda circolare implementata tramite un array di 100 interi.
    • Ogni thread produttore
        – genera cinque numeri casuali (funzione random())
        – Inserisce gli interi in coda
    • Ogni thread consumatore
        – Estrae un elemento dalla coda e lo visualizza.
    • Tutti i thread, dopo l'operazione sulla variabile condivisa dormono per un
    secondo.

    Utilizzare una condition variable per la sincronizzazione.
    Fermare il programma quando sono stati consumati tutti i 100 interi
 
*/

#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <threads.h>
#include <unistd.h>
#include <time.h>
#include <stdbool.h>

#define BUFFER_SIZE 100
#define THREAD_NUM 10
#define CONSUMER_THREADS 5
#define PRODUCER_THREADS 5
#define TOT_ITEMS 100


// generic consumer structure
typedef struct cons
{
    pthread_cond_t *cv; // condition variable
    pthread_mutex_t *mu; // mutual exclusion
    int *buffQueue; // shared buffer
    int *head;
    int *tail;
    int *count;
    int *consumed;
} cons;

typedef struct prod 
{
    pthread_mutex_t *mu;
    pthread_cond_t *cv;
    int *buffQueue;
    int *head;
    int *tail;
    int *count;
    int *produced;
} prod;

void *consumer(void *args)
{
    cons *th = (cons*)args;
    // always consuming
    while (true)
    {
        pthread_mutex_lock(th->mu);
        // check if the buffer has been fully consumed
        if ((*th->consumed) == TOT_ITEMS)
        {
            pthread_mutex_unlock(th->mu);
            break;
        }

        // waiting for at least one element inside the buffer
        while (*th->count == 0) // empty buffer
        {
            printf("[Consumer] Waiting for at least one element inside the buffer...\n");
            pthread_cond_wait(th->cv, th->mu);
        }

        // pop first element
        int x = th->buffQueue[*th->head];    
        printf("[Consumer] Popped from queue %d\n", x);
        (*th->head) = (*(th->head) + 1) % BUFFER_SIZE; // incrementing the head counter
        (*th->count)--; // decrementing counter
        (*th->consumed)++;

        pthread_mutex_unlock(th->mu); // release mutual exclusion
        pthread_cond_signal(th->cv); // waking up a thread
        // pthread_cond_broadcast(th->cv);
        sleep(1);
    }
    return NULL;
}

void *producer(void *args)
{
    prod *pdThread = (prod*)args;
    unsigned int seed = time(NULL);

    // always producing
    while (true)
    {
        if ((*pdThread->produced) == TOT_ITEMS)
        {
            break;
        }

        // generates five random numbers
        for (int i = 0; i < 5; ++i)
        {
            int y = (rand_r(&seed) % 5) + 1;
            // mutual exclusion
            pthread_mutex_lock(pdThread->mu);

            while (*pdThread->count == BUFFER_SIZE)
            {
                printf("Buffer full! Waiting for threads to pop something...\n");
                pthread_cond_wait(pdThread->cv, pdThread->mu);
            }

            // insert the integer into the buffer
            printf("[Producer] Inserting number %d\n", y);
            pdThread->buffQueue[*pdThread->tail] = y;
            (*pdThread->tail) = (*(pdThread->tail) + 1) % BUFFER_SIZE; // incrementing the tail counter
            (*pdThread->count)++;
            (*pdThread->produced)++;

            pthread_mutex_unlock(pdThread->mu);
            pthread_cond_signal(pdThread->cv);
            // pthread_cond_broadcast(pdThread->cv);
            sleep(1);

        }
    } 
    return NULL;
}

int main(int argc, char **argv)
{
    srand(time(NULL));
    pthread_t *threadArr = malloc(THREAD_NUM * sizeof(pthread_t));
    cons *thCons = malloc(CONSUMER_THREADS * sizeof(cons));
    prod *thProd = malloc(PRODUCER_THREADS * sizeof(prod));
    int *buffer = malloc(BUFFER_SIZE * sizeof(int));

    if (!threadArr || !thCons || !thProd || !buffer)
    {
        fprintf(stderr, "Failed malloc\n");
        exit(EXIT_FAILURE);
    }

    pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
    pthread_cond_t condVar = PTHREAD_COND_INITIALIZER;
    int tailCounter = 0;
    int headCounter = 0;
    int countElements = 0;
    int produced = 0;
    int consumed = 0;

    // populating producer and consumer structs
    for (int i = 0; i < CONSUMER_THREADS; i++)
    {
        thCons[i].tail= &tailCounter;
        thCons[i].head= &headCounter;
        thCons[i].buffQueue = buffer;
        thCons[i].cv = &condVar;
        thCons[i].mu = &mutex;
        thCons[i].count = &countElements;
        thCons[i].consumed = &consumed;
    }

    for (int i = 0; i < PRODUCER_THREADS; i++)
    {
        thProd[i].tail = &tailCounter;
        thProd[i].head = &headCounter;
        thProd[i].buffQueue = buffer;
        thProd[i].cv = &condVar;
        thProd[i].mu = &mutex;
        thProd[i].count = &countElements;
        thProd[i].produced = &produced;
    }

    for (int i = 0; i < CONSUMER_THREADS; i++)
    {
        if (pthread_create(&threadArr[i], NULL, &consumer, &thCons[i]) != 0)
        {
            perror("Error creating consumer thread");
            exit(1);
        }
    }

    for (int i = 0; i < PRODUCER_THREADS; i++)
    {
        if (pthread_create(&threadArr[i + CONSUMER_THREADS], NULL, &producer, &thProd[i]) != 0)
        {
            perror("Error creating producer thread");
            exit(1);
        }
    }

    for (int j = 0; j < THREAD_NUM; ++j)
    {
        if (pthread_join(threadArr[j], NULL) != 0)
        {
            perror("Error joining threads");
            exit(2);
        }
    }

    // freeing resources
    free(thCons);
    free(thProd);
    free(buffer);
    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&condVar);

    return 0;
}



