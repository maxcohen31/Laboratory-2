#define _GNU_SOURCE   // avverte che usiamo le estensioni GNU 
#include <stdio.h>    // permette di usare scanf printf etc ...
#include <stdlib.h>   // conversioni stringa exit() etc ...
#include <stdbool.h>  // gestisce tipo bool
#include <assert.h>   // permette di usare la funzione ass
#include <string.h>   // funzioni per stringhe
#include <errno.h>    // richiesto per usare errno
#include <math.h>
#include "strint.h"


void strint_stampa(const strint *a, FILE *f) {
  fprintf(f, "%20s (%d)\n", a->s, a->n);
}

strint *strint_crea(char *w, int v)
{
    strint *a  = malloc(sizeof(*a));
    a->n = v;
    a->s = strdup(w); // creo una copia di s
    a->left = NULL;
    a->right = NULL;
    return a;
}

void strint_distruggi(strint *a)
{
    free(a->s);
    free(a);
}

void abr_strint_stampa(const strint *root, FILE *f)
{
    if (root != NULL)
    {
        abr_strint_stampa(root->left, f);
        strint_stampa(root, f);
        abr_strint_stampa(root->right, f);
    }
}

void abr_strint_stampa_preorder(const strint *root, FILE *f, int depth)
{
    if (root != NULL)
    {
        strint_stampa(root, f);
        abr_strint_stampa(root->left, f);
        abr_strint_stampa(root->right, f);
    }
    else 
    {
        fprintf(f, "X\n");
    }
}

void abr_strint_distruggi(strint *root)
{
    if (root != NULL)
    {
        abr_strint_distruggi(root->left);
        abr_strint_distruggi(root->right);
        strint_distruggi(root);
    }
}

strint *abr_inserisci(strint *root, strint *c)
{
    c->left = NULL;
    c->right = NULL;

    if (root == NULL)
    {
        return c;
    }

    int ris = strcmp(root->s, c->s);
    
    if (ris == 0) // i nomi sono uguali
    {
        fprintf(stderr, "Nodo duplicato");
        strint_stampa(c, stderr);
        strint_distruggi(c);
    }
    else if (ris < 0) // vado a dx
    {
        return abr_inserisci(root->right, c);
    }
    else 
    {
        return abr_inserisci(root->left, c);
    }
}

strint *abr_ricerca(strint *root, char *w)
{
    if (root == NULL)
    {
        return NULL;
    }

    int cfr = strcmp(w, root->s);
    
    if (cfr == 0)
    {
        return root;
    }
    else if (cfr < 0)
    {
        // cerco a sx
        return abr_ricerca(root->left, w);
    }
    else 
    {
        // cerco a dx
        return abr_ricerca(root->right, w);
    }
}

int abr_altezza(strint *root)
{
    if (root == NULL)
    {
        return 0;
    }
    int h_sx = abr_altezza(root->left);
    int h_dx = abr_altezza(root->right);

    return (h_sx > h_dx) ? h_sx + 1 : h_dx + 1;
}

int abr_strint_sommanodi(const strint *root)
{
    int tot = 0;
    if (root != NULL)
    {
        tot += abr_strint_totnodi(root->left);
        tot += root->n;
        tot += abr_strint_totnodi(root->right);
    }

    return tot;
}

int abr_strint_totnodi(const strint *root)
{
    int tot = 0;
    if (root != NULL)
    {
        tot += abr_strint_totnodi(root->left);
        tot += 1;
        tot += abr_strint_totnodi(root->right);
    }

    return tot;


}
