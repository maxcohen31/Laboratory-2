/*
 Modificare l'esempio shm_prod.c/shm_cons.c in modo che 
    1) vengano utilizzati 3 consumatori e 
    2) i consumatori (eseguibile shm_cons.out) vengono lanciati dal produttore shm_prod.out utilizzado fork e exec.

    Si osservi che i consumatori necessitano di una variabile cindex condivisa e di un relativo mutex per regolarne l'accesso; 
    a questo scopo conviene che il produttore crei un nuovo blocco di memoria condivisa con un unico intero
    e un ulteriore semaforo con nome da utilizzare come mutex (in alternativa a creare un nuovo blocco, 
    il blocco usato per il buffer può essere creato di dimensione Buf_size+1).
*/

#include "xerrori.h"
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>

#define QUI __LINE__, __FILE__
#define BUF_SIZE 10
#define SHM "/my_shm"           /* shared memory for circular buffer */
#define SEM_ITEMS "/my_items"   /* counts available items (consumer waits here) */
#define SEM_SLOTS "/my_slots"   /* counts free slots (producer waits here) */
#define MUTEX "/mutex"          /* binary semaphore to protect shared index */
#define SHM_INDEX "/my_index"   /* shared memory for consumer index */

int main(int argc, char **argv)
{
    if (argc != 2)
    {
        fprintf(stderr, "Usage:\t%s <num_items>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    int n = atoi(argv[1]); /* Number of items */

    /* Buffer creation */
    int shm_size = BUF_SIZE * sizeof(int);
    int fd = xshm_open(SHM, O_RDWR|O_CREAT, 0666, QUI);
    xftruncate(fd, shm_size, QUI);
    int *buf = simple_mmap(shm_size, fd, QUI);
    close(fd);

    /* cindex - shared index for consumers */
    int fd_index = xshm_open(SHM_INDEX, O_RDWR|O_CREAT, 0666, QUI);
    xftruncate(fd_index, sizeof(int), QUI);
    int *cindex = simple_mmap(sizeof(int), fd_index, QUI);
    close(fd_index);
    *cindex = 0;

    /* Semaphores creation */
    sem_t *free_slots = xsem_open(SEM_SLOTS, O_CREAT|O_EXCL, 0666, BUF_SIZE, QUI); /* Initially all buffer slots are free */
    sem_t *data_items = xsem_open(SEM_ITEMS, O_CREAT|O_EXCL, 0666, 0, QUI);
    sem_t *mutex = xsem_open(MUTEX, O_CREAT|O_EXCL, 0666, 1, QUI);

    /* Spawn three children (consumers) */
    for (int i = 0; i < 3; i++)
    {
        if (xfork(QUI) == 0) 
        {
            if (execl("./shm_cons2.out", "shm_cons2.out", (char*)NULL) == -1) xtermina("Execl failed", QUI);
        }
    }

    /* Producer's loop - fills up the buffer */
    int pindex = 0;
    for (int i = 0; i < n; i++)
    {
        xsem_wait(free_slots, QUI); /* wait for a free slot */
        buf[pindex++ % BUF_SIZE] = i;
        xsem_post(data_items, QUI); /* new item is available */
        printf("%d written\n", i);
    }

    /* Termination signal sent to consumers */
    for (int i = 0; i < 3; i++)
    {
        xsem_wait(free_slots, QUI);
        buf[pindex++ % BUF_SIZE] = -1;
        xsem_post(data_items, QUI);
    }

    /* Wait for the children to finish */
    for (int i = 0; i < 3; i++) wait(NULL);

    /* Release resources */
    xsem_unlink(SEM_ITEMS, QUI);
    xsem_unlink(SEM_SLOTS, QUI);
    xsem_unlink(MUTEX, QUI);
    xshm_unlink(SHM, QUI);
    xshm_unlink(SHM_INDEX, QUI);

    xmunmap(buf, shm_size, QUI);
    xmunmap(cindex, sizeof(int), QUI);
    xsem_close(free_slots, QUI);
    xsem_close(data_items, QUI);
    xsem_close(mutex, QUI);

    return 0;
}

