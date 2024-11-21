#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <assert.h>

struct _personaggio
{
    char *nome;
    char *weapon;
    int power;
    struct _personaggio *next;
struct _personaggio *prev;
};

typedef struct _personaggio personaggio;

personaggio *crea_personaggio(char *nome, char *weapon, int power);

void distruggi_personaggio(personaggio *lista);

void distruggi_lista(personaggio *lista);

void stampa_lista(personaggio *lista);

personaggio *crea_personaggio_da_file(FILE *input_file);

personaggio *crea_lista_coda(FILE *input_file);

personaggio *crea_lista_testa(FILE *input_file);
