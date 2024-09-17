#define _GNU_SOURCE // Avverte che usiamo le estensioni GNU
#include <stdlib.h> // Conversioni stringa/numero exit() etc...
#include <stdio.h> // Permette di usare scanf printf etc...
#include <string.h> // Funzioni di confronto/copia/etc tra stringhe
#include <stdbool.h> // Gestisce tipo bool
#include <assert.h> // Permette di usare la funzione assert


// Le istruzioni qui sopra le spieghiamo più avanti

// Da compilare con gcc -std=c11 -Wall -O -g -o somma somma.c

// Scopo del programma
// legge un intero da tastiera 
// crea un array di N int
// calcola la somma degli elementi dell'array
// stampa la somma


int main(int argc, char *argv[])
{
    int n; 

    // Spiego cosa voglio leggere e lo leggo con scanf
    puts("Inserisci il numero di elementi: ");
    int e = scanf("%d", &n); // Il motivo della & lo spiegheremo più avanti

    // Controlli sulla lettura
    if (e != 1) 
    {
        puts("Valore non trovato");
        exit(1);
    }
    if (n <= 0) 
    {
        puts("Il numero di elementi deve essere positivo");
        exit(2);
    }

    int a[n]; // Crea array di n interi
    for (int i = 0; i < n; i++) 
    {
        printf("Inserisci l'elemento di posto %d: ", i);
        e = scanf("%d", &a[i]);
        if (e != 1) 
        {
            exit(1);
        }
    }

    // Calcola la somma
    int somma = 0;
    for (int i = 0; i < n; i++) 
    {
        somma += a[i];
    }

    printf("%d\n", somma);

    return 0;
}



