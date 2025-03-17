/*
    SEMAFORI BINARY 

    Sono semafori che possono assumere il valore 0 o 1
*/


#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <semaphore.h>
#include <unistd.h>

//#define THREAD_NUM 4

#define THREAD_NUM 1
// int fuel = 50;
// supponiamo che fuel sia però un puntatore
// voglio che un thread usi questa memoria e una volta che ha terminato
// voglio che il main liberi la memoria. Come possiamo fare?

int *fuel;
pthread_mutex_t mutexFuel;
sem_t semFuel;

void *routine(void *args)
{
    // pthread_mutex_lock(&mutexFuel);
    // sem_wait(&semFuel); // decrementa il semaforo 
                        // sem_wait non serve perché fuel è allocata 
                        // prima di eseguire routine()
    fuel += 50;
    printf("Cuurent fuel value is %d\n", *fuel);
    sem_post(&semFuel); // incrementa il semaforo
    // pthread_mutex_unlock(&mutexFuel);
    return NULL;
}

int main(int argc, char **argv)
{
    pthread_t th[THREAD_NUM];
    fuel = malloc(sizeof(int));
    *fuel = 50;
    pthread_mutex_init(&mutexFuel, NULL);    
    // sem_init(&semFuel, 0, 1); // esempio con fuel non puntatore
    sem_init(&semFuel, 0, 0); // esempio fuel puntatore
    for (int i = 0; i < THREAD_NUM; ++i)
    {
        if (pthread_create(&th[i], NULL, &routine, NULL) != 0)
        {
            perror("Thread creation failed\n");
        }
    }

    sem_wait(&semFuel); // il main deve attendere se il valore è 0
                        // e aspettare fino a che routine non abbia finito
    printf("Deallocatin memory\n");
                        
    for (int i = 0; i < THREAD_NUM; ++i)
    {
        if (pthread_join(th[i], NULL) != 0)
        {
            perror("Thread creation failed\n");
        }
    }

    pthread_mutex_destroy(&mutexFuel);
    sem_destroy(&semFuel);
    return 0;

}
