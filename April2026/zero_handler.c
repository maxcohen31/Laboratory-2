/*
    Write a C program that accepts two integers as argument,
    divides the first number by the second. When the divisor is
    zero, the program shall catch the corresponding signal, rather
    than crashing.
*/

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>

volatile sig_atomic_t counter = 0;

void zero_division_handler(int s)
{
    counter++;
    if (s == SIGFPE) write(1, "Received SIGFPE, Divide by Zero Exception\n", 42);
    _exit(EXIT_FAILURE);

}

int main(int argc, char **argv)
{
    if (argc != 3)
    {
        fprintf(stderr, "Usage:\t%s <N1> <N2>\n", argv[0]);
        exit(1);
    }
    
    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = &zero_division_handler;
    if (sigaction(SIGFPE, &sa, NULL) == -1)
    {
        perror("sigaction() error");
        exit(2);
    }

    int num = atoi(argv[1]);
    int den = atoi(argv[2]);
    int res;

    printf("Num is %d\n", num);
    printf("Den is %d\n", den);

    do 
    {
        res = num / den;
        den--;
        printf("Den is now: %d\n", den);
    } while (true);

    /* Never reached */
    printf("Signals received: %d\n", counter);
}
