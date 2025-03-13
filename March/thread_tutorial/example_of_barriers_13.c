#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <strings.h>
#include <time.h>
#include <string.h>

/* 
    Problema: 
    ogni thread lancia un dado e salva il risultato in un array.
    Il main thread calcola il vincitore e ogni thread stampa un messaggio dicendo se ha vinto o no
*/

#define NUM_THREAD 8 

int diceValues[8]; // salva i rusultati dei lanci
int status[8] = {0}; // salva i chi vince e chi perde

// non usiamo un mutex perché possiamo avere più linee eseguite allo stesso tempo
// abbiamo bisogno che siano eseguiti dopo che certe condizioni siano soddisfatte
// questa barriera ferma i thread per fare in modo che ogni thread abbia lanciato un dado
// devo aspettare anche il main thread
pthread_barrier_t barrierRollDice;
pthread_barrier_t barrierCalc;


void *rollDice(void *arg)
{
    int index = *(int*)arg;  
    diceValues[index] = (rand() % 6) + 1;

    pthread_barrier_wait(&barrierRollDice);
    pthread_barrier_wait(&barrierCalc);
    // non è garantito che questo if sia eseguito prima di calcolare il vincitore
    // creiamo un'altra barriera per gli stati
    if (status[index] == 1)
    {
        printf("(%d rolled %d) I won\n", index, diceValues[index]);
    }
    else
    {
        printf("(%d rolled %d) I lost\n", index, diceValues[index]);
    } 
    free(arg);
}


int main(int argc, char **argv)
{
    srand(time(NULL));
    pthread_t th[NUM_THREAD];
    pthread_barrier_init(&barrierRollDice, NULL, NUM_THREAD + 1);
    pthread_barrier_init(&barrierCalc, NULL, NUM_THREAD + 1);

    int i;
    for (i = 0; i < NUM_THREAD; ++i)
    {
        int *a = malloc(sizeof(int));
        *a = i;
        if (pthread_create(&th[i], NULL, &rollDice, a) != 0)
        {
            perror("Thread non creato");
        }
    }

    pthread_barrier_wait(&barrierRollDice);
    // cerco il massimo risulato nei lanci ovvero il vincitore
    int max = 0;
    for (i = 0; i < NUM_THREAD; ++i)
    {
        if (diceValues[i] > max)
        {
            max = diceValues[i];
        }
    }

    // setto lo stato del lancio a 1 se il valore è il più alto
    // decretando un vincitore altrimenti a 0
    for (i = 0; i < NUM_THREAD; ++i)
    {
        if (diceValues[i] == max)
        {
            status[i] = 1;
        }
        else
        {
            status[i] = 0;
        }
    }
 
    pthread_barrier_wait(&barrierCalc);
    for (i = 0; i < NUM_THREAD; ++i)
    {
        if (pthread_join(th[i], NULL) != 0)
        {
            perror("Join fallita");
        }
    }

    pthread_barrier_destroy(&barrierRollDice);
    pthread_barrier_destroy(&barrierCalc);
    return 0;
}
