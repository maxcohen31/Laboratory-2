/*
 *  Credits go to CodeVault yt channel
 *  Named pipes (FIFO) - command: man 3 mkfifo  
 * */


#include <asm-generic/errno-base.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>


int main(int argc, char **argv)
{
    /* Creates a file called myfifo. This file can be read/wrote from/to anybody in the system */
    if (mkfifo("myfifo", 0777) == -1) 
    {
        if (errno != EEXIST)
        {
            fprintf(stderr, "Could not create myfifo file");
            return 1;
        }
    }
    /* ACHTUNG! if you read the following: Opening the read or write end of a FIFO blocks until the other end
     * is also opened (by another process or thread).  See fifo(7) for
     * further details. This programs hang untill someone try to read myfifo (example: open another terminal and type: cat myfifo) 
     * And you try reading myfifo the process hangs on untill someone writes something.
     * If you change O_WRONLY to O_RDWR the program writes and reads without hanging on 
     */ 
    printf("Opening...\n");

    int fd = open("myfifo", O_WRONLY); /* Opens the fifo to write something */
    if (fd == -1) return 3;
    int x = 97;
    if (write(fd, &x, sizeof(int)) == -1)
    {
        return 2;
    }
    printf("Written\n");
    close(fd);
    return 0;
}
