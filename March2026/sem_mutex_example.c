/*
 *  Credits to: https://mohitmishra786.github.io/exploring-os/src/day-12-mutex-and-semaphores.html
 * */


#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>

#define BUFFER_SIZE 10
#define PRODUCER_COUNT 3
#define CONSUMER_COUNT 2

typedef struct {
    int data[BUFFER_SIZE];
    int count;
    pthread_mutex_t mutex;
    sem_t empty_slots;
    sem_t filled_slots;
} SharedBuffer;

SharedBuffer buffer = {0};

void initialize_buffer(SharedBuffer* buf) {
    buf->count = 0;
    pthread_mutex_init(&buf->mutex, NULL);
    sem_init(&buf->empty_slots, 0, BUFFER_SIZE);
    sem_init(&buf->filled_slots, 0, 0);
}

void produce_item(SharedBuffer* buf, int item) {
    // Wait for an empty slot
    sem_wait(&buf->empty_slots);
    
    // Acquire mutex to safely modify buffer
    pthread_mutex_lock(&buf->mutex);
    
    buf->data[buf->count++] = item;
    printf("Produced: %d (Buffer: %d)\n", item, buf->count);
    
    // Release mutex
    pthread_mutex_unlock(&buf->mutex);
    
    // Signal that a new item is available
    sem_post(&buf->filled_slots);
}

int consume_item(SharedBuffer* buf) {
    // Wait for a filled slot
    sem_wait(&buf->filled_slots);
    
    // Acquire mutex to safely read from buffer
    pthread_mutex_lock(&buf->mutex);
    
    int item = buf->data[--buf->count];
    printf("Consumed: %d (Buffer: %d)\n", item, buf->count);
    
    // Release mutex
    pthread_mutex_unlock(&buf->mutex);
    
    // Signal that an empty slot is now available
    sem_post(&buf->empty_slots);
    
    return item;
}

void* producer_thread(void* arg) {
    for (int i = 0; i < 5; i++) {
        produce_item(&buffer, i * 100);
    }
    return NULL;
}

void* consumer_thread(void* arg) {
    for (int i = 0; i < 5; i++) {
        consume_item(&buffer);
    }
    return NULL;
}

int main() {
    pthread_t producers[PRODUCER_COUNT];
    pthread_t consumers[CONSUMER_COUNT];

    initialize_buffer(&buffer);

    // Create producer and consumer threads
    for (int i = 0; i < PRODUCER_COUNT; i++) {
        pthread_create(&producers[i], NULL, producer_thread, NULL);
    }

    for (int i = 0; i < CONSUMER_COUNT; i++) {
        pthread_create(&consumers[i], NULL, consumer_thread, NULL);
    }

    // Wait for all threads to complete
    for (int i = 0; i < PRODUCER_COUNT; i++) {
        pthread_join(producers[i], NULL);
    }

    for (int i = 0; i < CONSUMER_COUNT; i++) {
        pthread_join(consumers[i], NULL);
    }

    return 0;
}
