/*
 
   Scrivere un programma triangolo che invocato scrivendo:

triangolo nomefile

    legge da nomefile le stringhe contenute in esso e le salva in un array. 
    Successivamente crea un file nomefile.triangolo contenente nella riga i-esima le parole dalla i-esima alla prima in ordine inverso. 
    Ad esempio, se il file di input si chiama moria e contiene le parole dite amici ed entrate il file di output deve chiamarsi moria.triangolo e contentere le 4 righe:

    dite
    amici dite
    ed amici dite
    entrate ed amici dite

*/


#define _GNU_SOURCE
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>



int main(int argc, char **argv)
{
    int c = 0;
    int size = 10; // initial capacity of our array
    char w[40]; // strings of 40 chars
    char **words = malloc(size*sizeof(char*));
    if (words == NULL)
    {
        perror("Error allocating memory");
        exit(1);
    }

    FILE *f = fopen(argv[1], "r");
    if (f == NULL)
    {
        fprintf(stderr, "Error opening file");
    }
    
    while (fscanf(f, "%39s", w) != EOF)
    {
        // check size
        if (size == c)
        {
            size *= 2;
            words = realloc(words, size*sizeof(char*));
            if (words == NULL)
            {
                perror("Error allocating memory");
                exit(1);
            }
        }
        // memory for each string pointer
        words[c] = malloc((strlen(w) + 1) * sizeof(char));
        if (words[c] == NULL)
        {
            perror("Error allocating memory for string pointers");
            exit(2);
        }
        strcpy(words[c], w);
        c++;
        
    }

    fclose(f);

    // for (int i = 0; i < c; i++)
    // {
    //     printf("%s ", words[i]);
    // }



    char *nome;
    int e = asprintf(&nome, "%s.triangolo", argv[1]);
    if (e == -1)
    {
        perror("aspritf failure");
        exit(EXIT_FAILURE);
    }

    FILE *output_file = fopen(nome, "w");
    
    for (int i = 0; i < c; i++)
    {
        for (int j = i; j >= 0; j--)
        {
            fprintf(output_file, "%s ", words[j]);
        }
        fprintf(output_file, "\n");
    }

    for (int i = 0; i < c; i++)
    {
        free(words[i]);
    }
    free(words);

    fclose(output_file);

    return 0;
}
