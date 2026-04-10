/*
 * Example of producer/consumer problem using processes and shared memory
 * */


#include "xerrori.h"
#include <fcntl.h>
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
        fprintf(stderr, "Usage:\t%s <num_items>\n", argv[0]);
        exit(1);
    }
    int n = atoi(argv[1]);

    /* Creation of the buffer prod/cons in shared memory */
    int shm_size = n * sizeof(int);
    int fd = xshm_open(SHM, O_RDWR | O_CREAT, 0666, QUI);
    xftruncate(fd, shm_size, QUI);
    int *buf = simple_mmap(shm_size, fd, QUI);
    close(fd);

    /* Named semaphores */
    sem_t *free_slots = xsem_open(SEM_SLOTS, O_CREAT|O_EXCL, 0666, BUF_SIZE, QUI);
    sem_t *data_items = xsem_open(SEM_ITEMS, O_CREAT|O_EXCL, 0666, 0, QUI);
    /* Do not use unlink otherwise the consumer will not be able to open the shared memory and the semaphores */

    /* Producerìs loop */
    int pindex = 0;
    for (int i = 0; i < n; i++)
    {
        xsem_wait(free_slots, QUI);
        buf[pindex++ % BUF_SIZE] = i;
        xsem_post(data_items, QUI);
        printf("Item %d written\n", i);
    }
    /* Termination value */
    xsem_wait(free_slots, QUI);
    buf[pindex++ % BUF_SIZE] = -1;
    xsem_post(data_items, QUI);
    printf("Item %d wriiten", -1);

    /* Book erasement of shm and sem*/
    xsem_unlink(SEM_SLOTS, QUI);
    xsem_unlink(SEM_ITEMS, QUI);
    xshm_unlink(SHM, QUI);

    /* Unmap shared memory and close the semaphores */
    xmunmap(buf, shm_size, QUI);
    xsem_close(free_slots, QUI);
    xsem_close(data_items, QUI);


    return 0;
}
