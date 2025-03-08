#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>

/*
    Passing certain unique values to each thread
*/

// global array
int primes[10] = {2, 3, 5, 7, 11 ,13, 17, 19, 23, 29};

// PROBLEMA:
// creare 10 thread, ogni thread stampa a schermo un numero primo
// contenuto nell'array primes

void *routine(void *arg)
{
    int index = *(int*)arg; // casting a intero dell'indice
    printf("%d\n", primes[index]);
    // soluzione migliore
    // printf("%d\n", index)
    free(arg);
    pthread_exit(NULL);
}

int main(int argc, char **argv)
{
    pthread_t t[10];
    for (int i = 0; i < 10; ++i)
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

        // soluzione migliore (vedere anche routine)
        // pthread_create(&t[i], NULL, &routine, &primes[i])
    
        *a = i; // a diventa l'indice del ciclo
        if (pthread_create(&t[i], NULL, &routine, a) != 0)
        {
            perror("Creazione thread fallita");
        }
    }
    for (int i = 0; i < 10; ++i)
    {
        if (pthread_join(t[i], NULL) != 0)
        {
            perror("Join dei thread fallita");
        }
    }

    return 0;

}
