#define _GNU_SOURCE   // avverte che usiamo le estensioni GNU 
#include <stdio.h>    // permette di usare scanf printf etc ...
#include <stdlib.h>   // conversioni stringa/numero exit() etc ...
#include <stdbool.h>  // gestisce tipo bool (per variabili booleane)
#include <assert.h>   // permette di usare la funzione assert
#include <string.h>   // funzioni di confronto/copia/etc di stringhe
#include <errno.h>    // richiesto per usare errno
#include <math.h>

// Scopo del programma:
//  mostrare come si crea un file binario 
//  in questo esempio viene scritto un intero alla volta
//  ma se ho un array posso scriverlo con una singola fwrite 

void termina(const char *messaggio);


bool is_prime(int n)
{
    if (n <= 1) 
    {
        return false;
    } 

    int start = 2; // 2 is prime
    
    while (start <= sqrt(n))
    {
        if (n % start == 0) 
        {
            return false;
        }
        start++;
    }

    return true;
}

int main(int argc, char **argv)
{
    if (argc != 3)
    {
        printf("Uso: %s N nome_file", argv[0]);
        return 1;
    }

    int n = atoi(argv[1]);
    if (n < 0)
    {
        termina("Il parametro n deve essere positivo");
    }

    char *nome_file = argv[2];
    FILE *f = fopen(nome_file, "wb");
    /*
        // apro il file in scrittura usando open
        //   i permessi specificati nell'ultimo argomento
        //   valgono solo quando il file viene creato!
        //   i permessi effettivi dipendono anche dalla umask 
        int fd = open(nome_file,O_WRONLY|O_CREAT,0666); 
    */

    for (int i = 0; i < n; i++) 
    {
        if (is_prime(i))
        {
            int e = fwrite(&i, sizeof(i), 1, f);
            // int e = write(fd,&i,sizeof(i));
            if (e != 1)
            {
                termina("Erorre nella scrittura");
            }
        }
    }
    // se io avessi messo i primi in un array a[0...k-1],
    // li avrei potuti scrivere in f con l'istruzione
    //   fwrite(a,sizeof(*a),k,f);

    if (fclose(f) == EOF)
    {
        termina("Errore chiusura file");
    }

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
