// esempio produttore/consumatore tra processi
// utilizzando memoria condivisa e named semaphores
#include "xerrori.h"
// #include <time.h>

#define QUI __LINE__,__FILE__
#define Buf_size 10 
#define Shm        "/my_shm"
#define Sem_items  "/my_items"
#define Sem_slots  "/my_slots"



int main(int argc,char *argv[])
{
    if( argc != 2) 
    {
        fprintf(stderr,"Uso\n\t%s num_items\n", argv[0]);
        exit(1);
    }
    // conversione input
    int n = atoi(argv[1]);

    // ---- creazione buffer prod-cons in memoria condivisa
    int shm_size = Buf_size*sizeof(int);
    int fd = xshm_open(Shm, O_RDWR | O_CREAT, 0666, QUI);
    xftruncate(fd, shm_size, QUI);
    int *b = simple_mmap(shm_size,fd, QUI);
    close(fd); // dopo mmap e' possibile chiudere il file descriptor
    
    // ---- creazione semafori named
    sem_t *free_slots = xsem_open(Sem_slots,O_CREAT|O_EXCL,0666,Buf_size,QUI);
    sem_t *data_items = xsem_open(Sem_items,O_CREAT|O_EXCL,0666,0,QUI);
    // non devo usare unlink qui altrimenti il consumatore non
    // non vede memoria condivisa e semafori

    // loop produttore 
    int pindex = 0;
    for (int i = 0; i < n; ++i)
    {
        xsem_wait(free_slots, QUI);
        b[pindex % Buf_size] = i * 10;
        pindex++;
        xsem_post(data_items, QUI);
        printf("item %d written\n", i * 10);
    }

    // valore di terminazione
    xsem_wait(free_slots, QUI);
    b[pindex % Buf_size] = -1;
    pindex++;
    xsem_post(free_slots, QUI);
    printf("Item %d written\n", -1);

    xmunmap(b, shm_size, QUI);
    xsem_close(free_slots, QUI);
    xsem_close(data_items, QUI);

    return 0;
}
