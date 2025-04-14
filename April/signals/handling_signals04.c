#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <signal.h>
#include <unistd.h>

/*
 *  Come posso far si che se premendo ctrl+z il programma non vada in background?
 *  Dovrò usare un handler
*/

void handleSIGTSTP(int sig)
{
    printf("Stop not allowed!");
}

void handleSIGCONT(int sig)
{
    printf("Insert number: ");
    fflush(stdout);
}

int main(int argc, char **argv)
{
    struct sigaction sa; // in questa struct possiamo definre come la stessa gestica i segnali
    // sa.sa_handler = &handleSIGTSTP;
    sa.sa_handler = &handleSIGCONT;
    sa.sa_flags = SA_RESTART; // stiamo usando una combinazione di SIGTSP e scanf.
                              // Sotto il cofano hanno un comportamento strano (documentarsi) se non usiamo 
                              // il flag SA_RESTART
    sigaction(SIGCONT, &sa, NULL);


    int x;
    printf("Input number: ");
    scanf("%d", &x);
    printf("Result for 4 * %d is %d", x, x * 4);
    return 0;
}
