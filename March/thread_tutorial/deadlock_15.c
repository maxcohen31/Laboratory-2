#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

/*
    DEADLOCK
    
    Un esempio di deadlock è quando si fa la lock su un mutex due volte.
    Un insieme di processi si trova in deadlock (stallo) se ogni processo dell'insieme è 
    in attesa di un evento che solo un altro processo dell'insieme può provocare.
    Tipicamente, l'evento atteso è proprio il rilascio di risorse non prerilasciabili.

*/

#define THREAD_NUM 8

pthread_mutex_t mutexFuel;
int fuel = 50;
// e con due mutex cosa succede?
// il problema è che quando si hanno lock multipli lockati in ordine diverso (vedere l'if in routine)
pthread_mutex_t mutexWater;
int water = 10;

void *routine(void *arg)
{
    // in un programma con migliaia di linee è possibile trovare qualcosa del genere
    // se il thread 1 acquisisce il lock del fuel e il thread 2 il lock water
    // avremo che il thread 1 aspetterò per il lock water e il thread 2 per il lock fuel
    if ((rand() % 2) == 0)
    {
        pthread_mutex_lock(&mutexFuel);
        sleep(1); // la sleep qui può portare ad un deadlock
        pthread_mutex_lock(&mutexWater);
    }
    else 
    {
        pthread_mutex_lock(&mutexWater);
        sleep(1); // la sleep qui può portare ad un deadlock
        pthread_mutex_lock(&mutexFuel);
    }

    // pthread_mutex_lock(&mutexFuel); // causa deadlock e il programma non termina
    fuel += 50; 
    water = fuel;
    // printf("Incremented fuel to %d\n", fuel);
    printf("Incremented fuel to %d and set water to: %d\n", fuel, water);
    pthread_mutex_unlock(&mutexFuel);
    pthread_mutex_unlock(&mutexWater);

}

int main(int argc, char **argv)
{
    pthread_t th[THREAD_NUM];
    int i;

    pthread_mutex_init(&mutexFuel, NULL);
    pthread_mutex_init(&mutexWater, NULL);
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

    printf("Fuel: %d\n", fuel);
    printf("Water: %d\n", water);
    pthread_mutex_destroy(&mutexFuel);
    pthread_mutex_destroy(&mutexWater);

    return 0;
}
