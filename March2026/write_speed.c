/*
 *  Measure the performance of fwrite and write of few bytes on multiple writes
 * */

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <stdbool.h>
#include <errno.h>
/* Required by open SC */
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
/* Required by me */
#include <time.h>


void termina(const char *msg);

int main(int argc, char **argv)
{
    if (argc != 2)
    {
        printf("Usage: %s #integers", argv[0]);
        exit(1);
    }

    int n = atoi(argv[1]);
    if (n < 0) termina("Parameter must be positive");

    /* fwrite */
    time_t start = time(NULL); /* Seconds from 1/1/1970 */
    FILE *f = fopen("fwrite.out", "wb");
    if (f == NULL) termina("Error opening file");
    for (int i = 0; i < n; i++)
    {
        if (fwrite(&i, sizeof(i), 1, f) != 1) termina("Error writing with fwrite\n");
    }
    fclose(f);
    printf("fwrite time: %ld secs", time(NULL)-start);

    /* Write */
    start = time(NULL);
    int fd = open("write_out", O_WRONLY|O_CREAT, 0666);
    if (fd < 0) termina("Failed opening file\n");
    for (int i = 0; i < n; i++)
    {
        if (write(fd, &i, sizeof(i)) != sizeof(int)) termina("Error writing with write\n");
    }
    close(fd);
    printf("write time: %ld secs", time(NULL)-start);

    return 0;
}

/* Write on stderr the given message. 
 * If errno is different than zero it prints out the message associated with errno */
void termina(const char *msg)
{
    if (errno == 0) fprintf(stderr, "%s\n", msg);
    else perror(msg);
    exit(1);
}


