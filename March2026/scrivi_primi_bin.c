#include <time.h>
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <errno.h>
#include <assert.h>

// Scopo del programma:
//  mostrare come si crea un file binario 
//  in questo esempio viene scritto un intero alla volta
//  ma se ho un array posso scriverlo con una singola fwrite

void termina(const char *messaggio);

bool is_prime(int n)
{
    if (n < 2) return false;
    if (n % 2 == 0) return n == 2;
    for (int i = 3; i*i <= n; i += 2)
    {
        if (n % i == 0) return false;
    }
    return true;
}


int main(int argc, char **argv)
{
    if (argc != 3) 
    {
        printf("Usage:\t%s N <file_name>\n", argv[0]);
        return 1;
    }

    int n = atoi(argv[1]); /* first parameter */
    /* Copy the pointer of argv[2] into file_name */
    char *file_name = argv[2]; 
    FILE *f = fopen(file_name, "wb");
    if (f == NULL) termina("Error opening file");

    /* Search for the first primes in range [2, n] and write them onto the file */
    for (int i = 2; i <= n; i++)
    {
        if (is_prime(i)) 
        {
            int e = fwrite(&i, sizeof(i), 1, f);
            if (e != 1) termina("Writing error");
        }
    }
    // se io avessi messo i primi in un array a[0...k-1],
    // li avrei potuti scrivere in f con l'istruzione
    //   fwrite(a,sizeof(*a),k,f);

    if (fclose(f) == EOF) termina("Error closing file");
    return 0;
}

void termina(const char *messaggio)
{
  if(errno==0) 
     fprintf(stderr,"%s\n",messaggio);
  else 
    perror(messaggio);
  exit(1);
}
