/*
    Un produttore e un consumatore 
    utilizziamo un buffer circolare con semafori per la sincronizzazione
*/

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

#define BUFFER_SIZE 5

typedef struct
{
    int buffer[BUFFER_SIZE];
    int in;    // Indice di inserimento
    int out;   // Indice di estrazione
    sem_t empty;
    sem_t full;
    pthread_mutex_t mutex;
} shared_buffer_t;

shared_buffer_t shared_buffer;

void init_buffer() 
{
    shared_buffer.in = 0;
    shared_buffer.out = 0;
    sem_init(&shared_buffer.empty, 0, BUFFER_SIZE);
    sem_init(&shared_buffer.full, 0, 0);
    pthread_mutex_init(&shared_buffer.mutex, NULL);
}

void producer_insert(int item) {
    sem_wait(&shared_buffer.empty);
    pthread_mutex_lock(&shared_buffer.mutex);

    // Inserimento nel buffer
    shared_buffer.buffer[shared_buffer.in] = item;
    shared_buffer.in = (shared_buffer.in + 1) % BUFFER_SIZE;
    printf("Produttore: inserito %d\n", item);

    pthread_mutex_unlock(&shared_buffer.mutex);
    sem_post(&shared_buffer.full);
}

int consumer_remove() 
{
    int item;
    
    sem_wait(&shared_buffer.full);
    pthread_mutex_lock(&shared_buffer.mutex);

    // Prelievo dal buffer
    item = shared_buffer.buffer[shared_buffer.out];
    shared_buffer.out = (shared_buffer.out + 1) % BUFFER_SIZE;
    printf("Consumatore: prelevato %d\n", item);

    pthread_mutex_unlock(&shared_buffer.mutex);
    sem_post(&shared_buffer.empty);

    return item;
}

void* producer_function(void* arg) 
{
    int i;
    for (i = 0; i < 10; i++) 
    {
        producer_insert(i);
        sleep(rand() % 2); // Simuliamo un tempo variabile di produzione
    }
    return NULL;
}

void* consumer_function(void* arg) 
{
    int i;
    for (i = 0; i < 10; i++) 
    {
        int item = consumer_remove();
        sleep(rand() % 2); // Simuliamo un tempo variabile di consumo
    }
    return NULL;
}

int main(int argc, char **argv) 
{
    pthread_t producer, consumer;
    
    init_buffer();
    
    pthread_create(&producer, NULL, producer_function, NULL);
    pthread_create(&consumer, NULL, consumer_function, NULL);
    
    pthread_join(producer, NULL);
    pthread_join(consumer, NULL);
    
    sem_destroy(&shared_buffer.empty);
    sem_destroy(&shared_buffer.full);
    pthread_mutex_destroy(&shared_buffer.mutex);
    
    return 0;
}
