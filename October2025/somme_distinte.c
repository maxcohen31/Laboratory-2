/*
    Scrivere una funzione 
    int *somme_distinte(int a[], int n, int *size)
    che dato un array di interi a[] di lunghezza n>0 alloca e restituisce un nuovo array b[] contenente tutti gli interi distinti 
    che si possono ottenere sommando tra loro due elementi di a (compreso un elemento sommato a se stesso). 

    Ad esempio
	input: 1 2
	output: 2 3 4   [infatti: 2=1+1, 3=1+2, 4=2+2] 
	input: 1 0 0 1 0 0 1 
	output: 2 1 0   [l'ordine nell'array di output non è importante]
	input: -2 3 1 10 
	output: -4 1 -1 8 6 4 13 2 11 20
    
    Dato che non è possibile stabilire a priori quale sarà la lunghezza dell'array risultato, 
    tale lunghezza deve essere passata per riferimento utilizzando il puntatore *size. 
    Non vi preoccupate dell'efficienza della soluzione da voi proposta.

    Si scriva poi un programma che invoca somme_distinte passandogli l'array ottenuto convertendo in interi i valori forniti sulla linea di comando e 
    successivamente stampa su stderr l'array restituito da somme_distinte. 
    Infine il programma deve stampare su stdout la somma dei valori contenuti nell'array restituito da somme_distinte. 
    (Nota: per stampare su stdout si può usare fprintf(stdout,...) oppure semplicemente printf). Il programma deve deallocare tutta la memoria utilizzata (verificare con valgrind).
 
*/

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

/* ########## functions declarations ########### */
void printArray(int *a, int n); /* utility function - it prints out the elements stored into an array */
int *sommeDistinte(int *a, int n, int *size); /* creates an array with all the possible sums by using elemets of a */
bool checkDuplicate(int *a, int n, int x); /* check if target x appears more than once into a */;



int main(int argc, char **argv)
{
    if (argc <= 1)
    {
        fprintf(stderr, "Usage: <executable> <n0, n1, ...n_m-1>");
        exit(EXIT_FAILURE);
    }

    int initSize = 10;
    int *arrayA = malloc((argc - 1) * sizeof(int)); /* the size is the number of integers passed to argv */
    if (!arrayA)
    {
        printf("Failed malloc for arrayA!\n");
        exit(1);
    }

    for (int i = 1; i < argc; ++i)
    {
        arrayA[i-1] = atoi(argv[i]); /* populate the initial array A with elements of argv*/
    }

    printArray(arrayA, argc-1);
    int *b = sommeDistinte(arrayA, argc-1, &initSize);
    printArray(b, initSize);

    free(arrayA);
    free(b);

    return 0;
}

/* ########## functions definitions ########## */
void printArray(int *a, int n)
{
    for (int i = 0; i < n; ++i)
    {
        printf("%d ", a[i]);
    }
    printf("\n");
}

bool checkDuplicate(int *a, int n, int x)
{
    for (int i = 0; i < n; ++i)
    {
        if (a[i] == x)
        {
            return true;
        }
    }
    return false;
}

int* sommeDistinte(int *a, int n, int *size)
{
    int insertedElements = 0; /* keep track of the total elements inserted into result */
    int *result = malloc(*size * (sizeof(int)));
    if (!result)
    {
        fprintf(stderr, "Malloc failed for array result\n");
        exit(2);
    }

    /* main routine */
    for (int i = 0; i < n; ++i)
    {
        for (int j = 0; j < n; ++j)
        {
            int s = a[i] + a[j];
            if (checkDuplicate(result, insertedElements, s)) /* s already exists? */
            {
                continue;
            }
            if (insertedElements == *size) /* check if the size is larger enough */
            {
                *size *= 2; /* doubling the size */
                result = realloc(result, (*size) * sizeof(int)); /* allocation using the new size */
                if (!result)
                {
                    fprintf(stderr, "Malloc failed inside main routine...\n");
                    exit(3);
                }
            }
            result[insertedElements] = s;
            insertedElements++;
        }
    }

    *size = insertedElements; /* brings the size to the exact number of elements inserted */
    result = realloc(result, (*size) * sizeof(int));
    if (!result)
    {
        fprintf(stderr, "Malloc failed calling last realloc...\n");
        exit(4);
    }

    return result;
}
