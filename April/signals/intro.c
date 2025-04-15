#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <sys/wait.h>

/*
 * TUTTO IL CODICE NELLA CARTELLA signals FA RIFERIMENTO AL CANALE YOUTUBE CodeVault!
*/

int main(int argc, char **argv)
{
    int pid = fork();
    if (pid == -1)
    {
        return 1;
    }

    if (pid == 0)
    {
        while (true)
        {
            printf("Some text goes here\n");
            usleep(5000);
        }
    }
    else
    {
        sleep(1);
        kill(pid, SIGKILL);
        wait(NULL);
    }

    return 0;
}
