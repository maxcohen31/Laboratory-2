/*
    CASO 4: Più produttori e più consumatori
    Utilizziamo semafori per la sincronizzazione e un mutex per il buffer
*/

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

#define BUFFER_SIZE 10
#define NUM_PRODUCERS 3
#define NUM_CONSUMERS 4
#define NUM_ITEMS 24

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

void producer_insert(int item, int producer_id) 
{
    sem_wait(&shared_buffer.empty);
    pthread_mutex_lock(&shared_buffer.mutex);

    // Inserimento nel buffer
    shared_buffer.buffer[shared_buffer.in] = item;
    shared_buffer.in = (shared_buffer.in + 1) % BUFFER_SIZE;
    printf("Produttore %d: inserito %d\n", producer_id, item);

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
    int producer_id = *((int*)arg);
    int items_to_produce = NUM_ITEMS / NUM_PRODUCERS;
    int i;
    
    for (i = 0; i < items_to_produce; i++) 
    {
        int item = producer_id * 100 + i; // Produciamo un item identificabile
        producer_insert(item, producer_id);
        usleep(rand() % 200000); // Tempo variabile di produzione
    }
    return NULL;
}

void* consumer_function(void* arg) 
{
    int consumer_id = *((int*)arg);
    int items_to_consume = NUM_ITEMS / NUM_CONSUMERS;
    int i;
    
    for (i = 0; i < items_to_consume; i++) 
    {
        int item = consumer_remove(consumer_id);
        usleep(rand() % 300000); // Tempo variabile di consumo
    }
    return NULL;
}

int main(int argc, char **argv) 
{
    pthread_t producers[NUM_PRODUCERS], consumers[NUM_CONSUMERS];
    int producer_ids[NUM_PRODUCERS], consumer_ids[NUM_CONSUMERS];
    int i;
    
    init_buffer();
    
    for (i = 0; i < NUM_PRODUCERS; i++) 
    {
        producer_ids[i] = i;
        pthread_create(&producers[i], NULL, producer_function, &producer_ids[i]);
    }
    
    for (i = 0; i < NUM_CONSUMERS; i++) 
    {
        consumer_ids[i] = i;
        pthread_create(&consumers[i], NULL, consumer_function, &consumer_ids[i]);
    }
    
    for (i = 0; i < NUM_PRODUCERS; i++) 
    {
        pthread_join(producers[i], NULL);
    }
    
    for (i = 0; i < NUM_CONSUMERS; i++) 
    {
        pthread_join(consumers[i], NULL);
    }
    
    sem_destroy(&shared_buffer.empty);
    sem_destroy(&shared_buffer.full);
    pthread_mutex_destroy(&shared_buffer.mutex);
    
    return 0;
}
