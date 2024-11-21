#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <errno.h>
#include <stdbool.h>
#include "strint.h"

#define Delimitatori " .,:;!?<>\"\n-"

void termina(const char *msg);

strint *crea_albero(FILE *f, int *parole_distinte, int *parole_totali)
{
    assert(f != NULL);
    strint *root = NULL;
    *parole_distinte = 0;
    *parole_totali = 0;
    // ciclo di lettura delle linee del file
    char *buffer = NULL;
    size_t n = 0;

    while (true)
    {
        ssize_t e = getline(&buffer, &n, f); // legge singola linea
        // file terminato
        if (e < 0)
        {
            free(buffer);
            break;
        }
        // tokenizzazione: legge primo token
        char *s = strtok(buffer, Delimitatori);
        while (s != NULL)
        {
            if (strlen(s) > 0)
            {
                *parole_totali += 1;
                // controllo se la stringa è presente o meno
                strint *nodo = abr_ricerca(root, s);
                if (nodo != NULL)
                {
                    nodo->n += 1; // incrementa l'intero associato se la parola è già presente
                }
                else // abbiamo una nuova parola 
                {
                    strint *si = strint_crea(s, 1);
                    root = abr_inserisci(root, si);
                    *parole_distinte += 1;
                }
            }
            s = strtok(NULL, Delimitatori); // legge prossima parola
        }
    }
    return root;

}


int riempi_array_nodi(strint *root, strint **a)
{
    if (root != NULL)
    {
        int messi_sx = riempi_array_nodi(root->left, a);
        a[messi_sx] = root;
        int messi_dx = riempi_array_nodi(root->right, &a[messi_sx+1]);

        return 1 + messi_sx + messi_dx;
    }
    return 0;
}

// funzione confronto per qsort
int confronta_freq(strint **a, strint **b)
{
    strint *pa = *a;
    strint *pb = *b;
    return pb->n - pa->n;
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
        termina("Error opening file");
    }
    // crea albero con le stringhe lette da file
    int tot_parole;
    int parole_dist;
    strint *root = crea_albero(f, &parole_dist, &tot_parole);
    fclose(f);

    fprintf(stderr, "Distinte: %d, nodi: %d\n", parole_dist, abr_strint_totnodi(root));
    fprintf(stderr, "Parole: %d, somma nodi: %d\n", tot_parole, abr_strint_sommanodi(root));

    // creo array di puntatori a nodi
    strint **a = malloc(parole_dist*sizeof(*a));
    if (a == NULL)
    {
        termina("Error allocating memory");
        exit(2);
    }

    int n = riempi_array_nodi(root, a);
    assert(n == parole_dist);

    qsort(a, n, sizeof(*a), (__compar_fn_t) &confronta_freq);
    puts ("**** stampa frequenza ****");

    for (int i = 0; i < n; i++)
    {
        strint_stampa(a[i], stdout);
    }
    free(a); // dealloca **a: noto l'albero esiste ancora

    puts("**** inizio visita ****");
    abr_strint_stampa(root, stdout);
    puts("**** fine visita ****");
    abr_strint_distruggi(root);

    return 0;
}

void termina(const char *msg)
{
    if (errno == 0)
    {
        fprintf(stderr, "%s\n", msg);
    }
    else
    {
        perror(msg);
    }
    exit(1);
}
