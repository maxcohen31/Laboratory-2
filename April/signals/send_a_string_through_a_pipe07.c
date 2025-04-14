#include <unistd.h>
#define _GNU_SOURCE
#include <string.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/wait.h>
#include <time.h>




int main(int argc, char **argv)
{
    int fd[2];
    if (pipe(fd) == -1)
    {
        exit(1);
    }

    pid_t pid = fork();
    if (pid == 0)
    {
        char s[200];
        printf("Input string: ");
        if (fgets(s, 200, stdin) == NULL)
        {
            perror("Error fgets\n");
            return -1;
        }
        s[strlen(s) - 1] = '\0';

        int numberOfChars = strlen(s) + 1;
        if (write(fd[1], &numberOfChars, sizeof(int)) < 0)
        {
            perror("Error sending the number of chars\n");
            exit(1);
        }

        if (write(fd[1], s, (strlen(s) + 1) * numberOfChars) < 0)
        {
            perror("Error writing into pipe\n");
            exit(2);
        }
        // finisce la scrittura
        close(fd[1]);
    }
    else 
    {
        close(fd[1]);
        char s[200]; 
        // dobbiamo ricevere dal processo figlio il numero di caratteri da leggere
        int numberOfChars;
         
        if (read(fd[0], &numberOfChars, sizeof(int)) < 0)
        {
            perror("Error reading the number of chars process\n");
            exit(3);
        } 

        if (read(fd[0], s, sizeof(char) * numberOfChars)  < 0)
        {
            perror("Error reading the number of chars process\n");
            exit(4);
        }

        printf("Received: %s", s);
        close(fd[0]); // chiude lettura
        wait(NULL);
    }

    return 0;
}
