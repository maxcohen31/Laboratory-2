/*
    Scrivere un programma concatena che costruisce la stringa ottenuta concatenando tra loro le stringhe passate sulla linea di comando. Ad esempio, scrivendo

    concatena sole azzurro 123
    l'output dovrebbe essere

    Stringa concatenata: soleazzurro123
    In dettaglio il vostro programma deve

    1. calcolare la lunghezza lun della stringa risultato, come somma delle lunghezze delle stringhe argv[1] ... argv[argc-1]
    2. allocare con malloc un blocco di lun+1 byte (il +1 serve per il byte 0 finale)
    3. copiare i singoli caratteri dalle stringhe argv[i] nella stringa appena allocata, seguiti dal terminatore 0
    4. stampare la stringa ottenuta e deallocarla

    Testare il programma con valgrind
 */

#define _GNU_SOURCE
#include <string.h>
#include <stdio.h>
#include <stdlib.h>



int main(int argc, char *argv[])
{

    int l = 0; // store length of strings in argv[]
    
    for (int i = 0; i < argc; i++)
    {
        l += strlen(argv[i]);
    }

    char *result = malloc(l*sizeof(char));

    result[0] = '\0'; // initializing with \0

    for (int i = 1; i < argc; i++) 
    {
        // append argv[i] to result
        result = strcat(result, argv[i]);
    }

    printf("Concatenated string -> %s", result);

    free(result);
        

    return 0; 
}
