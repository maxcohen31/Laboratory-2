#include "xerrori.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

// programma per il conteggio di numero dei primi in un
// intervallo usando piu' processi ausiliari
// e una pipe per comunicare i conteggi parziali 
// al processo genitore


/*
                --------------------------- 
 <---   write 1 |                         |   0 read <---
                |                         |
                ---------------------------
*/

#define QUI __LINE__, __FILE__

bool isPrime(int n)
{
    if (n < 2)
    {
        return false;
    }

    int first = 2;

    while (first < sqrt(n))
    {
        if (first % 2 == 0)
        {
            return false;
        }
        first++;
    }
    return true;
}

// conta i primi nel range [a, b)
int contaPrimi(int a, int b)
{
    int tot = 0;
    for (int i = a; i < b; ++i)
    {
        if (isPrime(i))
        {
            tot++;
        }
    }
    return tot;
}


int main(int argc, char **argv)
{
    if (argc != 4)
    {
        printf("Uso: \n\t%s n1 n2 p\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    int n1 = atoi(argv[1]);
    int n2 = atoi(argv[2]);
    int p = atoi(argv[3]);
    assert(n1 > 0 && n2 >=n1 && p > 0);

    // creo una pipe di comunicazione dai figli al genitore
    int up[2]; // la chiamo up perché la uso da figli a genitore
    xpipe(up, QUI);
    // genero processi figli
    for (int i = 0; i < p; ++i)
    {
        pid_t pid = xfork(QUI);
        if (pid == 0) // figlio
        {
            xclose(up[0], QUI);
            // il figlio calcola l'intervallo che deve analizzare
            int n = (n2-n1) / p; // quanti numeri verifica ogni figlio (più o meno)
            int start = n1 + (n2 * i); // inizio range figlio i
            int end = (i==p-1) ? n2 : n1 + (n * (i + 1));
            int tot = contaPrimi(start, end);
            printf("Figlio %d: cercato tra %d e %d, trovati %d primi\n", i, start, end, tot);
            ssize_t e = write(up[1], &tot, sizeof(int));

            if (e != sizeof(int))
            {
                termina("errore scrittura pipe");
            }
            xclose(up[1], QUI);
            exit(0);
        }
    }
    // qui arriva solo il genitore
    int tot = 0; 
    xclose(up[1], QUI);
    // leggo finchè tutti non hanno chiuso up[1]
    while (true)
    {
        int x;
        ssize_t e = read(up[0], &x, sizeof(int));
        if (e == 0)
        {
            break;
        }
        printf("Genitore: letto il valore %d dalla pipe\n", x);
        tot += x;
    }
    
}
