#include "xerrori.h"
#include <asm-generic/errno-base.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define QUI __LINE__, __FILE__

int main(int argc,char **argv)
{
    if (argc != 2)
    {
        printf("Usage:\t%s pipe_name", argv[0]);
        exit(EXIT_FAILURE);
    }

    /* Create the pipe used to communicate */
    int e = mkfifo(argv[1], 0660);
    if (e == 0) puts("Pipe succesfully created\n");
    else if (e == EEXIST) puts("Pipe already exists\n");
    else xtermina("Error creating the pipe\n", QUI);


#ifdef USA_EXEC
    /* I let start the reader */
    if (xfork(QUI) == 0)
    {
        /* We are violating the condtions and we pass argv[0] different from the program name. 
         * We can run a Python program though.
         * This is just a demonstrative example. DO NOT DO IT. */
        if (execl("lettore.out", "attore.???", argv[1], (char*)NULL) == 1) xtermina("Exec failed", QUI);
    }
#endif

    /* Open file descriptor */
    int fd = open(argv[1], O_WRONLY);
    if (fd < 0) xtermina("Error opening the file descriptor\n", QUI);
    puts("Start writing!\n");
    for (int val = 0; ; val++)
    {
        ssize_t e = write(fd, &val, sizeof(val));
        if (e != sizeof(val)) xtermina("Error writing the pipe\n", QUI);
        if (val % 10000 == 0) fprintf(stderr, "%d: wrote %d integers\n", getpid(), val);
    }
    xclose(fd, QUI);
    printf("%d i'm done\n", getpid());
    return 0;
}
