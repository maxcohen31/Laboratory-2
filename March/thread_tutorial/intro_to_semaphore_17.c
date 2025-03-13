#define _GNU_SOURCE
#include <sys/types.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <semaphore.h>

#define THREAD_NUM 4
#define gettid() syscall(SYS_gettid)

/*
        SEMAFORI

        Due operazioni: 
          
                - WAIT: simile alla lock
                - POST: simile alla unlock

        SEM_WAIT: controlla il valore del semoforo. Se è 0 il thread si mette in attesa. Se il valore è per esempio 5
                   allora sarà decrementato e verrà eseguita la prossima linea di codice.

        SEM_POST: incrementa il valore del semaforo. Non attende nessuno.
*/

sem_t semaph; 

void *routine(void *args)
{
    // printf("Ciao dal thread %lu\n", (unsigned long)pthread_self()%100);
    sem_wait(&semaph);
    printf("Ciao dal thread %d\n", *(int*)args);
    sleep(1);
    sem_post(&semaph);
    free(args);
}


int main(int argc, char **argv)
{
    pthread_t th[THREAD_NUM];
    sem_init(&semaph, 0, 1); // il secondo parametro è il numero dei processi
                             // il terzo il valore iniziale del semaforo
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
