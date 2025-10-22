/*
 * Facendo riferimento alla struct coppia vista nell'esempio array_coppie.c, 
 * scrivere un programma che richiede sulla linea di comando un numero pari 2n di interi, 
 * crea un array di n puntatori a coppie, le ordina per differenza di elementi crescente e stampa e dealloca l'array ordinato. 
 * Ad esempio, scrivendo
                    
                coppie 2 2 3 4 7 0 2 1
                    
  * deve creare l'array di coppie (2 2) (3 4) (7 0) (2 1), 
  *  che ordinato per differenza crescente risulta 
  *  (3 4) (2 2) (2 1) (7 0), 
  *  essendo le differenze rispettivamente -1, 0, 1, 7.
*/

#include <stdio.h>
#include <stdlib.h>

/* defining the type couple */
typedef struct {
    int x; 
    int y;
} couple ;


/* qsort utility function */
int differenceCompare(const void *c1, const void *c2)
{
    couple *firstCouple = (couple *) c1;
    couple *secondCouple = (couple *) c2;
    
    int d1 = firstCouple->x - firstCouple->y;
    int d2 = secondCouple->x - secondCouple->y;

    return d1 - d2;
}

/* prints out a couple (x, y) */
void printArray(couple *a, int size)
{
    for (int i = 0; i < size; ++i)
    {
        printf("(%d, %d) ", a[i].x, a[i].y);
    }
    printf("\n");

}

int main(int argc, char **argv)
{
    if (argc % 2 == 0 || argc < 3)
    {
        fprintf(stderr, "Uso:\n\t %s un numero pari e positivo di interi\n", argv[0]);
        exit(1);
    }

    /* trovo la dimensione dell'array - numero di coppie che andranno inserite */
    int size = (argc - 1) / 2;
    /* creo l'array di coppie */
    couple *arrayCouple = malloc(size * sizeof(couple));
    if (!arrayCouple)
    {
        fprintf(stderr, "Errore: allocazione fallita per arrayCouple\n");
        exit(2);
    }

    for (int i = 0; i < size; ++i)
    {
        /* building the array of couples */
        arrayCouple[i].x = atoi(argv[2*i+1]);
        arrayCouple[i].y = atoi(argv[2*i+2]);
    }

    printArray(arrayCouple, size);
    /* qsort overview 
     * first parameter: pointer to the first element of the array to be sorted 
     * second parameter: number of elements inside the array 
     * third parameter: the size of one element
     * fourth parameter: a function pointer to a comparison function 
     */
    qsort(arrayCouple, size, sizeof(couple), differenceCompare);
    printArray(arrayCouple, size);
    free(arrayCouple); /* release memory */
    return 0;
}






