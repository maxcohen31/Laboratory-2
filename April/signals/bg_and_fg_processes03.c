#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <signal.h>
#include <unistd.h>

/*
 *  The SIGTSTP signal is an interactive stop signal. Unlike SIGSTOP, this signal can be handled and ignored.
 *  ctrl+z
 *
 * 
*/

int main(int argc, char **argv)
{
    int x;
    printf("Input number: ");
    scanf("%d", &x);
    printf("Result: %d * 4 = %d", x, x * 4);
    return 0;
}
