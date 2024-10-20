/*
    Scrivere una funzione char *converte16(int n)
    che dato un intero positivo n alloca e restituisce una stringa contenente l'intero n scritto in base 16, 
    utilizzando i caratteri da a a f per le cifre da 10 a 15. 
    (Suggerimento: non è difficile convertire un intero in esadecimale, 
    ma questa funzione deve restituire una stringa che deve essere allocata. 
    Per semplificare le cose potete usare funzione asprintf vista a lezione tenendo conto che nella printf 
    come il modificatore %d visualizza un intero in decimale, 
    il modificatore %x lo visualizza in base 16, vedere man 3 printf per i dettagli).
*/

#define _GNU_SOURCE
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>


char *converte16(int n)
{

    char *str = NULL; 
    int x = asprintf(&str, "%x", n);
    if (x == -1)
    {
        fprintf(stderr, "Error allocating bytes");
    }

    return str;
}


int main(int argc, char *argv[])
{
    if (argc == 1)
    {
        fprintf(stderr, "Not enough argument");
        exit(1);
    }

    for (int i = 1; i < argc; i++)
    {
        // convert string to integer 
        int c = atoi(argv[i]);
        
        printf("%s\n", converte16(c));
    }
    
    return 0;
}
