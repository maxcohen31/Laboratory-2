/*
    Lo scopo di questo programma è leggere da un file dei dati e creare con gli stessi una doppia lista linkata.
    Questo è un esercizio inventato da me per fare pratica con le liste a doppio link.
*/

#define _GNU_SOURCE
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <stdbool.h>
#include <err.h>

struct _personaggio
{
    char *nome;
    char *weapon;
    int power;
    struct _personaggio *next;
    struct _personaggio *prev;
};

typedef struct _personaggio personaggio;


personaggio *crea_personaggio(char *nome, char *weapon, int power)
{

    personaggio *p = malloc(sizeof(personaggio));
    if (p == NULL)
    {
        printf("Error allocating memory");
        exit(1);
    }

    p->nome = strdup(nome) ;
    p->weapon = strdup(weapon);
    p->power = power;
    p->next = NULL;
    p->prev = NULL;

    return p;
}

void distruggi_personaggio(personaggio *p)
{
    free(p->nome);
    free(p->weapon);
    free(p);
}

void distruggi_lista(personaggio *lista)
{
    assert (lista != NULL);
    while (lista != NULL)
    {
        personaggio *tmp = lista->next; // salvo primo nodo
        distruggi_personaggio(lista);
        lista = tmp;
    }
}

void stampa_lista(personaggio *lista)
{
    assert (lista != NULL);
    printf("null");
    while (lista != NULL)
    {
        printf(" <-> [%s %s %d] ", lista->nome, lista->weapon, lista->power);
        lista = lista->next;
    }
    printf("<-> null");
}

personaggio* crea_personaggio_da_file(FILE *input_file)
{
    assert (input_file != NULL);

    personaggio *persona = NULL;
    char *buffer = NULL;
    size_t buffsize = 0;

    int e = getline(&buffer, &buffsize, input_file);
    if (e == -1)
    {
        free(buffer);
        return NULL;
    }

    // parsing dei vari campi
    char *name = strtok(buffer, ";\n");
    char *weap = strtok(NULL, ";\n");
    char *pow = strtok(NULL, ";\n");

    if (name && weap && pow) 
    {
        persona = crea_personaggio(name, weap, atoi(pow));
    }
    else 
    {
        printf("riga ignorata %s\n", buffer);
    }

    free(buffer);
    return persona; 
}

// inserisce ogni personaggio letto da file
// in coda alla lista
personaggio *crea_lista_coda(FILE *input_file)
{
    assert (input_file != NULL);

    personaggio *head = NULL;
    personaggio *tail = NULL;

    while (true)
    {
        personaggio *p = crea_personaggio_da_file(input_file);
        if (p == NULL)
        {
            break;
        }
        // lista vuota
        if (head == NULL)
        {
            head = p;
            tail = p;
        }
        else
        {
            tail->next = p; // aggiorna la coda corrente
            p->prev = tail; // il precedente della coda diventa la vecchia coda
            tail = p; // la coda diventa il nuovo nodo
        }
    }
    return head;
}

personaggio *crea_lista_testa(FILE *input_file)
{
    assert (input_file != NULL);

    personaggio *head = NULL;
    personaggio *tail = NULL;

    while (true)
    {
        personaggio *p = crea_personaggio_da_file(input_file);
        if (p == NULL)
        {
            break;
        }
        if (tail == NULL) // lista vuota
        {
            head = p;
            tail = p;
        }
        else 
        {
            head->prev = p; // aggiorna la test acorrente 
            p->next = head; // il next della nuova testa diventa la vecchia testa
            head = p; // testa diventa il nuovo nodo
        }
    }
    return head;
}


int main(int argc, char **argv)
{
    if (argc < 2)
    {
        printf("Uso: %s <filename>", argv[0]);
    }

    personaggio *lista_personaggi = NULL;
    FILE *f = fopen(argv[1], "r");
    if (f == NULL)
    {
        fprintf(stderr, "Error opening file");
        exit(1);
    }

    puts("----- Creo lista... -----\n");
    // lista_personaggi = crea_lista_coda(f);
    lista_personaggi = crea_lista_testa(f);

    fclose(f);
    stampa_lista(lista_personaggi);

    distruggi_lista(lista_personaggi);

    return 0;
}
