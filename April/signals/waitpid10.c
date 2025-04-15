#include <sys/types.h>
#include <unistd.h>
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>


int main(int argc, char **argv)
{
    pid_t pid1 = fork();
    if (pid1 < 0)
    {
        printf("Error forking pid1\n");
        exit(1);
    }
    
    if (pid1 == 0)
    {
        sleep(3);
        printf("Finished execution [%d]\n", getpid());
        return 0;
    }

    pid_t pid2 = fork();
    if (pid2 < 0)
    {
        printf("Error forking pid2\n");
        exit(1);
    }
    
    if (pid2 == 0)
    {
        sleep(1);
        printf("Finished execution [%d]\n", getpid());
        return 0;
    }


    // aspetta i processi in ordine casuale
    // wait(NULL);
    // wait(NULL);


    // se volessimo aspettare per primo il processo più lungo come potremmo fare? Basta usare waitpid
    int pid1Res = waitpid(pid1, NULL, 0); // aspetto prima pid1 anche se pid2 è terminato
    printf("Waited for %d\n", pid1Res);
    int pid2Res = waitpid(pid2, NULL, 0);
    printf("Waited for %d\n", pid2Res);

    // versione con WIFEXITED 

    // int status;
    // int pid1Res1 = waitpid(pid1, &status, 0); // aspetto prima pid1 anche se pid2 è terminato
    // if (pid1Res1 == -1)
    // {
    //     perror("Error pidRes1\n");
    //     exit(1);
    // }
    // if (WIFEXITED(status))
    // {
    //     printf("Child process %d terminated with code %d", pid1Res1, WEXITSTATUS(status));
    // }

    // int pid1Res2k= waitpid(pid1, &status, 0); // aspetto prima pid1 anche se pid2 è terminato
    // if (pid1Res2 == -1)
    // {
    //     perror("Error pidRes1\n");
    //     exit(1);
    // }
    // if (WIFEXITED(status))
    // {
    //     printf("Child process %d terminated with code %d", pid1Res2, WEXITSTATUS(status));
    // }
  
    return 0;
}
