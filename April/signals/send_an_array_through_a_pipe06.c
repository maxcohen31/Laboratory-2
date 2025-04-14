#define _GNU_SOURCE
#include <time.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <stdbool.h>

/*
 * 2 processi
 * Il processo figlio deve generare numeri casuali e mandarli al padre
 * Il processo padre somma tutti i numeri ricevuti e li stampa
 *
*/

int main(int argc, char **argv)
{
    // creo la pipe
    int fd[2];
    if (pipe(fd) == -1)
    {
        perror("Pipe error\n");
        exit(2);
    }
    
    // fork del processo
    pid_t pid = fork();
    if (pid == -1)
    {
        perror("Error pid\n");
        exit(1);
    }

    if (pid == 0)
    {
        close(fd[0]); // fd[0] -> read
        int n;
        int arr[10];
        srand(time(NULL));
        n = (rand() % 10) + 1;

        for (int i = 0; i < n; i++)
        {
            arr[i] = rand() % 11;
            printf("%d\n", arr[i]);
        }
        // scrive la dimensione dell'array nella pipe così che il padre possa leggerla
        if (write(fd[1], &n, sizeof(int)) < 0)
        {
            perror("Error sending n\n");
            exit(-1);
        }

        printf("Sent n = %d\n", n);
        // scrive l'array nella pipe
        if (write(fd[1], arr, sizeof(int) * n) < 0)
        {
            perror("Error writing into pipe\n");
            exit(2);
        }
        printf("Sent array\n");
        close(fd[1]); // chiude fd[1] -> scrittura
    }
    else 
    {
        close(fd[1]);
        // il processo padre inizia a leggere 
        // usiamo un altro array
        int n;
        int arr[10];
        int sum = 0;
        if (read(fd[0], &n, sizeof(int)) < 0)
        {
            perror("Error reading n in parent process\n");
            exit(3);
        }
        // per cosa multiplichiamo sizeof(int)? Di certo non n 
        // nel processo figlio dobbiamo mandare anche il numero di elementi che il padre dovrà leggere dalla pipe
        if (read(fd[0], arr, sizeof(int) * n) < 0)        
        {             
            perror("Error reading arr in parent process\n");
            exit(5);
        }
        close(fd[0]);        

        for (int i = 0; i < n; i++)
        {
            sum += arr[i];
        }

        printf("Result is %d\n", sum);
        wait(NULL);
    }
    return 0;
}
