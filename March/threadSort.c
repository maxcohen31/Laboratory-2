#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistdio.h>
#include <pthread.h>
#include <assert.h>


// funzione usata dal thread ausiliario nella qsort
int intCmp(const void *x, const void *y);
// genera array di interi casuali
int *randomArray(int n, int seed);
// merge di due array
void merge(int *a1, int dim1, int *a2, int dim2, int *a);


// struct contenente i parametri di input e output dei thread
typedef struct 
{
    int *a; // array
    int dim; // dimensione
} dati;

// funzione eseguita dal thread ausiliario
void *routine(void *arg)
{
    dati *d = (dati*) arg;
    qsort(d->a, d->dim, sizeof(int), &intCmp);
    pthread_exit(NULL);
}

int main(int argc, char **argv)
{
    if (argc != 3)
    {
        fprintf(stderr, "Uso: %s interi seed", argv[0]);
        exit(1);
    }

    int n = atoi(argv[1]);
    assert (n > 0);
    int s = atoi(argv[2]);
    // calcolo somma prima dell'ordinamento
    long somma = 0;
    int *a = randomArray(n, s);

    for (int i = 0; i < n; ++i)
    {
        somma += a[i];
    }

    // ordinamento
    // l'array con solo un elemento è già ordinato
    if (n > 1)
    {
        int dimenN1 = n /2;
        int dimenN2 = n - dimenN1;
        int *a1 = malloc(dimenN1 * sizeof(int));
        int *a2 = malloc(dimenN2 * sizeof(int));
        assert(a1 != NULL && a2 != NULL);

        // copio la prima metà dell'array a in a1 e l'altrà metà in a2
        for (int i = 0; i < dimenN1; ++i)
        {
             a1[i] = a[i]; // prima metà
        }
        for (int j = 0; j < dimenN2; ++j)
        {
            a2[j] = a[dimenN1 + j]; // seconda metà
        }

        // preparo lancio thread ausiliario che ordinerà la seconda metà dell'array
        dati d;
        d.a = a2;
        d.dim = dimenN2; 
        pthread_t th;

        pthread_create(&th, NULL, &routine, &d);
        qsort(a1, dimenN1, sizeof(int), &intCmp);
        pthread_join(th, NULL);

        // merge
        merge(a1, dimenN1, a2, dimenN2, a);
        free(a1);
        free(a2);
    }

    // controllo ordinamento
    long somma2 = a[0];
    for (int i = 1; i < n; ++i)
    {
        somma2 += a[i];
        if (a[i-1] > a[i])
        {
            printf("Array non ordinato alla posizione %d\n", i);
            exit(3);
        }
    }
    if (somma != somma2)
    {
        printf("La somma degli elementi non corrisponde: %ld vs %ld\n", somma, somma2);
    }
    else
    {
        printf("Le somme coincidono, sorting apparentemente ok\n");
    }

    free(a);
    return 0;
}


int intCmp(const void *x, const void *y)
{
    return *(int*)x - *(int*)y;
}

// genera un array di n elementi compresi tra 0 e 999999
int *randomArray(int n, int seed)
{
    assert (n > 0);
    int *arr = malloc(n * sizeof(int));
    if (arr == NULL)
    {
        printf("Allocazione memoria per array fallita");
        exit(2);
    }
    
    srandom(seed);
    for (int i = 0; i < n; ++i)
    {
        arr[i] = (random() % 1000000);
    }
    return arr;
}

// funzione per il merge di due array in un terzo array già allocato
// merge di a[0...n1-1] e c[0... n2-1] dentro b[]
// Soluzione proposta da co-pilot apparentemente corretta
void merge(int a[], int na, int c[], int nc, int b[])
{
    assert(a!=NULL);
    assert(c!=NULL);
    assert(b!=NULL);
    assert(na>0);
    assert(nc>0);
    
    int i=0; // indice per a[]
    int j=0; // indice per c[]
    int k=0; // indice per b[]
    
    // scorro a[] e c[] e copio il minore in b[]
    while (i < na && j < nc) 
    {
        if(a[i]<c[j]) {
        b[k] = a[i];
        i++;
        } else {
        b[k] = c[j];
        j++;
        }
        k++;
    }
    
    // copio il resto di a[] in b[]
    while (i < na) 
    {
        b[k] = a[i];
        i++;
        k++;
    }
    
    // copio il resto di c[] in b[]
    while(j < nc) 
    {
        b[k] = c[j];
        j++;
        k++;
    }
    // asserzioni aggiunte da GM perché non si sa mai
    assert(i==na);
    assert(j==nc);
    assert(k==na+nc); 
}
