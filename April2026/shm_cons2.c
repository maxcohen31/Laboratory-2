#include "xerrori.h"
#include <fcntl.h>

#define QUI __LINE__,__FILE__
#define BUF_SIZE 10 
#define SHM        "/my_shm"
#define SEM_ITEMS  "/my_items"
#define SEM_SLOTS "/my_slots"
#define MUTEX      "/mutex"
#define SHM_INDEX "/my_index"


int main(int argc,char *argv[])
{
    if (argc != 1) 
    {
        fprintf(stderr,"Uso\n\t%s\n", argv[0]);
        exit(1);
    }

    int shm_size = BUF_SIZE * sizeof(int);
    int fd = xshm_open(SHM, O_RDWR, 0, QUI);
    xftruncate(fd, shm_size, QUI);
    int *buf = simple_mmap(shm_size, fd, QUI);
    close(fd);

    int fd_index = xshm_open(SHM_INDEX, O_RDWR, 0, QUI);
    xftruncate(fd_index, sizeof(int), QUI);
    int *cindex = simple_mmap(sizeof(int), fd_index, QUI);

    sem_t *free_slots = xsem_open(SEM_SLOTS, 0, 0, 0, QUI);
    sem_t *data_items = xsem_open(SEM_ITEMS, 0, 0, 0, QUI);
    sem_t *mutex = xsem_open(MUTEX, 0, 0, 0, QUI);

    /* Consumer's loop */
    while (true)
    {
        xsem_wait(data_items, QUI);
        xsem_wait(mutex, QUI);
        int idx = *cindex;
        (*(cindex))++;
        xsem_post(mutex, QUI);
        int d = buf[idx % BUF_SIZE];
        xsem_post(free_slots, QUI);
        printf("Item %d read\n", d);
        if (d < 0) break;
    }

    xmunmap(buf, shm_size, QUI);
    xmunmap(cindex, sizeof(int), QUI);
    xsem_close(data_items, QUI);
    xsem_close(free_slots, QUI);
    xsem_close(mutex, QUI);
    return 0;
}

