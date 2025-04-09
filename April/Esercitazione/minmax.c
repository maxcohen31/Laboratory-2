#define _GNU_SOURCE
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <pthread.h>
#include <limits.h>

#define BUFF_SIZE 10
#define BLOCK_SIZE 20

typedef struct 
{
    int32_t **buffer; // array of block pointers
                  // this is the shared buffer
    char *filename;
    int *threadDone; // counts the threads that have completed the task 
    int *count; // elements into buffer
    int *in; // element in
    int *out; // element out
    pthread_mutex_t *mu; // mutual exclusion
    pthread_cond_t *isEmpty; // cond. variable for empty buffer
    pthread_cond_t *isFull; // cond. variable for full buffer
    int threadCreated;
} threadGenerics;

// struct used by the consumer thread
typedef struct 
{
    int32_t min;
    int32_t max;
} result;

void *producer(void *args)
{
    threadGenerics *pr = (threadGenerics*)args;
    FILE *fl = fopen(pr->filename, "rb");
    if (fl == NULL)
    {
        fprintf(stderr, "Error opening file\n");
        return NULL;
    }
    
    // main loop
    while (true)
    {
        // allocate a block of 20 integers
        int32_t *block = malloc(BLOCK_SIZE * sizeof(int32_t));
        if (block == NULL)
        {
            fprintf(stderr, "Error allocating block memory\n");
            fclose(fl);
            return NULL;
        }

        // read a block of up to 20 integers
        size_t readCount = fread(block, sizeof(int32_t), BLOCK_SIZE, fl);
        if (readCount == 0)
        {
            free(block);
            break;
        }

        // fill the rest of the block with the first value
        for (int i = readCount; i < BLOCK_SIZE; ++i)
        {
            block[i] = block[0];
        }

        // the block has to be put into the buffer
        pthread_mutex_lock(pr->mu);
        // check if bufer is full
        while (*(pr->count) == BUFF_SIZE)
        {
            pthread_cond_wait(pr->isFull, pr->mu);
        }
        pr->buffer[*(pr->in)] = block;
        *(pr->in) = (*(pr->in) + 1) % BUFF_SIZE;
        (*(pr->count))++;

        pthread_cond_signal(pr->isEmpty);
        pthread_mutex_unlock(pr->mu);
    }

    fclose(fl);

    // update threadDone variable -> used to tell the geric thread i'm done :)
    pthread_mutex_lock(pr->mu);
    (*(pr->threadDone))++;
    pthread_cond_signal(pr->isEmpty);
    pthread_mutex_unlock(pr->mu);

    pthread_exit(NULL);

}

void *consumer(void *args)
{
    threadGenerics *cons = (threadGenerics*)args;
    int min = INT_MAX;
    int max = INT_MIN;
    result *r = malloc(sizeof(result)); // the pointer must live!
    if (r == NULL)
    {
        fprintf(stderr, "Error allocating memory in consumer function\n");
        exit(EXIT_FAILURE);
    }

    while (true)
    {
        pthread_mutex_lock(cons->mu);
        while (*(cons->count) == 0 && cons->threadCreated > *(cons->threadDone))
        {
            pthread_cond_wait(cons->isEmpty, cons->mu);
        }
        // check if buffer is empty and for all the threads to have finished
        // if the condition is met the program finishes
        if ((*(cons->count) == 0 && *(cons->threadDone) >= cons->threadCreated))
        {
            pthread_mutex_unlock(cons->mu);
            break;
        }

        // get a block from the buffer
        int32_t *blockConsumer = cons->buffer[*(cons->out)];
        // update out index
        *(cons->out) = (*(cons->out) + 1) % BUFF_SIZE;
        // update total count of elements into the buffer
        (*(cons->count))--;

        pthread_cond_signal(cons->isFull);
        pthread_mutex_unlock(cons->mu);

        // find max and min in each block
        for (int i = 0; i < BLOCK_SIZE; ++i)
        {
            if (blockConsumer[i] > max)
            {
                max = blockConsumer[i];
            }
            if (blockConsumer[i] < min)
            {
                min = blockConsumer[i];
            }
        }
        free(blockConsumer);
    }

    r->min = min;
    r->max = max;

    return r;
}


int main(int argc, char **argv)
{
    if (argc < 2)
    {
        fprintf(stderr, "Usage: %s file1 ... fileN\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    // variables
    int numOfFiles = argc - 1; // number of files
    int32_t **sharedBuffer = malloc(BUFF_SIZE * sizeof(int32_t*));
    int total = 0;
    int inBuff = 0;
    int outBuf = 0;
    pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
    pthread_cond_t empty = PTHREAD_COND_INITIALIZER;
    pthread_cond_t full = PTHREAD_COND_INITIALIZER;
    int threadDone = 0;

   // array of threads -> use dynamic allocation for larger input sets
    pthread_t *th = malloc(numOfFiles * sizeof(pthread_t));
    if (th == NULL) 
    {
        fprintf(stderr, "Error allocating thread array\n");
        free(sharedBuffer);
        exit(EXIT_FAILURE);
    }
    // consumer's struct
    threadGenerics consumerStruct;
    // array of threadGenerics structs
    threadGenerics *thGen = malloc(numOfFiles * sizeof(threadGenerics));

    pthread_t consumerThread;
    consumerStruct.buffer = sharedBuffer;
    consumerStruct.mu = &mutex;
    consumerStruct.isFull = &full;
    consumerStruct.isEmpty = &empty;
    consumerStruct.count = &total;
    consumerStruct.in = &inBuff;
    consumerStruct.out = &outBuf;
    consumerStruct.filename = NULL;
    consumerStruct.threadCreated = numOfFiles;
    consumerStruct.threadDone = &threadDone;
    
    // populate the array of pointers to structs (threadGenerics)
    for (int i = 0; i < numOfFiles; ++i)
    {
        thGen[i].mu = &mutex;
        thGen[i].buffer = sharedBuffer;
        thGen[i].isFull = &full;
        thGen[i].isEmpty= &empty;
        thGen[i].count = &total;
        thGen[i].in = &inBuff;
        thGen[i].out = &outBuf;
        thGen[i].threadCreated = numOfFiles;
        thGen[i].threadDone = &threadDone;
        thGen[i].filename = argv[i + 1];
        
        // create producers threads
        if (pthread_create(&th[i], NULL, &producer, &thGen[i]) != 0)
        {
            perror("Error creating producer thread\n");
            free(sharedBuffer);
            free(th);
            free(thGen);
            exit(1);
        }
    }

    // create consumer thread
    if (pthread_create(&consumerThread, NULL, &consumer, &consumerStruct) != 0)
    {
        perror("Errore creazione thread consumatore\n");
        free(sharedBuffer);
        free(th);
        free(thGen);
        exit(1);
    }

    for (int i = 0; i < numOfFiles; ++i)
    {
        if (pthread_join(th[i], NULL) != 0)
        {
            perror("Error joining producer thread\n");
            exit(1);
        }
    }
    // create a struct result and return the values of it
    // res is the second argument of pthread_join
    void *res;

    if (pthread_join(consumerThread, &res) != 0)
    {
        perror("Error returnng result!\n");
        exit(2);
    }
    // casting res to result type to retrieve the values
    result *r = (result*)res;

    printf("Max: %d\n", r->max);
    printf("Min: %d\n", r->min); 
    fflush(stdout);

    // free resources
    free(thGen);
    free(th); 
    free(sharedBuffer);
    free(res);
    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&empty);
    pthread_cond_destroy(&full);

    return 0;

}
