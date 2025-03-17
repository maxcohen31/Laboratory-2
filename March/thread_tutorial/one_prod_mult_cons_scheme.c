/*
    CASO 2: Un produttore e più consumatori
    Utilizziamo semafori per la sincronizzazione e un mutex per il buffer
*/


#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

#define BUFFER_SIZE 5
#define NUM_CONSUMERS 3
#define NUM_ITEMS 15

typedef struct 
{
    int buffer[BUFFER_SIZE];
    int in;
    int out;
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

void producer_insert(int item) 
{
    sem_wait(&shared_buffer.empty);
    pthread_mutex_lock(&shared_buffer.mutex);

    // Inserimento nel buffer
    shared_buffer.buffer[shared_buffer.in] = item;
    shared_buffer.in = (shared_buffer.in + 1) % BUFFER_SIZE;
    printf("Produttore: inserito %d\n", item);

    pthread_mutex_unlock(&shared_buffer.mutex);
    sem_post(&shared_buffer.full);
}

int consumer_remove(int consumer_id) 
{
    int item;
    
    sem_wait(&shared_buffer.full);
    pthread_mutex_lock(&shared_buffer.mutex);

    // Prelievo dal buffer
    item = shared_buffer.buffer[shared_buffer.out];
    shared_buffer.out = (shared_buffer.out + 1) % BUFFER_SIZE;
    printf("Consumatore %d: prelevato %d\n", consumer_id, item);

    pthread_mutex_unlock(&shared_buffer.mutex);
    sem_post(&shared_buffer.empty);

    return item;
}

void* producer_function(void* arg) 
{
    int i;
    
    for (i = 0; i < NUM_ITEMS; i++) 
    {
        producer_insert(i);
        usleep(rand() % 100000); // Tempo variabile di produzione
    }
    return NULL;
}

void* consumer_function(void* arg) 
{
    int consumer_id = *((int*)arg);
    int count = 0;
    
    // Ogni consumatore consuma un numero variabile di elementi
    while (count < NUM_ITEMS / NUM_CONSUMERS) 
    {
        int item = consumer_remove(consumer_id);
        usleep(rand() % 300000); // Tempo variabile di consumo
        count++;
    }
    return NULL;
}

int main(int argc, char **argv) 
{
    pthread_t producer, consumers[NUM_CONSUMERS];
    int consumer_ids[NUM_CONSUMERS];
    int i;
    
    init_buffer();
    
    pthread_create(&producer, NULL, producer_function, NULL);
    
    for (i = 0; i < NUM_CONSUMERS; i++) 
    {
        consumer_ids[i] = i;
        pthread_create(&consumers[i], NULL, consumer_function, &consumer_ids[i]);
    }
    
    pthread_join(producer, NULL);
    for (i = 0; i < NUM_CONSUMERS; i++) 
    {
        pthread_join(consumers[i], NULL);
    }
    
    sem_destroy(&shared_buffer.empty);
    sem_destroy(&shared_buffer.full);
    pthread_mutex_destroy(&shared_buffer.mutex);
    
    return 0;
}
