#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

/*
    RECURSIVE MUTEXES
    
    E' possibile fare la lock più volte sullo stesso mutex, evitando il deadlock,
    ma dobbiamo lavorare con un certo parametro pthread_mutexattr_t

    Questo mutex può essere lockato e unlockato dal solito thread a differenza di un semaforo

*/

#define THREAD_NUM 8

pthread_mutex_t mutexFuel;
int fuel = 50;

void *routine(void *args)
{
    // bisogna lockare e unlockare il mutex lo stesso numero di volte per evitare deadlock
    pthread_mutex_lock(&mutexFuel);
    pthread_mutex_lock(&mutexFuel);
    fuel += 50;
    printf("Carburante aumentato a %d", fuel);
    pthread_mutex_unlock(&mutexFuel);
    pthread_mutex_unlock(&mutexFuel);

}

int main(int argc, char **argv)
{
    pthread_t th[THREAD_NUM];
    pthread_mutexattr_t recursiveMutex;
    pthread_mutexattr_init(&recursiveMutex);
    pthread_mutexattr_settype(&recursiveMutex, PTHREAD_MUTEX_RECURSIVE);
    pthread_mutex_init(&mutexFuel, &recursiveMutex);
    int i;

    for (i = 0; i < THREAD_NUM; i++) 
    {
        if (pthread_create(&th[i], NULL, &routine, NULL) != 0) 
        {
            perror("Failed to create thread");
        }
    }

    for (i = 0; i < THREAD_NUM; i++) 
    {
        if (pthread_join(th[i], NULL) != 0) 
        {
            perror("Failed to join thread");
        }
    }

    pthread_mutexattr_destroy(&recursiveMutex);
    pthread_mutex_destroy(&mutexFuel);
    return 0;
}
