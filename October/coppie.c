/*
    Facendo riferimento alla struct coppia vista nell'esempio array_coppie.c, scrivere un programma che richiede sulla linea di comando un numero pari 2n di interi, 
    crea un array di n puntatori a coppie, le ordina per differenza di elementi crescente e stampa e dealloca l'array ordinato. Ad esempio, scrivendo

    coppie 2 2 3 4 7 0 2 1

    deve creare l'array di coppie (2 2) (3 4) (7 0) (2 1), che ordinato per differenza crescente risulta (3 4) (2 2) (2 1) (7 0), essendo le differenze rispettivamente -1, 0, 1, 7.
 */

#define _GNU_SOURCE
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>


typedef struct
{
    int x;
    int y;
} coppia;


void print_coppia(const coppia *c);


int compare_difference(const void *c1, const void *c2);


int main(int argc, char **argv)
{
    if ((argc - 1) % 2 != 0)
    {
        fprintf(stderr, "Need an even number of arguments");
        exit(1);
    }

    // number of elements
    int n = (argc - 1) / 2; 
    // array to store pointers to structs
    coppia **arr = malloc(n*sizeof(coppia*));
    if (arr == NULL)
    {
        perror("Error allocating memory");
        exit(2);
    }

    for (int i = 0; i < n; i++)
    {
        arr[i] = malloc(sizeof(coppia));
        if (arr[i] == NULL)
        {
            perror("Error allocating memory for each elements");
            exit(3);
        }

        arr[i]->x = atoi(argv[2*i+1]); 
        arr[i]->y = atoi(argv[2*i+2]);
    }
    
    // sorting the array
    qsort(arr, n, sizeof(coppia*), compare_difference); 

    for (int i = 0; i < n; i++)
    {
        print_coppia(arr[i]);
    }

    for (int i = 0; i < n; i++)
    {
        free(arr[i]);
    }
    free(arr);

    return 0;

}


// passing coppia by reference
void print_coppia(const coppia *c)
{
    fprintf(stdout, "(%d, %d)\n", c->x, c->y);
}


int compare_difference(const void *c1, const void *c2)
{
    int first_diff = (*(coppia**)c1)->x - (*(coppia**)c1)->y;
    int second_diff = (*(coppia**)c2)->x - (*(coppia**)c2)->y;

    return first_diff - second_diff; 
}

