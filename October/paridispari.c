/*
    Scrivere un programma paridispari che legge gli interi dal un file di testo argv[1] e 
    copia quelli pari in un in file di nome argv[2] e quelli dispari in un file di nome argv[3]. 
    In altre parole se il programma viene invocato scrivendo

    paridispari interi.txt pari dispari
    e il file interi.txt contiene

    10
    70
    17
    36
    -23
    al termine dell'esecuzione il file pari deve contenere

    10
    70
    36
    e il file dispari

    17
    -23
    Si noti che non è necessario salvare gli interi in un array; il programma può aprire i tre file contemporaneamente, 
    leggere dal primo con fscanf e scrivere sul secondo o terzo file con fprintf. 
    Quando non ci sono altri interi da leggere (cioè fscanf restituisce EOF si devono chiudere tutti i file e terminare).

    Il codice l'ho scritto alle 22 dopo una giornata passata in università. Mi sono alzato alle 5 per prendere il treno.
    Il codice fa cacare anche a me ergo non rompete le palle. Peace.
*/


#define _GNU_SOURCE
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <assert.h>

void failure(char *error_message)
{
    perror(error_message);
    exit(EXIT_FAILURE);
}

int main(int argc, char *argv[])
{
    if (argc != 4)
    {
        printf("Wrong input! You must pass two values");
    }

    FILE *file_to_read = fopen(argv[1], "r");
    int numb; // read integer

    if (file_to_read == NULL) 
    {
        failure("Error! Exit...");
    }

    FILE *odd_file = fopen(argv[2], "w");
    if (odd_file == NULL)
    {
        failure("Error! Exit...");

    }

    FILE *even_file = fopen(argv[3], "w");
    if (odd_file == NULL)
    {
        failure("Error! Exit...");

    }

    while (true)
    {
        int e = fscanf(file_to_read, "%d", &numb);
        if (e == EOF) 
        {
            break;
        }
        if (e != 1) 
        {
            printf("Error!");
        }

        if (numb % 2 == 1) 
        {
            fprintf(odd_file, "%d\n", numb);
        }
        else 
        {
            fprintf(even_file, "%d\n", numb);
        }
    }

    fclose(file_to_read);
    fclose(odd_file);
    fclose(even_file);

    fprintf(stderr, "Job done!");

    return 0;
}
