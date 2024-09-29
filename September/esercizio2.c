/*
    Scrivere un programma reverse che stampa sullo schermo gli argomenti passati sulla linea di comando 
    (escluso il nome del programma) con i caratteri in ordine inverso. Ad esempio, scrivendo

    reverse sole azzurro 123
    l'output dovrebbe essere

    elos
    orruzza
    321
*/

#define _GNU_SOURCE
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

// swap two character
void swap(char *a, char *b)
{
    char temp = *a;
    *a = *b;
    *b = temp;
}


// reverse a string and return it
char* reverse(char *word)
{
    int i = 0;
    int j = strlen(word) - 1;
    char *copy = strdup(word);

    // two pointers cycle over the current word and swap its characters
    while (i < j) 
    {
        swap(&copy[i], &copy[j]);
        i++;
        j--;
    }

    return copy;
    
    // strdup uses malloc to allocate memory. We must deallocate it.
    free(copy);
}


int main(int argc, char *argv[])
{
    
    for (int i = 1; i < argc; i++)
    {
        printf("%s\n", reverse(argv[i]));
    }

    return 0;
}


