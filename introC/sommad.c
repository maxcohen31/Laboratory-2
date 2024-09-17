#define _GUN_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <assert.h>

/*
    Stesso programma di prima ma allocheremo dinamicamente l'array di int
*/


int main(int argc, char *argv[])
{
    int n; 

    printf("Inserisci il numero di elementi: ");
    int e = scanf("%d", &n);

    if (e != 1) 
    {
        puts("Valore non trovato");
        exit(1);
    } 
    if (n <= 0)
    {
        puts("Il valore deve essere positivo");
        exit(2);
    }

    // prima era solo a[n]
    // crea un array dinamico e riempi array
    int *a;
    a = malloc(n*(sizeof(int))); // int: 4 byte. Se n è 10 size di a sarà 40 byte
    if (a == NULL)
    {
        puts("Malloc fallita");
        exit(3);
    }

    for (int i = 0; i < n; i++) 
    {

        printf("Inserisci l'elemento di posto %d: ", i);
        e = scanf("%d", &a[i]);
        if (e != 1) 
        {
            puts("Valore non trovato");
            exit(1);
        }
    }

    int somma = 0;
    for (int i = 0; i < n; i++) 
    {
        somma += a[i];
    }

    printf("%d", somma);

    // libera la memoria
    free(a);

    return 0;

    
}
