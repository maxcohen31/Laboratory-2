/*
    Somme distinte
    Scrivere una funzione C

    int *somme_distinte(int a[], int n, int *size)
    che dato un array di interi a[] di lunghezza n>0 alloca e restituisce un nuovo array b[] 
    contenente tutti gli interi distinti che si possono ottenere sommando tra loro due elementi di a 
    (compreso un elemento sommato a se stesso). Ad esempio

        input: 1 2
        output: 2 3 4   [infatti: 2=1+1, 3=1+2, 4=2+2] 
        input: 1 0 0 1 0 0 1 
        output: 2 1 0   [l'ordine nell'array di output non è importante]
        input: -2 3 1 10 
        output: -4 1 -1 8 6 4 13 2 11 20
    
    Dato che non è possibile stabilire a priori quale sarà la lunghezza dell'array risultato, 
    tale lunghezza deve essere passata per riferimento utilizzando il puntatore *size. 
    Non vi preoccupate dell'efficienza della soluzione da voi proposta.

    Si scriva poi un programma che invoca somme_distinte passandogli l'array ottenuto convertendo in interi 
    i valori forniti sulla linea di comando e successivamente stampa su stderr l'array restituito da somme_distinte. 
    Infine il programma deve stampare su stdout la somma dei valori contenuti nell'array restituito da somme_distinte. 
    (Nota: per stampare su stdout si può usare fprintf(stdout,...) 
    oppure semplicemente printf). Il programma deve deallocare tutta la memoria utilizzata (verificare con valgrind).
 */


#define _GNU_SOURCE
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>


bool in_array(int *a, int n, int target)
{ 
    
    for (int i = 0; i < n; i++) 
    {
        if (a[i] == target) 
        {
            return true;
        }
    }
    return false;
}

int *somme_distinte(int a[], int n, int *size) 
{
    
    *size = 0;

    int *b = malloc((n*n) * sizeof(int));
    if (b == NULL) 
    {
        exit(EXIT_FAILURE);
    }

    int capacity = 0;

    for (int i = 0; i < n; i++) 
    {
        for (int j = i; j < n; j++) 
        {
            int s = a[i] + a[j];
            // add sum only if doesn't exist 
            if (!in_array(b, capacity, s)) 
            {
                b[capacity] = s;
                capacity++;
            }
        }
    }

    // update size
    *size = capacity;

    b = realloc(b, (*size)*sizeof(int));

    return b;
}

int main(int argc, char* argv[]) 
{
    if (argc < 2)
    {
        printf("\nArguments must be two or more!Exit...\n");
    }

    int *a = malloc(argc*(sizeof(int)));
    if (a == NULL)
    {
        exit(EXIT_FAILURE);
    }

    for (int i = 1; i < argc; i++)
    {
        int n = atoi(argv[i]);
        a[i-1] = n;
    }

    int *result= somme_distinte(a, argc-1, &argc);
    for (int i = 0; i < argc; i++)
    {
        printf("%4d", result[i]);
    }

    fprintf(stderr, "\nDone!");
    free(a);

    return 0;
}
