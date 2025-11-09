#define _GNU_SOURCE
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <stdbool.h>

typedef struct capitale_ 
{
    char *nome;
    char *nazione;
    int kpop;
    struct capitale_ *left;
    struct capitale_ *right;
} capitale;

void termina(const char *mess);

void capitaleStampa(const capitale *a, FILE *f)
{
    fprintf(f, "%-16s Nazione: %-16s Popolazione: %d\n", a->nome, a->nazione, a->kpop);
}

capitale *creaCapitale(char *s, int pop, char *nazione)
{
    capitale *a = malloc(sizeof(capitale));
    a->kpop = pop;
    a->nome = strdup(s);
    a->nazione = strdup(nazione);
    a->left = NULL;
    a->right = NULL;

    return a;
}

void distruggiCapitale(capitale *c)
{
    assert(c != NULL);
    free(c->nome);
    free(c->nazione);
    free(c);
}

void stampaABR(capitale *c, FILE *f)
{
    if (c != NULL)
    {
        stampaABR(c->left, f);
        capitaleStampa(c, f); 
        stampaABR(c->right, f);
    }
}


void preorder(const capitale *c, FILE *f, int depth)
{
    /* stampa depth spazi per mostrare la profondità del nodo */ 
    if (c == NULL) { return; }
    for (int i = 0; i < depth; i++) { fprintf(f, ". "); }
    capitaleStampa(c, f);
    preorder(c->left, f, depth + 1);
    preorder(c->right, f, depth + 1);
}

/* distrugge l'albero binario di ricerca */
void distruggiABR(capitale *root)
{
    if (root != NULL)
    {
        distruggiABR(root->left);
        distruggiABR(root->right);
        distruggiCapitale(root);
    }
}



/* inserisce il nodo "c" dentro l'albero con radice root */
/* se c è già presente non lo inserisce. La funzione restituisce il puntatore a root */
capitale *inserisciABR(capitale *root, capitale *c)
{
    assert(c != NULL);
    c->left = NULL;
    c->right = NULL;
    if (root == NULL)
    {
        return c;
    }
    
    int ris = strcmp(c->nome, root->nome);
    if (ris == 0) 
    {
        fprintf(stderr, "nodo duplicato: ");
        capitaleStampa(c, stderr);
        distruggiCapitale(c);
    }
    else if (ris < 0)
    {
        root->left = inserisciABR(root->left, c);
    }
    else 
    {
        root->right = inserisciABR(root->right, c);
    }
    return root;
}


/* crea un abr con gli oggetti capitale letti dal file */
capitale *creaABR(FILE *f)
{
    capitale *root = NULL; 
    char *buffer = NULL; /* usato da getline */
    size_t n = 0; /* usata da getline */

    while (true)
    {
        ssize_t e = getline(&buffer, &n, f);
        if (e < 0) /* il file è finito */
        {
            if (buffer != NULL) 
            { 
                free(buffer);  /* dealloco il buffer usato per le linee */
                break;
           }         
        }
        /* esegue la tokenizzazione di buffer: legge primo token */
        char *s = strtok(buffer, ";\n");
        char *pop = strtok(NULL, ";\n");
        char *naz = strtok(NULL, ";\n");
        /* controllo che s, pop e naz siano validi */
        if (!s && !pop && !naz)
        {
            fprintf(stderr, "Errore linea\n");
            continue;
        }
        capitale *c = creaCapitale(s, atoi(pop), naz);
        root = inserisciABR(root, c);
    }
    return root;
}

/* cerca una capitale nell'abr */
capitale *ricercaABR(capitale *root, char *nome)
{
    if (root == NULL) { return NULL; }
    int cfr = strcmp(root->nome, nome);
    if (cfr == 0) { return root; }
    else if (cfr < 0) { return ricercaABR(root->right, nome); }
    else { return ricercaABR(root->left, nome); }
}


/* dato un abr di radice root restituisce la sua altezza */
int altezzaABR(capitale *root)
{
    if (root == NULL) { return 0; }
    int leftH = altezzaABR(root->left);
    int rightH = altezzaABR(root->right);
    return (leftH > rightH) ? leftH + 1 : rightH +1;
}

/* restituisce true se c ha latitudine tra 40 e 43 */
void ABRCondition(capitale *r, FILE *f, bool (*funz)(capitale *))
{
    if (r == NULL) { return; }
    ABRCondition(r->left, f, funz);
    if (funz(r))
    {
        capitaleStampa(r, f);
    }
    ABRCondition(r->right, f, funz);
}


int main(int argc, char **argv)
{
    if (argc < 2) 
    {
        printf("Uso: %s nomefile [nome1 nome2 ...]\n",argv[0]);
        exit(1);
    } 

    FILE *f = fopen(argv[1],"r");
    if(f==NULL) termina("Errore apertura file");
    
    if (f == NULL)
    {
        termina("Errore apertura file");
    }

    capitale *root = creaABR(f);
    if (fclose(f) == EOF)
    {
        termina("Errore chiusura");
    }

    puts("### INIZIO LISTA ###");
    stampaABR(root, stdout);
    puts ("### FINE LISTA ###");

    printf("Altezza: %d\n", altezzaABR(root));

    /* esegue la ricerca delle città passate sulla linea di comando */
    for (int i = 2; i < argc; i++)
    {
        capitale *c = ricercaABR(root, argv[i]);
        if (c == NULL)
        {
            printf("%s non trovata\n", argv[i]);
        }
        else
        {
            printf("trovata: ");
            capitaleStampa(c, stdout);
        }
    }

    distruggiABR(root);
    return 0;
}

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
