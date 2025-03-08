#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>

/*
    Sum of array values using multithreading
    Split the array in half and sum
*/

int primes[10] = {2, 3, 5, 7, 11 ,13, 17, 19, 23, 29};

void *routine(void *arg)
{
    int index = *(int*)arg; // casting a intero dell'indice
    int sum = 0;
    for (int j = 0; j < 5; ++j)
    {
        sum += primes[index + j];
    }
    // come ritorniamo il risultato? pthread_join
    *(int*)arg = sum;
    return arg;
}

int main(int argc, char **argv)
{
    pthread_t t[2];
    for (int i = 0; i < 2; ++i)
    {
        // la soluzione è creare dinamicamente una variabile
        // e passarla poi a pthread_create al posto dell'indice del ciclo
        // tuttavia c'è una soluzione migliore
        int *a = malloc(sizeof(int));
        if (a == NULL)
        {
            perror("Allocazione memoria di a fallita");
            exit(1);
        }
 
        *a = i * 5; // a diventa l'indice del ciclo
        if (pthread_create(&t[i], NULL, &routine, a) != 0)
        {
            perror("Creazione thread fallita");
        }
    }

    int globalSum = 0;
    for (int i = 0; i < 2; ++i)
    {
        int *res;
        if (pthread_join(t[i], (void**)&res) != 0)
        {
            perror("Join dei thread fallita");
        }
        globalSum += *res;
        free(res);
    }

    printf("Somma %d\n", globalSum);
    return 0;

}
