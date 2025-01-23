/*
    Si vuole gestire una classe di studenti tramite un vettore di dimensione variabile di record, dove i record hanno la seguente struttura:

    struct elem
    {
        char *nome;
        int eta;
    };

    Scrivere una funzione C che prende come parametro il numero di studenti da inserire e che
    restituisce l’indirizzo del vettore leggendo da tastiera i nomi (non più lunghi di 20 caratteri) e
    l’età degli studenti. Gestire eventuali errori nella fase di allocazione di memoria, restituendo
    NULL in caso di errori, l’indirizzo del vettore in caso di successo.
*/

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#define LUNGHEZZA_MASSIMA 21

typedef struct elem 
{
    char *nome;
    int eta;
} studente;


studente *leggi_studenti(int n)
{
    studente *classe = malloc(n * sizeof(*classe));
    if (classe == NULL)
    {
        printf("Errore allocazione array");
        exit(EXIT_FAILURE);
    }
    char nome[LUNGHEZZA_MASSIMA];
    int eta;

    for (int i = 0; i < n; i++)
    {
        printf("Inserisci nome (max 21 caratteri)-> ");
        int s = scanf("%s", nome);
        if (s != 1)
        {
            printf("Errore lettura nome");
        }
        if (strlen(nome) > LUNGHEZZA_MASSIMA)
        {
            classe[i].nome = NULL;
        }

        classe[i].nome = malloc(LUNGHEZZA_MASSIMA*sizeof(char*));
        if (classe[i].nome == NULL)
        {
            printf("Errore allocazione nome");
            exit(EXIT_FAILURE);
        }
        classe[i].nome = strcpy(classe[i].nome, nome);

        printf("Inserisci età -> ");
        if (scanf("%d", &eta) != 1)
        {
            printf("Errore lettura età");
        }
        classe[i].eta = eta;
    }

    return classe;
    free(classe);
}


int main(int argc, char **argv)
{
    int n;
    printf("Inserisci il numero degli studenti -> ");
    if (scanf("%d", &n) != 1)
    {
        printf("Errore lettura scanf");
    }

    studente *stud = leggi_studenti(n);
    for (int i = 0; i < n; i++)
    {
        printf("Nome: %20s - Eta: %3d\n", stud->nome, stud->eta);
    }

    return 0;
}


