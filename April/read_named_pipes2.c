#include <stdio.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>


int main(int argc, char **argv)
{
    
    int arr[5]; 
    int fd = open("sum", O_RDONLY);
    if (fd == -1) return 2;

    if (read(fd, &arr, sizeof(int)*5) == -1) return 3;
    close(fd);
    int sum = 0;
    for (int i = 0; i < 5; i++) sum += arr[i];

    /* Sends the sums back */
    int fd2 = open("sum", O_WRONLY); 
    if (fd2 == -1) return 4;
    if (write(fd2, &sum, sizeof(int)) == -1) return 5;
    printf("Sum sent\n");
    close(fd2);


    return 0;
}
