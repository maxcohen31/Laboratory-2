/*
    Il triangolo di Tartaglia (Pascal) contiene i coefficienti binomali:

    1
    1 1
    1 2 1
    1 3 3 1
    1 4 6 4 1
    ...

    Per i=0,1,2,... la riga i-esima contiene i+1 valori

    t[i][0] ... t[i][i] che soddisfano alla relazione t[i][0] = t[i][i] = 1 e per k=1,...,i-1

    t[i][k] = t[i-1][k-1] + t[i-1][k]

    Scrivere una procedura int **tartaglia(int n) che dato un intero n restituisce le prime n righe del triangolo di tartaglia. 
    Si noti che il triangolo può essere visto come una matrice in cui la prima riga ha 1 elemento, la seconda due, etc.

    Scrivere poi un main che legge un intero positivo n dalla riga di comando invoca la procedura tartaglia passando n come argomento, 
    stampa il triangolo risultante e dealloca tutta la memoria utilizzata.

    Verificare con valgrind che il programma acceda solo a memoria precedentemente allocata 
    e che al termine del'esecuzione tutta la memoria venga rilasciata.
 */

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>


int **tartaglia(int n)
{
    int **res_matrix = malloc(n*sizeof(*res_matrix));
    if (res_matrix == NULL)
    {
        printf("Error allocating memory");
        exit(1);
    }

    for (int i = 0; i < n; i++)
    {
        res_matrix[i] = malloc((i+1)*sizeof(int));
        if (res_matrix[i] == NULL)
        {
            printf("Error alocating memory at index %d", i);
            exit(1);
        }
    }
    
    for (int i = 0; i < n; i++)
    {
        // first column 
        res_matrix[i][0] = 1;
        // second column
        res_matrix[i][i] = 1;
        for (int j = 0; j < i; j++)
        {
            res_matrix[i][j] = res_matrix[i-1][j-1] + res_matrix[i-1][j];
        }
    }
   
    return res_matrix;
}

void print_triangle(int **m, int n)
{

    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j <= i; j++)
        {
            printf("%d ", m[i][j]);;
        }
        printf("\n");
    }
}


int main(int argc, char **argv)
{

    int n;

    printf("Insert number: ");
    int e = scanf("%d", &n);
    if (e != 1)
    {
        puts("value not found");
    }

    int **t = tartaglia(n);
    print_triangle(t, n);

    for (int i = 0; i < n; i++)
    {
        free(t[i]);
    }
    free(t);


    return 0;
}
