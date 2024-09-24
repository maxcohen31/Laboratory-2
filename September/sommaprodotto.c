#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>


// Se vogliamo che una  funzione in C restituisca più
// di un valore, bisogna passare per i puntatori


int sommaprod(int x, int y, int *prod)
{
    *prod = x * y;
    return x + y;

}

int main(int argc,char * argv[])
{
    // calcolo somma e prodotto di 5 e 10
    int a = 5;
    int b = 10;
    int prodotto;
    int somma = sommaprod(a, b, &prodotto);
    printf("Somma: %d, prodotto: %d\n", somma, prodotto);
} 

