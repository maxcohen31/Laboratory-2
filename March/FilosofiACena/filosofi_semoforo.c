/*
    #########################################################################                                                                           #
    #    Solution to the dining philosophers problem using semaphores       #
    #    Abbate Emanuele - Spring 2025                                      #
    #                                                                       #
    ########################################################################
*/

#define _GNU_SOURCE
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <stdbool.h>
#include <semaphore.h>

#define NUM_PHIL 5
#define DINNER_TIME 50

// struct representing the generic thread
typedef struct 
{
    int id;
    sem_t *fork; // semaphores array

} thread;

// Utils functions
// eat -> simulate the act of eating
// void -> simulate the act of thinking
void eat(int threadID, unsigned int *seed)
{
    printf("[%d] I'm eating...\n", threadID);
    // return a value between 0 and RAND_MAX
    long r = rand_r(seed) % 100000;
    // time interval
    struct timespec t = {0, r};
    nanosleep(&t, NULL); 
    fprintf(stdout, "[%d] Done eating!\n", threadID);
}

void think(int threadID, unsigned int *seed)
{
    fprintf(stdout, "[%d] I'm thinking...\n", threadID);
    long r = rand_r(seed) % 100000;
    struct timespec t = {0, r};
    nanosleep(&t, NULL);
    fprintf(stdout, "[%d] Done thinking!...\n", threadID);
}

void *philosopher(void *args)
{
    thread *th = (thread*)args; 
    int left = th->id - 1;
    int right = (th->id) % NUM_PHIL;
    int eatCount = 0;
    int thinkCount = 0;
    unsigned int seed = th->id * time(NULL); // different seed for each phil
    sem_t leftFork = th->fork[left];
    sem_t rightFork = th->fork[right];

    for (int i = 0; i < DINNER_TIME; ++i)
    {
        think(th->id, &seed);
        thinkCount++;

        // odd and even phils pick up the forks in different order
        // the odd one picks up the fork for first
        if (th->id % 2 == 0)
        {
            sem_wait(&leftFork);
            printf("Left fork acquired. Waiting for right fork to come along...\n");
            sem_wait(&rightFork);
            printf("Right fork acquired");
        }
        else
        {
            sem_wait(&rightFork);
            printf("Right fork acquired. Waiting for left fork to come along...\n");
            sem_wait(&leftFork);
            printf("Left fork acquired\n");
        }

        eatCount++;
        eat(th->id, &seed);

        // incrementing the semaphores
        sem_post(&leftFork);
        sem_post(&rightFork);
    }

    fprintf(stdout, "Phil [%d] has eaten %d times and thought %d times\n", th->id, eatCount, thinkCount);
    fflush(stdout);
    return NULL;

}

int main(int argc, char **argv)
{
    // pthread_t philThread[NUM_PHIL];
    pthread_t *philThread = malloc(sizeof(pthread_t) * NUM_PHIL);
    thread *arrThread = malloc(sizeof(thread) * NUM_PHIL);
    sem_t *forks = malloc(sizeof(sem_t) * NUM_PHIL);
    if (!philThread || !arrThread || !forks)
    {
        fprintf(stderr, "Failed malloc\n");
        exit(EXIT_FAILURE);
    }
     
    // initializing forks sem
    for (int i = 0; i < NUM_PHIL; i++)
    {
        sem_init(&forks[i], 0, 1); // 1 -> fork is avaiable
    }

    for (int i = 0; i < NUM_PHIL; i++)
    {
        arrThread[i].id = i + 1; 
        arrThread[i].fork = forks;
    }

    for (int i = 0; i < NUM_PHIL; i++)
    {
        if (pthread_create(&philThread[i], NULL, &philosopher, &arrThread[i]) != 0)
        {
            perror("Error creating thread\n");
            exit(EXIT_FAILURE);
        }
    }

    for (int i = 0; i < NUM_PHIL; i++)
    {
        if (pthread_join(philThread[i], NULL) != 0)
        {
            perror("Error joining threads\n");
            exit(EXIT_FAILURE);
        }
    }

    // free reesources
    free(philThread);
    free(arrThread);
    for (int i = 0; i < NUM_PHIL; i++)
    {
        sem_destroy(&forks[i]);
    }
    free(forks);

    return 0;
}
