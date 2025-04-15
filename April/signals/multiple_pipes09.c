#define _GNU_SOURCE
#include <unistd.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <string.h>


/*
 *  L'esercizio consiste nel creare due processi figli. Dopodiché vogliamo mandare un intero
 *  creato dal processo padre, attraverso ogni processo. I processi figli incrementano questo intero di 5.
 *  Di quante pipe abbiamo bisogno? E quale pipe poi andrà chiusa?
 *  Abbiamo bisogno di 3 pipe: main -> p1 -> p2 -> main.
 *  Dove vanno aperte però? Per semplicità le apriamo tutte nel processo padre
 *  Quanti file descriptor? 6: 3 pipe con lettura e scrittura
 *  Ogni figli però erediterà i 6 file descriptor e questo ci porta a dire che in totale avremo 18 fd.
 *
 *  fd[0][1]: scrive padre -> fd[0][0]: legge figlio 1
 *  fd[1][1]: scrive figlio 1 -> fd[1][0]: legge figlio 2
 *  fd[2][1]: scrive figlio 2 -> fd[2][0]: legge padre
*/

int main(int argc, char **argv)
{
    int fd[3][2]; // matrice 3x2 che rappresenta i file descriptor

    // crea 3 pipe
    for (int i = 0; i < 3; i++)
    {
        // La gestione degli errori fatta in questo modo è sbagliata
        // ci vorrebbe un controllo più sicuro
        if (pipe(fd[i]) < 0)
        {
            return 1;
        }
    }

    pid_t pid1 = fork();
    if (pid1 < 0)
    {
        return 2;
    }

    // primo processo figlio
    if (pid1 == 0)
    {
        close(fd[0][1]); // chiudo scrittura del padre -> figlio perché il figlio 1 deve solo leggere da questa pipe, non scrivere
        close(fd[1][0]); // chiudo lettura della pipe figlio 1 -> figlio 2 perché figlio 1 deve solo scrivere verso il figlio 2
        close(fd[2][1]); // chiudo scrittura della pipe figlio 2 -> padre perché figlio 1 non serve qui
        close(fd[2][0]); // chiudo lettura della pipe figlio2 -> padre perché figlio 1 non ci interessa

        int x; // la variabile che sarà mandata attraverso i processi
        if (read(fd[0][0], &x, sizeof(int)) < 0)
        {
            return 3;
        }

        x+= 5;
        if (write(fd[1][1], &x, sizeof(int)) < 0)
        {
            return 4;
        }
        close(fd[0][0]);
        close(fd[1][1]);
        return 0; // voglio che il codice dopo questo sia eseguito dal padre
    }
    
    pid_t pid2 = fork();
    if (pid2 < 0)
    {
        return 5;
    }

    if (pid2 == 0)
    {
        close(fd[0][0]);
        close(fd[1][1]);
        close(fd[0][1]);
        close(fd[2][0]);

        int x; // la variabile che sarà mandata attraverso i processi
        if (read(fd[1][0], &x, sizeof(int)) < 0)
        {
            return 6;
        }

        x+= 5;
        if (write(fd[2][1], &x, sizeof(int)) < 0)
        {
            return 7;
        }
        close(fd[1][0]);
        close(fd[2][1]);
        return 0; // voglio che il codice dopo questo sia eseguito dal padre
    }

    // processo padre
    close(fd[0][0]);
    close(fd[1][0]);
    close(fd[1][1]);
    close(fd[2][1]);

    int x;
    if (write(fd[0][1], &x, sizeof(int)) < 0)
    {
        return 8;
    }

    if (read(fd[2][0], &x, sizeof(int)) < 0)
    {
        return 9;
    }

    printf("Result: %d", x);

    close(fd[0][1]);
    close(fd[2][0]);
    waitpid(pid1, NULL, 0);
    waitpid(pid2, NULL, 0);

    return 0;
}
