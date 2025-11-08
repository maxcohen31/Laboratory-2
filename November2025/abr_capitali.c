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
    double lat;
    double lon;
    struct capitale_ *left;
    struct capitale_ *right;
} capitale;

void termina(const char *mess);

void capitaleStampa(const capitale *a, FILE *f)
{
    fprintf(f, "%-16s (%f, %f)\n", a->nome, a->lat, a->lon);
}

capitale *creaCapitale(char *s, double lat, double lon)
{
    capitale *a = malloc(sizeof(capitale));
    a->lat = lat;
    a->lon = lon;
    a->nome = strdup(s);
    a->left = NULL;
    a->right = NULL;

    return a;
}

void distruggiCapitale(capitale *c)
{
    free(c->nome);
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


/* legge un oggetto capitale dal file f e restituisce il puntatore all'oggetto letto */
capitale *leggiCapitale(FILE *f)
{
    assert(f != NULL);
    char *s;
    double lat;
    double lon;
    int e = fscanf(f, "%ms %lf %lf", &s, &lat, &lon);
    if (e != 3)
    {
        if (e == EOF) { return NULL; } /* file finito */
        else { termina("formato input file non valido"); }
    }
    capitale *c = creaCapitale(s, lat, lon);
    free(s);
    return c;
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
    while (1)
    {
        capitale *c = leggiCapitale(f);
        if (c == NULL)
        {
            break;
        }
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
    else if (cfr < 0) { return ricercaABR(root->left, nome); }
    else { return ricercaABR(root->right, nome); }
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
bool latrange(capitale *c)
{
  assert(c!=NULL);
  return (c->lat>=40.0) && (c->lat <= 43.0);
}

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

/* funzione che prende una capitale c e restituisce true se e solo se la città si trova a sud-est di Pisa (43.70853, 10.4036) */
bool pisaSE(capitale *c)
{
    return (c->lat <= 43.70853 && c->lon <= 10.4036);
}

/* funzione che conta il numero di città nel range [argv[2], argv[3]] */
int abr_ricerca_range(capitale *r, char *smin, char *smax)
{
    if (r == NULL)
    {
        return 0;
    }
    
    /* contatore */
    int res = 0;

    if (strcmp(r->nome, smax) <= 0 && strcmp(r->nome, smin) >= 0)
    {
        res += 1;
    }

    if (strcmp(r->nome, smin) >= 0)
    {
        res += abr_ricerca_range(r->left, smin, smax);
    }

    if (strcmp(r->nome, smax) <= 0)
    {
        res += abr_ricerca_range(r->right, smin, smax);
    }
    return res;
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
    capitaleStampa(root, stdout);
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

    /* stampa capitali che si trovano a Sud-Est di Pisa */
    printf("---- Capitali a Sud-Est di Pisa ----\n");
    ABRCondition(root, stdout, &pisaSE);

    printf("Città in range [%s, %s]: %d", argv[2], argv[3], abr_ricerca_range(root, argv[2], argv[3]));

    distruggiABR(root);
    return 0;
}

void termina(const char *messaggio)
{
  if (errno == 0) 
     fprintf(stderr,"%s\n",messaggio);
  else 
    perror(messaggio);
  exit(1);
}
