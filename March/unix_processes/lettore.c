/*
    Programma per lettura di numeri interi da una pipe
    
    command example: mkfifo pipe1
*/

#include "xerrori.h"
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>

#define QUI __LINE__,__FILE__

int main(int argc, char **argv)
{
    if (argc != 2)
    {
        printf("Uso:\n\t%s nome_pipe\n", argv[0]);
        exit(1);
    }
    // apre file descriptor associato alla named pipe
    // se il file non esiste termina con errore
    int fd = open(argv[1], O_RDONLY);
    if (fd < 0)
    {
        termina("Errore apertura file\n");
    }
    puts("Inizio lettura\n");

    while (true)
    {
        int val;
        ssize_t e = read(fd, &val, sizeof(val));
        if (e == 0)
        {
            break;
        }
        if (val % 10000 == 0)
        {
            printf("Letto: %d\n", val);
        }
    }
    xclose(fd, QUI);
    printf("Lettura finita\n");
    return 0;
}
