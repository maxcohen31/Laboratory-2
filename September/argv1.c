#define _GNU_SOURCE
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>



int main(int argc, char* argv[])
{
    // stampo gli elementi di argv come stringhe
    for (int i = 0; i < argc; i++)
    {
        printf("argv[%d]: %s\n", i, argv[i]);
    }
    puts("----FINE----\n");

    if (argc < 2)
    {
        puts("Per procedere servono almeno un argomento oltre il nome del programma");
        exit(1);
    }
    
    printf("Hello %s\n", argv[1]); // stampa Hello e primo parametro passato da tastiera
    printf("First letter of first param is: %c", argv[1][0]); 

    char *copia = argv[0];
    printf("\nCopia: %s\n", copia);
    printf("Originale: %s\n", argv[0]);

    // strdup copia i singoli caratteri della stringa
    copia = strdup(argv[1]); // copia primo parametro passato ad argv
    copia[0] = 'V';

    printf("\nCopia: %s\n", copia);
    printf("Originale: %s\n", argv[1]);
    
    for (int i = 0; i < strlen(argv[1]); i++)
    {
        printf("Carattere %d: %c\n", i, argv[1][i]);
    }

    // dato che strdup() ottiene lo spazio con malloc bisogna deallocare la memoria
    free(copia);

    return 0;
} 



