#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>

/*
    Esempio pratico dell'uso di trylock

    chefs = threads
    stove = dati condivisi (+mutex)

    Cosa succede se abbiamo più di una pentola?
    Possiamo fare in modo che ogni chef(thread) prende una pentola se disponibile
    e se non lo fosse si mette in attesa? Si, con trylock

    Se abbiamo mutex multipli così che i thread possono lavorare in parallelo
    allora un trylock ha senso. Sensa trylock aspettremmo un probabile definito mutex
*/

//pthread_mutex_t stoveMutex;
// int stoveFuel = 100;
pthread_mutex_t stoveMutexes[4];
int stoveFuel[4] = {100, 100, 100, 100};


void *routine(void *args)
{
    // pthread_mutex_lock(&stoveMutex);
    for (int i = 0; i < 4; ++i)
    {
        if (pthread_mutex_trylock(&stoveMutexes[i]) == 0) // lock acquisito
        {
            int fuelNeeded = (rand() % 30);
            if (stoveFuel[i] - fuelNeeded < 0)
            {
                printf("Non c'è abbastanza sugo. Vado a casa\n");
            }
            else
            {
                stoveFuel[i] -= fuelNeeded;
                sleep(1);  
                printf("sugo rimasto %d\n", stoveFuel[i]);
            }
            pthread_mutex_unlock(&stoveMutexes[i]); 
            break; // se lock su un dato mutex ovvero uno chef è stato capace di cucinare
                   // almeno una volta, allora dovrebbe terminare
        }
        else
        {
            if (i == 3) // ultima iterazione
            {
                printf("Nessuna pentola disponibile ancora, aspetto...\n");
                sleep(1);
                i = 0; // riparte il ciclo, in questo modo tutti i thread cucinano
            }
        }
    }

    pthread_exit(NULL);
}

int main(int argc, char **argv)
{
    srand(time(NULL));
    pthread_t th[10];

    for (int i = 0; i < 4; ++i)
    {
        pthread_mutex_init(&stoveMutexes[i], NULL);
    }
    for (int i = 0; i < 10; ++i)
    {
        if (pthread_create(&th[i], NULL, &routine, NULL) != 0)
        {
            perror("Creazione thread fallita");
        }
    }
    for (int i = 0; i < 10; ++i)
    {
        if (pthread_join(th[i], NULL) != 0)
        {
            perror("Join dei thread fallito");
        }
    }
    
    for (int i = 0; i < 4; ++i)
    {
        pthread_mutex_destroy(&stoveMutexes[i]);
    }
    return 0;
}
