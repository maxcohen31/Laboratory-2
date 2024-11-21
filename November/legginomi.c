#define _GNU_SOURCE    
#include <stdlib.h>
#include <stdio.h>     
#include <stdlib.h>   
#include <stdbool.h>  
#include <assert.h>   
#include <string.h>   
#include <errno.h>    

// Scopo del programma:
// mostrare il funzionamento di getline() e strtok() 
// Eseguire passando come argomento persone.txt


// prototipo funzione dopo il main
void termina(const char *messaggio);

char *elemina_spazi_testa(char *s)
{
    int i = 0;
    while (s[i] == ' ')
    {
        i++;
    }

    assert(s[i] != ' ');

    return &s[i];
}


int main(int argc, char **argv)
{
    if (argc < 2)
    {
        printf("Uso: %s nomefile", argv[0]);
        exit(1);
    }

    FILE *f = fopen(argv[1], "r");
    if (f == NULL)
    {
        termina("Error opening file");
    }

    // ciclo lettura dal file
    char *buffer = NULL;
    size_t n = 0;

    while (true)
    {
        ssize_t e = getline(&buffer, &n, f);
        // file terminato
        if (e < 0)
        {
            free(buffer);
            break;
        }
        // tokenizzazione
        char *s = strtok(buffer, ";\n");
        while (s != NULL)
        {
            s = elemina_spazi_testa(s);
            if (strlen(s) > 0)
            {
                printf("Letto: <%s>", s);
            }
            // legge prossimo token
            s = strtok(NULL, ";\n");
        }
    }

    fclose(f);

    return 0;
}


void termina(const char *messaggio)
{
    if(errno==0) 
    {
        fprintf(stderr,"%s\n",messaggio);
    }
    else 
    {
        perror(messaggio);
    }
    exit(1);
}
