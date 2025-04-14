#define _GNU_SOURCE
#include <sys/wait.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <stdbool.h>

/*
 * Problema: vogliamo che l'utente risponda ad una domanda. Se la rispostaè corretta si ferma. 
 * Altrimenti, se per 5 secondi l'utente non risponde, vogliamo che l'utente riceva un indizio.
 * Come possiamo farlo usando due processi?
 *
 * 
*/

int x = 0;

void handleSIGUSR1(int sig)
{
    if (x == 0)
    {
        printf("\nHINT: Remember that multiplication is a sum\n");
    }
}

int main(int argc, char **argv)
{

    pid_t pid = fork();
    if (pid == -1)
    {
        perror("Error forking process\n");
        exit(1);
    }

    if (pid == 0) // processo figlio
    {
        sleep(5);
        kill(getppid(), SIGUSR1); // manda il segnale SIGUSR1 al processo padre
    }
    else // processo padre
    {
        struct sigaction sa = { 0 };
        sa.sa_flags = SA_RESTART;
        sa.sa_handler = &handleSIGUSR1;
        sigaction(SIGUSR1, &sa, NULL);

        printf("What is the result of 3 x 5? ");
        scanf("%d", &x);

        if (x == 15)
        {
            printf("Right!\n");
        }
        else 
        {
            printf("Wrong\n");
        }
        wait(NULL); // aspetta il processo figlio che termini
    }
    return 0;
}
