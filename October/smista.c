/*
    Scrivere un programma smista che legge dalla linea di comando una stringa nome e un intero positivo N
    e crea i file nome.d.primi contenenti tutti i primi minori di N che terminano con la cifra d. 
    Ad esempio il comando:

    smista elenco 35

    deve creare il file elenco.1.primi contenente i valori 11 e 31, 
    il file elenco.2.primi contenente il valore 2, il file elenco.3.primi contenente i valori 3, 13 e 23 e così via 
    (all'interno dei file i valori potete scriverli uno per riga).

    E' particolarmente importante verificare con valgrind il corretto uso e deallocazione della memoria: 
    se non chiudete un file che avete aperto, valgrind segnalerà un blocco di memoria che non è stato deallocato.

    Testato con valgrind --leak-check=full --show-leak=all ./smista elenco 35
*/

#define _GNU_SOURCE
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <math.h>

// return true if n is prime
// false otherwise
bool is_prime(int n)
{
    if (n <= 1) 
    {
        return false;
    } 

    int start = 2; // 2 is prime
    
    while (start <= sqrt(n))
    {
        if (n % start == 0) 
        {
            return false;
        }
        start++;
    }

    return true;
}

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        fprintf(stderr, "Not enough argument");
        exit(1);
    }

    int range = atoi(argv[2]);
    FILE *files[10] = {NULL}; // array of 10 NULL files

    for (int i = 0; i < 10; i++)
    {
        // We care only about prime files such as elenco.3.primi or elenco.5.primi
        if (is_prime(i) || i == 1)
        {
            char *name = NULL;
            int e = asprintf(&name, "elenco.%d.primi", i) ;
            if (e == -1)
            {
                perror("Error allocating bytes");
                exit(1);
            }

            files[i] = fopen(name, "w");
            if (files[i] == NULL)
            {
                fprintf(stderr, "Error opening file");
                exit(2);
            }
            free(name);
        }
        
    }

    for (int i = 2; i < range; i++)
    {
        // file name
        bool p = is_prime(i);

        // if i is prime we write it to a file
        if (p)
        {
            int last_digit = i%10;
            // if file exists write to it
            if (files[last_digit] != NULL)
            {
                fprintf(files[last_digit], "%d\n", i);
            }
        }
    } 

    for (int i = 0; i < 10; i++)
    {
        if (files[i] != NULL)
        {
            fclose(files[i]);
        }
    }

    return 0;
}
