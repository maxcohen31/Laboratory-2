/*
    Scrivere un programma che invocato dalla linea di comando scrivendo
        
        main infile outfile 
    
    legge tutte le stringhe dal file di testo infile e le riscrive su outfile in ordine inverso.
    
    Possiamo dare per assunto che le stringhe non superino i 40 caratteri.

    Mattarella    Pertini 
    Napolitano    Cossiga
    Ciampi        Scalfaro
    Scalfaro      Ciampi
    Cossiga       Napolitano
    Pertini       Mattarella

   */

#define _GNU_SOURCE
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

int main(int argc, char* argv[])
{
    if (argc != 3)
    {
        fprintf(stderr, "Not enough arguments!");
        exit(EXIT_FAILURE);
    }
    
    int lines = 0; // we don't know the number of lines 
    char **arr = (char**)malloc(lines*sizeof(char*));
    char string_size[40]; // we do know the max length of a string though

    FILE *f = fopen(argv[1], "r");
    if (f == NULL)
    {
        fprintf(stderr,"Error opening file");
        exit(EXIT_FAILURE);
    }

    while (fgets(string_size, 40, f))
    {
        // memory to store string pointers plus '\0'
        arr = realloc(arr, (lines+1) * sizeof(char*)); 
        if (arr == NULL)
        {
            fclose(f);
            exit(EXIT_FAILURE);
        }

        // memory to store a string plus '\0'
        arr[lines] = malloc((strlen(string_size) + 1) * sizeof(char));
        if (arr[lines] == NULL)
        {
            fclose(f);
            exit(EXIT_FAILURE);
        }

        // copy the actual file string into arr
        strcpy(arr[lines], string_size);
        lines++;
    }

    fclose(f);

    FILE *result = fopen(argv[2], "w");
    if (result == NULL)
    {
        fprintf(stderr,"Error opening result file");
        exit(EXIT_FAILURE);
    }

    
    for (int i = 0; i < lines; i++)
    {
        printf("%s", arr[i]);
    }

    for (int i = lines - 1; i >= 0; i--)
    {
        fprintf(result, "%s", arr[i]);
    }

    fclose(result); 
    
    for (int l = 0; l < lines; l++)
    {
        free(arr[l]);
    }
    free(arr);
    
    return 0;
}
