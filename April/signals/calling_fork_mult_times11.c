#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

/*
 *
 * Descrizione:
 * - Si crea una "catena" di PROCESS_NUMB processi figli.
 * - Il processo padre invia un intero (es. 5) al primo processo figlio tramite una pipe.
 * - Ogni processo figlio legge il numero, lo incrementa di 1 e lo passa al successivo.
 * - L’ultimo processo invia il risultato finale al padre.
 * 
 * Struttura pipe:
 *     Padre --> P0 --> P1 --> ... --> Pn --> Padre
 *     Pipe[0]   Pipe[1]   ...   Pipe[n]
 * 
*/

#define PROCESS_NUMB 3

int main(int argc, char **argv)
{
    // parte di codice dove viene chiesto ad un utente il numero di precessi da creare
    // int n;
    // if (scanf("%d", &n) != 1 || n <= 0)
    // {
    //     printf("Invalid processes number\n");
    //     exit(EXIT_FAILURE);
    // }

    // int *pipes = malloc((n + 1) * sizeof(int[2]));
    // int *pids = malloc(n * sizeof(pid_t));

    int pipes[PROCESS_NUMB + 1][2]; // numero di pipe - 3 processi da creare più il padre necessitano di 4 pipe
    pid_t pids[PROCESS_NUMB]; // array di processi
    for (int i = 0; i < PROCESS_NUMB + 1; ++i)
    {
        if (pipe(pipes[i]) == -1)
        {
            printf("Error with creating pipe\n");
            exit(1);
        }
    }

    // come creo n processi?
    // for (int i = 0; i < PROCESS_NUMB; ++i)
    // {
    //     // SBAGLIATO! -> ad ogni ciclo si creano 2 processi
    //     fork();
    // }
    for (int i = 0; i < PROCESS_NUMB; ++i)
    {
        pids[i] = fork();
        if (pids[i] == -1)
        {
            printf("Error with forking pids\n");
            exit(2);
        }
        // processo figlio
        if (pids[i] == 0)
        {
            for (int j = 0; j < PROCESS_NUMB + 1; ++j)
            {
                if (i != j) // ogni figlio legge solo dalla sua pipe di ingresso (pipes[i][0])
                {
                    close(pipes[j][0]); // chiude tutte le letture
                }
                if (i + 1 != j) // ogni figlio scrive solo sulla sua pipe di uscita (pipes[i+1][1])
                {
                    close(pipes[j][1]); // chiude tutte le scritture
                }
            }

            int x;
            // legge l'intero dalla pipe in ingresso
            if (read(pipes[i][0], &x, sizeof(int)) == -1)
            {
                printf("Error at reading\n");
                exit(3);
            }

            printf("[%d] Received %d", i, x);
            x++;
            
            // scrive il valore incrementato sulla pipe in uscita
            if (write(pipes[i + 1][1], &x, sizeof(int)) == -1)
            {
                printf("Error at writing\n");
                exit(4);
            }

            printf("[%d] Sent %d", i, x);
            close(pipes[i][0]);
            close(pipes[i + 1][1]);
            return 0; // evita che il processo figlio richiami la fork
        }
    }

    // processo padre (main)
    int y = 5;
    printf("Sent %d\n", y);

    // il padre manda al primo figlio l'intero y
    if (write(pipes[0][1], &y, sizeof(int)) == -1)
    {
        printf("Error ar writing - main\n");
        exit(5);
    }
    // il padre aspetta l'ultimo figlio
    if (read(pipes[PROCESS_NUMB][0], &y, sizeof(int)) == -1)
    {
        printf("Error at reading\n");
        exit(6);
    }

    printf("Result: %d", y);

    // aspetto che i figli terminino
    for (int i = 0; i < PROCESS_NUMB; ++i)
    {
        wait(NULL);
    }

    return 0;
}
