#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <semaphore.h>
#include <time.h>
#include <stdbool.h>

/*
    PROBLEMA PRODUTTORE - CONSUMATORE

    Abbiamo un buffer condiviso da più thread.
    Abbiamo poi i produttori che inseriscono dati nel buffer e i consumatori che li estraggono.
    
    Fondamentalmente ci sono tre problemi che dobbiamo risolvere:
    
    1. gestire gli accessi alla memoria condivisa
    2. controllare se il buffer è pieno (p > c)
    3. controllare che il buffer sia vuoto (p < c)
*/

#define THREAD_NUM 2

int buffer[10];
int count = 0;
pthread_mutex_t bufferMutex; // anche con un mutex si arriva ad un segmentation fault. Come risolviamo? Con un if
                             // tuttuvia non basta: usiamo dei semafori 
sem_t semaphoreEmpty;
sem_t semaphoreFull;

void *producer(void *args)
{
    // produce di continuo
    while (true)
    {
        // produce
        int x = rand() % 100;
        sem_wait(&semaphoreEmpty);
        pthread_mutex_lock(&bufferMutex);
        if (count < 10)
        {
            // aggiunge elemento al buffer
            buffer[count] = x;
            count++;
        }
        pthread_mutex_unlock(&bufferMutex);
        sem_post(&semaphoreFull);
    }
}

void *consumer(void *args)
{
    // consuma di continuo
    while (true)
    {
        int y = -1;
        sem_wait(&semaphoreFull);
        pthread_mutex_lock(&bufferMutex);
        // questo if non serve più: perchè se siamo su sem_wait allora semaphoreEmpty deve essere almeno 1
        // if (count > 0)
        // {
        //     // remove from buffer
        //     y = buffer[count - 1];
        //     count--;
        // }
        y = buffer[count - 1];
        count--;
        // consume
        printf("Numero prelevato %d\n", y);
        pthread_mutex_unlock(&bufferMutex);
        sem_post(&semaphoreEmpty);
    }
}

int main(int argc, char **args)
{
    srand(time(NULL));
    pthread_t th[THREAD_NUM];
    sem_init(&semaphoreEmpty, 0, 10); // 10 è il numero di slot liberi all'inizio
    sem_init(&semaphoreFull, 0, 0);
    int i;

    for (i = 0; i < THREAD_NUM; ++i)
    {
        // metà thread saranno produttori e l'altra metà consumatori
        if (i % 2 == 0)
        {
            if (pthread_create(&th[i], NULL, &producer, NULL) != 0)
            {
                perror("Creazione thread fallita\n");
            }
            else
            {
                if (pthread_create(&th[i], NULL, &consumer, NULL) != 0)
                {
                    perror("Creazione thread fallita\n");
                }
            }
        }
    }

    for (i = 0; i < THREAD_NUM; ++i)
    {
        if (pthread_join(th[i], NULL) != 0)
        {
            perror("Join thread fallita");
        }
    }

    sem_destroy(&semaphoreEmpty);
    sem_destroy(&semaphoreFull);
    return 0;
}
