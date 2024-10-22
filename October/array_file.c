#define _GNU_SOURCE
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <errno.h>
#include <assert.h>
#include <string.h>



/**  
Esercizio:

Scrivere un programma che legge dalla riga 
di comando un elenco di file e stampa 
questi file ordinati per dimensione decrescente, 
i file con la stessa dimensione devono essere 
ordinati lessicograficamente.

Esercizio svolto tranne parte sull'ordinamento

**/

// struct che rappresenta un file con il nome e la dimensione
typedef struct 
{
    char *nome;
    long len;
} miofile;


// crea oggetto capitale a partire dai suoi campi
// la funzione prendein input il nome di una struct file
// e ne calcola la lunghezza
miofile *miofile_crea(char *s)
{
    assert(s != NULL);
    
    FILE *f = fopen(s, "r");
    if (f == NULL)
    {
        fprintf(stderr, "Error opening file");
        return NULL;
    }

    // move the file pointer to the end
    if (fseek(f, SEEK_END, 0) != 0)
    {
        fprintf(stderr, "Error fseek\n");
        fclose(f);
        return NULL;
    }

    // il puntatore è alla fine del file
    long pos = ftell(f);
    if (pos < 0)
    {
        fprintf(stderr, "Error ftell\n");
        fclose(f);
        return NULL;
    }
    fclose(f);

    // se siamo arrivati qui pos contiene
    // la lunghezza del file in byte
    miofile *a = malloc(sizeof(*a));
    if (a == NULL)
    {
        fprintf(stderr, "Not enough memory");
        exit(1);
    }
    
    a->nome = strdup(s);
    a->len = pos;

    return a;
}

void miofile_distruggi(miofile *a)
{
    free(a->nome);
    free(a);
}

void miofile_stampa(const miofile *a, FILE *f) 
{
    fprintf(f,"%20ld %s\n",a->len,a->nome);
}


int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        printf("Uso: %s nome1 nome2 ...", argv[0]);
        exit(1);    
    }

    // array di file di tipo miofile
    miofile **arr;

    // allocazione di memoria per l'array arr
    arr = malloc((argc-1) * sizeof(*arr));
    if (arr == NULL)
    {
        printf("Malloc fallita");
        exit(1);
    }

    int n = 0;

    for (int i = 0; i < argc; i++)
    {
        miofile *tmp = miofile_crea(argv[i]);
        if (tmp != NULL)
        {
            arr[n++] = tmp;
        }

    }

    for (int i = 0; i < n; i++)
    {
        fprintf(stdout, "%20ld %s\n", arr[i]->len, arr[i]->nome);
    }

    // for(int i=0;i<n;i++)
    // {
    //     miofile_stampa(arr[i], stdout);
    // }

    for(int i=0;i<n;i++)
    {
        miofile_distruggi(arr[i]);
    }
    free(arr);

    return 0;

}
