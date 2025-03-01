#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>


/*
    Come creo più thread senza fare copy paste di thread_create?
*/

int mails = 0;
pthread_mutex_t mutex;

void *routine()
{
    for (int i = 0; i < 1000000; ++i)
    {
        // i mutex nel for loop pesano sulla CPU
        // buona norma è metterli prima del ciclo
        pthread_mutex_lock(&mutex);
        mails++;
        pthread_mutex_unlock(&mutex);
    }
    return NULL;
}


int main(int argc, char **argv)
{
    // pthread_t t1;
    // pthread_t t2;
    // pthread_t t3;
    // pthread_t t4;
    
    pthread_t t[4]; // array di thread
    // inizializza mutex
    pthread_mutex_init(&mutex, NULL);
    for (int i = 0; i < 4; ++i)
    {
        if (pthread_create(&t[i], NULL, &routine, NULL) != 0)
        {
            perror("Thread non creato");
            return 1;
        }
        printf("Thread %d è partito\n", i);
        // if (pthread_join(t[i], NULL) != 0)
        // {
        //     return 2;
        // }
        // printf("Thread %d è finito\n", i);
    }
    // notiamo che ogni thread viene creato solamente dopo che il precedente ha finito
    // creiamo il thread 0 e facciamo la join del thread 0. Dopodichè parte il thread 1 e così via
    // non è molto furbo se vogliamo parallelismo però :)
    // il modo corretto è creare un altro ciclo for ma solo con la funzione join
    for (int j = 0; j < 4; ++j)
    {
        if (pthread_join(t[j], NULL) != 0)
        {
            return 2;
        }
        printf("Thread %d è finito\n", j);

    }
    
    pthread_mutex_destroy(&mutex);
    printf("Number of mails: %d\n", mails);

    return 0;
}
