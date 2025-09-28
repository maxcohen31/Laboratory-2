/*
    Scrivere un programma C che legge un intero N e costruisce i seguenti due array di interi:

    l'array a[] contente i numeri tra 1 e N che sono multipli 3 ma non di 5: 3, 6, 9, 12, 18, 21, 24, 27, 33, ...
    
    l'array b[] contente i numeri tra 1 e N che sono multipli 5 ma non di 3: 5, 10, 20, 25, 35, 40, 50, ...
    
    Al termine della costruzione deve stampare lunghezza e la somma gli elementi degli array a e b, deve poi deallocarli e terminare.
    
    Ad esempio per N=100 i due array risultano
    
    a = [3, 6, 9, 12, 18, 21, 24, 27, 33, 36, 39, 42, 48, 51, 54, 57, 63, 66, 69, 72, 78, 81, 84, 87, 93, 96, 99]
    
    b = [5, 10, 20, 25, 35, 40, 50, 55, 65, 70, 80, 85, 95, 100]
    e di conseguenza l'output dovrebbe essere
    
    lunghezza a[] = 27,  somma a[] = 1368
    lunghezza b[] = 14,  somma b[] = 735
    Per N=99999 l'output dovrebbe essere
    
    lunghezza a[] = 26667,  somma a[] = 1333366668
    lunghezza b[] = 13333,  somma b[] = 666633335
    Eseguire il programma anche utilizzando valgrind verificando che non stampi nessun messaggio d'errore e al termine visualizzi il messaggio
    
    HEAP SUMMARY:
     in use at exit: 0 bytes in 0 blocks 
*/



#define GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>


int main(int argc, char **argv)
{
    int sizeA = 10;
    int sizeB = 10;
    int insertedA = 0; 
    int insertedB = 0;
    int input = 0;
    /* Starting with arrays of size 10 */
    int *arrayA = malloc(sizeA * sizeof(int));
    int *arrayB = malloc(sizeB * sizeof(int));
    if (!arrayA || !arrayB)
    {
        fprintf(stderr, "Error allocating memory\n");
    }

    /* keep prompting the user to insert a positive integer */
    do
    {
        printf("Insert a positive integer: ");
        if (scanf("%d", &input) != 1)
        {
            while (getchar() != '\n')
            {
                printf("Invalid input\n");
                input = -1;
            }
        }
    }while (input < 0);

    for (int i = 0; i <= input; ++i)
    {
        if ((i % 3 == 0) && (i % 5 != 0))         
        {
            if (sizeA == insertedA)
            {
                sizeA *= 2;
                arrayA = realloc(arrayA, 2 * sizeA * sizeof(int));
            }
            arrayA[insertedA] = i;
            insertedA++; 
        }
        if ((i % 5 == 0) && (i % 3 != 0))
        {
            if (sizeB == insertedB)
            {
                sizeB *= 2;
                arrayB = realloc(arrayB, 2 * sizeB * sizeof(int));
            }
            arrayB[insertedB] = i;
            insertedB++;
        }

    }

    /* the sizes will be the total of elements inserted in each array */
    sizeA = insertedA;
    sizeB = insertedB;

    /* creating new arrays */
    arrayA = realloc(arrayA, insertedA * sizeof(int));
    arrayB = realloc(arrayB, insertedB * sizeof(int));
    if (!arrayA || !arrayB)
    {
        fprintf(stderr, "Error allocating memory for new arrays\n");
        exit(2);
    }

    puts("Array A: ");
    for (int i = 0; i < insertedA; ++i)
    {
        printf("%d ", arrayA[i]);
    } 
    printf("\n");

    printf("Length of array A: %d\n", insertedA);
    
    puts("Array B: ");
    for (int i = 0; i < insertedB; ++i) 
    {
        printf("%d ", arrayB[i]);
    }
    printf("\n");

    printf("Length of array B: %d\n", insertedB);

    free(arrayA);
    free(arrayB);
    exit(0);


}
