#define _GNU_SOURCE
#include <sys/types.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <semaphore.h>

#define THREAD_NUM 16

/*
        SEMAFORI - UN'APPLICAZIONE (LOGIN QUEUE)
        
        Abbiamo un server che può ospitare fino a 12 giocatori loggati.
        I giocatori totali sono però 16

*/

sem_t semaph; 

void *routine(void *args)
{
    printf("(%d) Aspettando nella login queue\n", *(int*)args);
    sem_wait(&semaph);
    printf("(%d) Loggato!\n", *(int*)args);
    sleep(rand() % 5 + 1); // tempo logout
    printf("(%d) Sloggato!\n", *(int*)args);
    sleep(1);
    sem_post(&semaph);
    free(args);
}


int main(int argc, char **argv)
{
    pthread_t th[THREAD_NUM];
    sem_init(&semaph, 0, 12);
    int i;

    for (int i = 0; i < THREAD_NUM; ++i) 
    {
        int *a = malloc(sizeof(int));
        *a = i;
        if (pthread_create(&th[i], NULL, &routine, a) != 0)
        {
            perror("Thread non creato");
        }
    }

    for (i = 0; i < THREAD_NUM; ++i)
    {
        if (pthread_join(th[i], NULL) != 0)
        {
            perror("Join fallita");
        }
    }

    sem_destroy(&semaph);
    return 0;
}
