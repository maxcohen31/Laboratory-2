#define _GNU_SOURCE   // avverte che usiamo le estensioni GNU 
#include <stdio.h>    // permette di usare scanf printf etc ...
#include <stdlib.h>   // conversioni stringa exit() etc ...
#include <stdbool.h>  // gestisce tipo bool
#include <assert.h>   // permette di usare la funzione assert
#include <string.h>   // funzioni per stringhe
#include <errno.h>    // richiesto per usare errno

// Scopo del programma:
//   Mostrare come si definiscono e usano i puntatori a struct
//   in particolare gli array di puntatori a struct



void termina(const char *messaggio);

typedef struct
{
    char *nome;
    double lat;
    double lon;
} capitale;


capitale *crea_capitale(char *s, double lat, double lon)
{
    assert(s != NULL);
    assert(lat >= -90 && lat <= 90);
    assert(lon >= -180 && lon <= 180);

    // Pratica più flessibile per allocare memoria a struct
    capitale *a = malloc(sizeof(*a));
    if (a == NULL)
    {
        termina("Malloc fallita");
    }

    a->nome = strdup(s);
    a->lat = lat;
    a->lon = lon;

    return a;
}

void capitale_distruggi(capitale *a)
{
    free(a->nome);
    free(a);
}



#if 0
// Nota: questa parte di codice fino a #endif non viene compilata

// Avendo definito il tipo capitale, ecco due 
// possibili definizione di array di 100 capitali:

// statico (dimensione immutabile)
capitale a[100];  // ogni a[i] = una capitale  = 24 byte

// dinamico
capitale *a = malloc(100*sizeof(*a));
// dopo aver creato a[] in questo modo, posso modificare gli elementi: 
a[0].lat = 34.2;

// In questo esercizio invece di un array di oggetti di tipo capitale
// lavoreremo invece con un array di puntatori a capitale:

// versione statica
capitale *b[100]; // ogni b[i] = un puntatore = 8 byte

// versione dinamica
capitale **b = malloc(100*sizeof(*b));
// ogni b[i] però è solo un puntatore, non esiste lo spazio
// per i tre campi nome, lat, lon è necessario allocarlo:
b[0] = malloc(sizeof(capitale));

// Dato che b[0] è un puntatore, per settare la latitudine devo scrivere: 
(*b[0]).lat = 43.2;  // corretta, ma non si usa
// oppure:
b[0]->lat = 43.2;    // useremo questa;
#endif


void capitale_stampa(FILE *f, const capitale *a)
{
    fprintf(f, "20%s (%f, %f)\n", a->nome, a->lat, a->lon);
}

// legge un oggetto capitale dal file f
// restituisce il puntatore all'oggetto letto
// oppure NULL se è stato letto tutto il file
capitale *capitale_leggi(FILE *f)
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
            return NULL;
        }
        else
        {
            termina("Formato file non valido");
        }
    }

    capitale *c = crea_capitale(s, lat, lon);

    free(s);

    return  c;
}


// legge e restituisce un array di capitali *
capitale **capitale_leggi_file(FILE *f, int *num)
{
    assert(f != NULL);

    int size = 10; // dimensione attuale dell'array
    int messi = 0; // numero di elementi attualmente nell'array
    capitale **a = malloc(size*sizeof(capitale*));
    
    if(a==NULL)
    {
        termina("Memoria insufficiente");
    }

    capitale *b;
    while ((b = capitale_leggi(f)) != NULL)
    {
        if (messi == size)
        {
            size = size * 2; // raddoppia la dimensione
            a = realloc(a, size*sizeof(capitale*));
            if (a == NULL)
            {
                termina("Malloc fallita");
            }
        }

        assert(messi < size);

        a[messi] = b;
        messi++;

    }
    // ho messo tutti gli elementi che mi servivano
    size = messi;
    a = realloc(a, size*sizeof(capitale*));
    if (a == NULL)
    {
        termina("Malloc fallita");
    }

    // scrivo il numero di elementi e restituisco l'array
    *num = messi;
    return a;
}


int main(int argc, char *argv[])
{
    if(argc!=2) 
    {
        printf("Uso: %s nomefile\n",argv[0]);
        exit(1);
    }

    // legge i dati sulle capitali dal file 
    FILE *f = fopen(argv[1],"r");
    int n;
    capitale **a = capitale_leggi_file(f, &n);

    for (int i = 0; i < n; i++)
    {
        capitale_stampa(stdout, a[i]);
    }

    for (int i = 0; i < n; i++) 
    {
        capitale_distruggi(a[i]);
    }
    
    free(a);
    fclose(f);
    return 0;
}

void termina(const char *messaggio)
{
  perror(messaggio);
  exit(1);
}
