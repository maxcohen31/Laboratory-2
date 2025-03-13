#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <pthread.h>
#include <time.h>

/*
    Invece di chiamare return nella funzione del thread si può usare pthread_exit
*/

void *rollDice()
{
    int val = (rand() % 6) + 1;
    int *result = malloc(sizeof(int));
    *result = val;
    // return (void*) result;
    printf("Value %d\n", val);
    pthread_exit((void*)result);
}


int main(int argc, char **argv)
{
    int *res;
    srand(time(NULL));
    pthread_t th;

    if (pthread_create(&th, NULL, &rollDice, NULL) != 0)
    {
        return 1;
    }
    // se mettiamo pthread_exit qui, cioè nel main thread
    // il codice che viene dopo non verrà eseguito, tuttavia il processo non viene fermato.
    // Cercherà di terminare tutti gli altri thread 
    // pthread_exit è utile quando si vuole inizializzare thread nel main e lasciarli correre
    // mentre il main semplicemente termina
    pthread_exit(0);
    
    if (pthread_join(th, (void*)&res) != 0)
    {
        return 2;
    }

    free(res); // memory leak -> con pthread_exit non viene eseguita la free
    return 0;
}
