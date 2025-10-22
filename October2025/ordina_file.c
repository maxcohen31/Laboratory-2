/*
 * Scrivere un programma che legge dalla riga 
 * di comando un elenco di file e stampa 
 * questi file ordinati per dimensione decrescente, 
 * i file con la stessa dimensione devono essere 
 * ordinati lessicograficamente.
*/


#define _GNU_SOURCE
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>

/* struct representing a file */
typedef struct 
{
    char *name;
    long len;
} myfile;

/* creates a capital object by looking at its fields */
myfile *makeFile(char *fileName)
{
    assert (fileName != NULL);
    /* open file */
    FILE *f = fopen(fileName, "r");
    if (f == NULL)
    {
        fprintf(stderr, "Error: file did not open\n");
        return NULL;
    }
    /* we use fseek and ftell to get the size of the file */
    if (fseek(f, 0, SEEK_END) != 0)
    {
        fprintf(stderr, "Error: seek on file was not successful\n");
        fclose(f);
        return NULL;
    }

    long position = ftell(f); /* the pointer points to end of file */
    if (position < 0)
    {
        fprintf(stderr, "Error: ftell was not successful\n");
        fclose(f);
        return NULL;
    }
    fclose(f);

    myfile *file = malloc(sizeof(*file));
    if (!file) 
    {
        if (errno != 0) { perror("Error: not enough memory\n"); }
        else fprintf(stderr, "Error: not enough memory\n");
        return NULL;
    }

    file->name = strdup(fileName);
    file->len = position;

    return file;
}

/* deallocates a capital object */
void destroyFile(myfile *f) 
{ 
    free(f->name); 
    free(f); 
}

void printFiles(myfile **a, int size)
{
    for (int i = 0; i < size; ++i)
    {
        printf("%s ", a[i]->name);
    }
    printf("\n");
}

int compare(const void *a, const void *b)
{
   const myfile *f1 = *(const myfile **)a;
   const myfile *f2 = *(const myfile **)b;

    if (f1->len > f2->len) { return -1; }
    if (f1->len < f2->len) { return 1; }
    return strcmp(f1->name, f2->name);
}

int main(int argc, char **argv)
{
    if (argc < 2)
    {
        fprintf(stderr, "Uso: %s nome1 [nome2 nome3 ...]\n", argv[0]);
        exit(1);
    }
    
    /* declaring an array of pointers to myfile pointers */
    myfile **arr = malloc((argc - 1) * sizeof(myfile*));
    if (arr == NULL)
    {
        fprintf(stderr, "Error: myfile **arr malloc failed\n");
        exit(2);
    }
    /* elements successfully stored in the array */
    int n = 0;
    /* loop over each files passed to argc */
    for (int i = 1; i < argc; ++i)
    {
        /* trying to insert argv[i] */
        myfile *a = makeFile(argv[i]);
        if (a != NULL) { arr[n++] = a; }
    }

    printFiles(arr, n);
    qsort(arr, n, sizeof(myfile*), compare);
    printFiles(arr, n);
    
    /* release memory */
    for (int i = 0; i < n; ++i)
    {
        destroyFile(arr[i]);
    }
    free(arr);

    return 0;
}
