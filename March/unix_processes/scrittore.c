#include "xerrori.h"
#include <fcntl.h>
#include <stdio.h>
#include <sys/stat.h>
#include <unistd.h>

#define QUI __LINE__,__FILE__

// definire questa costante per far eseguire 
// il lettore con execl() 
#define USA_EXEC

int main(int argc, char *argv[])
{ 
    if (argc != 2) 
    {
        printf("Uso:\n\t%s nome_pipe\n",argv[0]);
        exit(1);
    }

    // creo la named pipe per le comunicazione
    int e = mkfifo(argv[1], 0660);
    if (e == 0)
    {
        puts("Pipe creata\n");
    }
    else if (e == EEXIST)
    {
        puts("Pipe già esistente\n");
    }
    else
    {
        xtermina("Errore creazione pipe\n", QUI);
    }

#ifdef USA_EXEC
    // faccio partire il lettore, eventualmente anche un programma python
    if (xfork(QUI) == 0) 
    {
            if (execl("lettore.py", "lettore.???", argv[1], (char *) NULL) == -1)
            xtermina("execl fallita",QUI);
    }
#endif

    // apre file descriptor
    int fd = open(argv[1], O_WRONLY);
    if (fd < 0)
    {
        xtermina("Errore apertura pipe", QUI);
    }

    // scrive interi sulla pipe per sempre 
    printf("Inizio a scrivere\n");
    for (int i = 0; ; i++)
    {
        ssize_t e = write(fd, &i, sizeof(i));
        if (e != sizeof(i))
        {
            xtermina("Errore scrittura pipe", QUI);
        }
        if (i % 1000 == 0)
        {
            fprintf(stderr, "%d: scritti %d interi\n", getpid(), i);
        }
    }
    xclose(fd, QUI);

    printf("Io %d ho finito\n", getpid());
    return 0;
}

