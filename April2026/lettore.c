/* Program that shows how to read integers from a named pipe */


#include "xerrori.h"

#define QUI __LINE__,__FILE__

int main(int argc, char **argv)
{
    if (argc != 2) 
    {
        printf("Usage:\t%s pipe_name", argv[0]);
        exit(EXIT_FAILURE);
    }

    /* Open the file descriptor associated with the pipe.
     * If the file does not exist returns an error */
    int fd = open(argv[1], O_RDONLY);
    if (fd < 0) termina("Error opening the named pipe\n");
    puts("lettore.out will start reading in 15 seconds\n");
    sleep(15);

    while (true)
    {
        int val;
        ssize_t e = read(fd, &val, sizeof(val));
        if (e == 0) break;
        if (val % 10000 == 0) printf("Read: %d\n", val);
    }
    xclose(fd, QUI);
    printf("Done reading!\n");

    return 0;
}
