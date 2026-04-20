/*
 *  Consider a program that reads memory location 0, which is an
    invalid location for user processes:

    int main ( ) {
        int a;
        a = *(int*) 0;
    }

    The program will cause a Segmentation fault. The task is to
    add a signal handler to trap the signal. 
    The desired output should be:

    i got signal 11
*/

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>

volatile sig_atomic_t signals = 0;

void sigfault_handler(int s)
{
    signals++;
    if (s == SIGSEGV) write(1, "I got signal 11\n", 16); 
    _exit(EXIT_FAILURE);
}

int main(int argc, char**argv)
{
    struct sigaction sa;                      /* Define signal handler */
    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = &sigfault_handler;
    if (sigaction(SIGSEGV, &sa, NULL) == -1)  /* SIGSEGV handler */
    {
        perror("sigaction() error");
        exit(1);
    } 

    int a;
    a = *(int*)0;

    /* This line will never be executed */
    printf("Exit program...\n");

    return 0;
}
