/*
 * Scopo del programma è come usare una funzione che accetta un'altra funzione come suo parametro formale
*/

#include <stdio.h>
#include <stdlib.h>

/* prototipo di applica */
/* il terzo argomento ha come tipo un puntatore a funzione che prende come input due interi e restituisce un intero */
int applica(int x, int y, int (*f)(int, int));

int add(int x, int y) { return x + y; }
int mul(int x, int y) { return x * y; }
int neg(int x) { return ~x; }


int main(int argc, char **argv)
{
    int a, b;
    if (argc != 3) 
    {
        fprintf(stderr, "Uso: \n\t%s a b\n", argv[0]);
        exit(1);
    }

    a = atoi(argv[1]);
    b = atoi(argv[2]);
    printf("Somma: %d\nProdotto: %d", applica(a, b, &add), applica(a, b, &mul));
    return 0;
}

int applica(int x, int y, int (*f)(int, int)) { return f(x, y); }

