#include <stdio.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>


int main(int argc, char **argv)
{
    srand(time(NULL));
    if (mkfifo("sum", 0777) == -1) 
    {
        if (errno == EEXIST) 
        {
            fprintf(stderr, "making of sum failed");
            return 1;
        }
    }
    int arr[5];
    for (int i = 0; i < 5; i++) arr[i] = rand()%100;
    
    int fd = open("sum", O_WRONLY);
    if (fd == -1) return 2;

    /* We can specified the size of the array here (20 byte) and remove the for loop */
    if (write(fd, &arr, sizeof(int)*5) == -1) return 3; 
    // printf("Wrote %d\n", arr);
    /* Receives the sum */
    int s = 0;
    int fd2 = open("sum", O_RDONLY);
    if (fd2 == -1) return 3;
    ssize_t r = read(fd2, &s, sizeof(int));
    if (r == -1) return 4;
    printf("Sum received is: %d\n", s);
    close(fd2);


    return 0;
}
