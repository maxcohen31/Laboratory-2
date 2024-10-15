/*
    Uso della funzione asprintf
    asprintf restituisce i byte allocati. Restituisce -1 se l'allocazione fallisce.
*/

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>



int main(int argc, char *argv[])
{
    int numb;
    char *s = NULL;
    printf("Insert a number and a string: %d %s", numb, s);
    int e = scanf("%d %ms", &numb, &s); // we delegate scanf to allocate memory - risky!

    if (e != 2) 
    {
        printf("scanf error");
    }

    // if i = 1 creates the file names 'name.1.txt'
    for (int i = 0; i < numb; i++)
    {
        char *name = NULL;
        int asp = asprintf(&name, "%s.%d.txt", s, i);
        if (asp == -1)
        {
            printf("asprintf fallita");
            exit(1);
        }

        FILE *f = fopen(name, "w");
        if (f == NULL)
        {
            printf(stderr, "Error opening file!");
        }

        // write on file
        fprintf(f, "%d\n", i);

        fclose(f);
        free(name);
    }

    free(s);
    
    return 0;
}
