#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>


// da compilare con:
//  gcc -std=c11 -Wall -O -g -o parametri  parametri.c


// Nota: per eseguire le stampe di interi e puntatori
// questi vengono tutti convertiti in long e 
// stampati usando il formato %ld in printf
// (%ld si usa infatti per stamnpare i long)  


// funzione che incrementa di 1 un parametro
// passato per valore
int incrementa1(int x)
{
    printf("incrementa1: il valore di x è %ld e si trova in posizione %ld", (long) x, (long) &x);
    x += 1;
    return 0;
}

// y è passato per riferimento attraverso un puntatore
int incrementa2(int *y)
{
    printf("\nincrementa2: il valore di y è %ld e vale %ld", (long)y, (long) &y);
    *y += 1;
    return 0;

}

int main(int argc, char *argv[])
{
  int n =5;
  
  printf("main 1: n si trova nella posizione %ld e vale %ld\n",
         (long) &n, (long )n);  
  incrementa1(n);    // chiamata per valore: non ha effetto su n
                     //
  printf("main 2: n si trova nella posizione %ld e vale %ld\n",
         (long) &n, (long )n);  

  incrementa2(&n);  // chiamata per riferimento: modifica n
  printf("main 3: n si trova nella posizione %ld e vale %ld\n",
         (long) &n, (long )n);  
  return 0;
}
