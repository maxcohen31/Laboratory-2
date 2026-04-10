#include "xerrori.h"
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define QUI __LINE__, __FILE__
#define BUF_SIZE 10
#define SHM "/my_shm"
#define SEM_ITEMS "/my_items"
#define SEM_SLOTS "/my_slots"


int main(int argc, char **argv)
{
    if (argc != 2)
    {
        fprintf(stderr, "Usage:\n\t%s\n", argv[0]);
        exit(1);
    }

    /* Is requested that the producer already made the semaphores 
     * and the shared memory for the buffer. It's also requested that
     * they have to be visible in /dev/shm */

    /* Open the buffer in shared memory */
    int shm_size = BUF_SIZE * sizeof(int);
    int fd = xshm_open(SHM, O_RDWR, 0, QUI);
    xftruncate(fd, shm_size, QUI);
    int *b = simple_mmap(shm_size, fd, QUI);
    close(fd);

    /* Open the named semaphores: the semaphores must exist. 
     * For this reason i don't specify mode and starting value */
    sem_t *free_slots = sem_open(SEM_SLOTS, 0);
    sem_t *data_items = sem_open(SEM_ITEMS, 0);
    if (!free_slots || !data_items) xtermina("Cannot open the sempahores", QUI);

    /* COnsumer's loop */
    int cindex = 0;
    while (true)
    {
        xsem_wait(data_items, QUI);
        int d = b[cindex++ % BUF_SIZE];
        xsem_post(free_slots, QUI);
        printf("Item %d read\n", d);
        if (d < 0) break; 
    }

    /* Unmap shared memory and close the semaphores */
    xmunmap(b, shm_size, QUI);
    xsem_close(data_items, QUI);
    xsem_close(free_slots, QUI);

    return 0;
}
