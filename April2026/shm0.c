#include "xerrori.h"
#include <fcntl.h>
#include <stdlib.h>
#include <time.h>

#define QUI __LINE__, __FILE__

/* Esempio semplice di di creazione di un array in memoriai condivisa
 * L'array viene semplicemente scritto, lo ritrovo in /dev/shm a meno
 * che non prenoti la cancellazione con sem_unlink */


int main(int argc, char **argv)
{
    if (argc != 3)
    {
        fprintf(stderr, "Usage:\t%s <shm_name> <dim>\n", argv[0]);
        exit(1);
    }
    /* Dimension */
    int n = atoi(argv[1]);
    if (n < 1) termina("Limit not valid");

    /* Array creation */
    int shm_size = n * sizeof(int); /* Number of bytes shared inside the memory */
    int fd = xshm_open(argv[1], O_RDWR | O_CREAT, 0666, QUI);
    xftruncate(fd, shm_size, QUI);
    int *a = simple_mmap(shm_size, fd, QUI);
    close(fd);
    /* After mmap is possible to close the file descriptor
     * Betting to book the erasement of the shared object */
    
    // xshm_unlink(argv[1], QUI); /* destroy the array */

    /* Fill up the array */
    for (int i = 0; i < n; i++) a[i] = i;
    
    /* Unmap the shared memory */
    xmunmap(a, shm_size, QUI);

    /* If i commented xshm_unlink() the object /dev/shm/name remains in the filesystem 
     * occupying the relative space until the next boot */

    return 0;
}
