#define _GNU_SOURCE
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <time.h>
#include <sys/wait.h>
#include <stdbool.h>


int main(int argc, char **argv)
{
    pid_t pid = fork();
    if (pid == -1)
    {
        return 1;
    }

    if (pid == 0) // processo figlio
    {
        while (true)
        {
            printf("Some output\n");
            usleep(50000);
        }
    }
    else // processo padre
    {
        kill(pid, SIGSTOP);

        int t;
        do 
        {     
            printf("Time in seconds for execution: ");
            scanf("%d", &t);
            if (t > 0)
            {
                kill(pid, SIGCONT);
                sleep(t);
                kill(pid, SIGSTOP);
            }
        }
        while (t > 0);

        // kill(pid, SIGSTOP); // stoppa il figlio
        // sleep(1);
        // kill(pid, SIGCONT); // il processo figlio riprende l'esecuzione
        // sleep(1);
        kill(pid, SIGKILL);
        wait(NULL); // aspetta il figlio che termini
    }
}
