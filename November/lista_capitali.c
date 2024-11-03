#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>
#include <string.h>


// Scopo del programma:
// imparare a costruire, visualizzare e distruggere le liste in C


typedef struct capit 
{
    char *nome;
    double lat;
    double lon;
    struct capit *next; // indirizzo del prossimo elemento
} capitale;


void capitale_stampa(const capitale *a, FILE *f)
{
    fprintf(f, "%20s (%f, %f)", a->nome, a->lat, a->lon);
}

capitale *crea_capitale(char* str, double lat, double lon)
{
    capitale *a = malloc(sizeof(*a));
    if (a == NULL)
    {
        perror("Error allocating memory");
    }

    a->nome = str;
    a->lat = lat;
    a->lon = lon;
    a->next = NULL;

    return a;
}

void capitale_distruggi(capitale *a)
{
    free(a->nome);
    free(a);
}


void stampa_capitali(const capitale *lis, FILE *f)
{
    // capitale *p = lis // se ne può fare a meno
    while (lis != NULL)
    {
        capitale_stampa(lis, f);
        lis = lis->next;
    }
}

void distruggi_capitali(capitale *lis)
{
    while (lis != NULL)
    {
        capitale *tmp = lis->next;
        capitale_distruggi(lis);
        lis = tmp;
    }
}


// legge un oggetto capitale dal file f
// restituisce il puntatore all'oggetto letto
// oppure NULL se non riesce a leggere dal file
capitale *leggi_capitale(FILE *f)
{
    assert(f != NULL);
    
    char *s;
    double lat;
    double lon;
    
    int e = fscanf(f, "%ms %lf %lf", &s, &lat, &lon);
    if (e != 3)
    {
        if (e == EOF)
        {
            return NULL; // finito il file non ci sono altri dati
        }
        else 
        {
            fprintf(stderr, "Error file");
        }
    }

    capitale *c = crea_capitale(s, lat, lon);
    free(s); 

    return c;
}

// crea una lista con gli oggetti capitale letti da 
// *f inserendoli ogni volta in testa alla lista
capitale *crea_lista_testa(FILE *f)
{
    // costruzione della lista
    capitale *head = NULL;
    while (true)
    {
        capitale *b = leggi_capitale(f);
        if (b == NULL) 
        {
            break;
        }
        b->next = head;
        head = b;

    }
    return head;
}

// crea una lista con gli oggetti capitale letti da 
// *f inserendoli ogni volta in coda alla lista
capitale *crea_lista_coda(FILE *f)
{
    capitale *head = NULL; // serve per il return
    capitale *tail = NULL; // serve per l'inserimento in coda
    
    while (true)
    {
        capitale *b = leggi_capitale(f);
        if (b == NULL)
        {
            break;
        }

        // lista vuota
        if (tail == NULL)
        {
            head = tail = b;
        }
        else
        {
            // lista non vuota
            tail->next = b;
            tail = b; 
        }
        
        tail->next = NULL;
    }
    return head;
}


// questo lo vedremo nella prossima lezione 
// cancella da una lista l'elemento con nome "s"
capitale *cancella_nome(capitale *testa, char *s)
{
    assert(s!=NULL);

    if (testa==NULL) return NULL; // lista vuota non c'è nulla da cancellare
                                 
    // verifico se il primo elemento va cancellato 
    if (strcmp(testa->nome,s) == 0)
    {
        capitale *tmp = testa->next;
        capitale_distruggi(testa);
        return tmp;
    }
    // ora so che il primo elemento non va cancellato
    // e che la lista non è vuota
    testa->next = cancella_nome(testa->next,s);

    return testa;
}



int main(int argc, char **argv)
{
    if (argc != 2)
    {
        printf("Uso: %s nomefile\n", argv[0]);
        exit(1);
    }

    FILE *f = fopen(argv[1], "r");
    if (f == NULL)
    {
        fprintf(stderr, "Error opening file");
    }

    // costruzione lista leggendo capitali dal file
    capitale *testa = crea_lista_testa(f);
    
    puts("--- inizio lista ---");

    // stampa lista capitali appena creata
    stampa_capitali(testa, stdout);  

    puts("--- fine lista ---");

    distruggi_capitali(testa);

    // costruzione lista inserendo in coda
    rewind(f); // riavvolge il file
    testa = crea_lista_coda(f);

    puts("--- inizio lista ---");

    // stampa lista capitali appena creata
    stampa_capitali(testa,stdout);  
    puts("--- fine lista ---");

    #if 0 // prossima lezione 
    // elimina Londra dalla lista
    testa = cancella_nome(testa,"Londra");
    puts("--- inizio lista ---");
    lista_capitale_stampa(testa,stdout);  
    puts("--- fine lista ---");
    #endif 

    if (fclose(f)==EOF)
    {
        fprintf(stderr,"Errore chiusura");
    }

    // dealloca la memoria usata dalla lista 
    distruggi_capitali(testa);

    return 0;
}


