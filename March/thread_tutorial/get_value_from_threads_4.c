#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

void *rollDice()
{
    int value = (rand() % 6) + 1;
    // dopo il seg. fault
    int *res = malloc(sizeof(int));
    *res = value;
    // printf("%d\n", value);
    printf("Thread risultato: %p\n", res);
    return (void*) res;
}

int main(int argc, char **argv)
{
    int *result;
    pthread_t th;
    if (pthread_create(&th, NULL, &rollDice, NULL) != 0)
    {
        return 1;
    }
    // Il secondo argomento di pthread_join è un puntatore a puntatore void (**void)
    // e verrà settato al valore che prende dalla funzione rollDice
    if (pthread_join(th, (void**) &result) != 0)
    {
        return 2;
    }
    
    printf("Risultato: %d\n", *result); // segmentation fault
                                        // perché vogliomo ritornare una variabile locale alla
                                        // funzione rollDice ma questa verrà deallocata in quanto è sullo stack

    free(result);
    return 0;
}
