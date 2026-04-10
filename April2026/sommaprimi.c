#include "xerrori.h"
#include <fcntl.h>
#include <unistd.h>

/* calcola il numero e la somma dei primi contenuti
 in alcuni file di testo 
 utilizza dei processi ausiliari per il conteggio
 la comunicazione avviene attraverso 
 shared memory e con il controllo di alcuni semafori 

 il processo genitore crea un processo figlio mediante 
 fork + exec passando ad ognuno di essi il nome di un file 
 i processi figli calcolano ognuno la somma dei primi del loro file
 e accumulano in memoria condivisa il numero totale di primi 
 Dopo che il processo principale ha stampato i risultati, i  
 processi figli ripartono e accedono a tutte le somme
 per trovare quale file conteneva la somma dei primi maggiore. 
 Si noti che due semafori sono in pratica usati come barrier 
 mentre il terzo è usato come un mutex */

#define QUI __LINE__, __FILE__
#define SH_MEM "/sommaprimi"
#define SH_SEM1 "/sommaprimi1"
#define SH_SEM2 "/sommaprimi2"
#define MUTEX "/sommaprimi3"

int main(int argc, char **argv)
{
    if (argc < 2)
    {
        fprintf(stderr, "Usage:\t%s file1 [file2 file3 ... ]\n", argv[0]);
        exit(1);
    }
    /* Number of auxiliary processes */
    int aux = argc-1;
    
    /* Shared memory creation */
    int shm_size = (aux * sizeof(int)) + sizeof(long); /* Using 4 + 4 + 8*aux bytes of shared memory */
    int fd = xshm_open(SH_MEM, O_RDWR|O_CREAT, 0660, QUI);
    xftruncate(fd, shm_size, QUI);
    char *tmp = simple_mmap(shm_size, fd, QUI);
    int *nump = (int*)(tmp);
    int *indice = (int*)(tmp + sizeof(int));
    long *sommap = (long*)(tmp + 2*sizeof(int));
    close(fd);

    /* Semaphores */
    sem_t *sem1 = xsem_open(SH_SEM1, O_CREAT|O_EXCL, 0666, 0, QUI);
    sem_t *sem2 = xsem_open(SH_SEM2, O_CREAT|O_EXCL, 0666, 0, QUI); 
    sem_t *mutex = xsem_open(MUTEX, O_CREAT|O_EXCL, 0666, 1, QUI);

    *nump = *indice = 0;
    for (int i = 0; i < aux; i++) sommap[i] = 0;

    /* Starting auxiliary processes */
    for (int i = 0; i < aux; i++)
    {
        if (xfork(QUI) == 0)
        {
            if (execl("sommaprimi_aux.out", "sommaprimi_aux.out", argv[i+1], (char*)NULL) == -1)
            {
                xtermina("Failed exec", QUI);
            }
        }
    }

    /* Waiting for processes to execute sem_post on sem1 to indicate the calculation is done */
    for (int i = 0; i < aux; i++)
    {
        xsem_wait(sem1, QUI);
    }

    /* All children have terminated. I can print the result */
    for (int i = 0; i < aux; i++)
    {
        printf("%12ld\n", sommap[i]);
    }

    /* Restart all children. Initially sem2 is 0 */
    for (int i = 0; i < aux; i++)
    {
        xsem_post(sem2, QUI);
    }
    
    /* Destroy everything in shared memory */
    xshm_unlink(SH_MEM, QUI);
    xsem_unlink(SH_SEM1, QUI);
    xsem_unlink(SH_SEM2, QUI); 
    xsem_unlink(MUTEX, QUI);

    xmunmap(tmp, shm_size, QUI);
    xsem_close(sem1, QUI);
    xsem_close(sem2, QUI);
    xsem_close(mutex, QUI);

    fprintf(stderr, "%d done\n", getpid());
    return 0;
}

