#define _GNU_SOURCE   // avverte che usiamo le estensioni GNU 
#include <stdio.h>    // permette di usare scanf printf etc ...
#include <stdlib.h>   // conversioni stringa exit() etc ...
#include <stdbool.h>  // gestisce tipo bool
#include <assert.h>   // permette di usare la funzione ass
#include <string.h>   // funzioni per stringhe
#include <errno.h>    // rischiesto per usare errno
#include <math.h>

// Scopo del programma:
// imparare a costruire, visualizzare e distruggere le liste in C


// prototipi delle funzioni che appaiono dopo il main()
void termina(const char *messaggio);


typedef struct _capit
{
    char *nome;
    double lat;
    double lon;
    struct _capit *left;
    struct _capit *right; 

} capitale;


void capitale_stampa(const capitale *a, FILE *f)
{
    fprintf(f, "%-16s (%lf, %lf)", a->nome, a->lat, a->lon);
}

capitale *capitale_crea(char *s, double lat, double lon)
{
    capitale *a = malloc(sizeof(*a));
    a->nome = strdup(s);
    a->lat = lat;
    a->lon = lon;
    a->left = NULL;
    a->right = NULL;

    return a;
}

void capitale_distruggi(capitale *a)
{
    free(a->nome);
    free(a);
}

// stampa tutti gli elementi dell'albero 
// che hanno come radice root
void abr_capitale_stampa(const capitale *root, FILE *f)
{
    if (root != NULL)
    {
        abr_capitale_stampa(root->left, f);
        capitale_stampa(root, f);
        abr_capitale_stampa(root->right, f);
    }
}

void abr_capitale_distruggi(capitale *root)
{
    if (root != NULL)
    {
        abr_capitale_distruggi(root->left);
        abr_capitale_distruggi(root->right);
        capitale_distruggi(root);
    }
}

// legge un oggetto capitale dal file f
// restituisce il puntatore all'oggetto letto
// oppure NULL se non riesce a leggere dal file
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
            termina("Invalid input file");
        }
    }

    capitale *c = capitale_crea(s, lat, lon);
    free(s);

    return c;
}


// inserisci il nuovo nodo "c" dentro l'albero 
// con root "radice", non inserisce se c
// è già presente, restituisce la root
// del nouvo albero contenente anche "c"
capitale *inserisci_abr(capitale *root, capitale *c)
{
    c->left = NULL;
    c->right = NULL;

    // caso base: albero vuoto
    if (root == NULL)
    {
        return c;
    }

    int ris = strcmp(c->nome, root->nome);
    
    if (ris == 0)
    {
        fprintf(stderr, "Duplicated node: ");
        capitale_stampa(c, stderr);
        capitale_distruggi(c);
    }
    else if (ris < 0) // c->nome < root->nome
    {
        root->left = inserisci_abr(root->left, c);
    }
    else 
    {
        root->right = inserisci_abr(root->right, c);
    }
   
    return root;
}

// crea un abr con gli oggetti capitale letti da 
// *f inserendoli usando l'ordinamento dei nomi 
capitale *crea_abr(FILE *f)
{
    capitale *root = NULL;
    while (true)
    {
        capitale *b = capitale_leggi(f);
        if (b == NULL)
        {
            break;
        }
        root = inserisci_abr(root, b);
    }
    return root;
}

capitale *ricerca_abr(capitale *root, char *nome)
{
    if (root == NULL)
    {
        return NULL;
    }

    int e = strcmp(root->nome, nome);

    if (e == 0)
    {
        return root;
    }
    else if (e < 0)
    {
        return ricerca_abr(root->left, nome);
    }
    else 
    {
        return ricerca_abr(root->right, nome);
    }
}

int abr_altezza(capitale *root)
{
    if (root == 0)
    {
        return -1;
    }

    int right_h = abr_altezza(root->right);
    int left_h = abr_altezza(root->left);

    return (right_h > left_h) ? right_h + 1 : left_h + 1;
}

// retituisce true se c ha latitudine tra 40 e 43
bool latrange(capitale *c)
{
    assert(c!=NULL);
    return (c->lat >= 40.0) && (c->lat <= 43.0);
}

void abr_stampa_cond(capitale *root, FILE *f, bool (*funz)(capitale *))
{
    if (root == NULL)
    {
        return;
    }

    abr_stampa_cond(root->left, f, funz);
    
    if (funz(root))
    {
        capitale_stampa(root, f);
    }

    abr_stampa_cond(root->right, f, funz);

}

int abr_ricerca_range(capitale *root, char *smin, char *smax)
{
    if (root == NULL)
    {
        return 0;
    }

    int c1 = strcmp(root->nome, smin); // confronta nome e smin
    int c2 = strcmp(root->nome, smax); // confronta nome e smax
    int counter = 1; // nodo corrente

    if (c1 >= 0 && c2 <= 0) // stampa nome nel range attuale
    {
        capitale_stampa(root, stdout);
        puts("\n");
    }

    if (c1 > 0) // smin < nome -> sottoalbero sx
    {
        counter += abr_ricerca_range(root->left, smin, smax);
    }

    else if (c2 < 0) // smax > nome -> sottoalbero dx
    {
        counter += abr_ricerca_range(root->right, smin, smax);
    }

    return counter;


}

int main(int argc, char **argv)
{   
    if (argc != 4) 
    {
        printf("Uso: %s nomefile\n",argv[0]);
        exit(1);
    } 

    FILE *f = fopen(argv[1],"r");
    if (f == NULL) 
    {
        termina("Errore apertura file");
    }

    // costruzione albero leggendo capitali dal file
    capitale *radice = crea_abr(f);
    // puts("--- inizio lista ---");
    // stampa lista capitali appena creata
    // abr_capitale_stampa(radice,stdout);  
    // puts("--- fine lista ---");

    puts("Chiama abr_ricerca_range\n");
    int n = abr_ricerca_range(radice, argv[2], argv[3]);
    printf("range nomi esplorati: %d\n", n);

    abr_capitale_distruggi(radice);

    if (fclose(f) == EOF)
    {
        termina("Errore chiusura");
    }

    return 0;
}


// stampa su stderr il  messaggio che gli passo
// se errno!=0 stampa anche il messaggio d'errore associato 
// a errno. dopo queste stampe termina il programma
void termina(const char *messaggio)
{
    if (errno == 0) 
    {
        fprintf(stderr,"%s\n",messaggio);
    }
    else 
    {
        perror(messaggio);
    }
    exit(1);
}
