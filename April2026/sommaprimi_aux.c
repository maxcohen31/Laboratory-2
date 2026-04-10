#include "xerrori.h"
#include <fcntl.h>
#include <stdbool.h>
#include <unistd.h>

/* programma per il conteggio di numero dei primi in un
   file di testo. 
   Usa shared memory e semafori per comunicare i risultati al
   genitore e agli altri processi figli
   vedere sommaprimi.c per i dettagli 
*/


#define QUI __LINE__,__FILE__
#define SH_MEM  "/sommaprimi"
#define SH_SEM1 "/sommaprimi1"
#define SH_SEM2 "/sommaprimi2"
#define MUTEX   "/sommaprimi3"

bool is_prime(int n)
{
    if (n < 2) return false;
    if (n % 2 == 0) return n == 2;
    for (int i = 3; i <= n/i; i++)
    {
        if (n % i == 0) return false;
    }
    return true;
}

int main(int argc, char **argv)
{
    if (argc != 2) 
    {
        fprintf(stderr, "Usage:\t%s <file_name>\n", argv[0]);
        exit(1);
    }

    // fprintf(stderr, "Processo %d (figlio di %d) iniziato\n",getpid(),getppid());
    // ---- creazione array memoria condivisa
    // la memria considvisa consiste delle aux+2 variabili:
    // | nump | indice | sommap[0] | sommap[1] | sommap[2] .... |
    int shm_size = (2 * sizeof(int)) + sizeof(long);
    int fd = xshm_open(SH_MEM, O_RDWR|O_CREAT, 0660, QUI);
    char *tmp = simple_mmap(fd, shm_size, QUI);
    int *nump = (int*)tmp;
    int *indice = (int*)(tmp + sizeof(int));
    long *sommap = (long*)(tmp + 2*sizeof(int));
    close(fd);

    /* Open the semaphores. DO NOT use O_CREAT othrwise the present value vanishes */
    sem_t *sem1 = xsem_open(SH_SEM1, 0,0,0, QUI);
    sem_t *sem2 = xsem_open(SH_SEM2, 0,0,0, QUI);
    sem_t *mutex = xsem_open(MUTEX, 0,0,0, QUI);

    /* Open the file and counts prime numbers */
    int tot = 0;
    long sum = 0;
    FILE *f = fopen(argv[1], "rt");
    if (f == NULL) printf("Error opening file %s\n", argv[1]);
    while (true)
    {
        int e;
        int n;
        e = fscanf(f, "%d", &n);
        if (e != 1) break;
        if (is_prime(n)) 
        {
            tot += 1;
            sum += n;
        }
    }
    fclose(f);

    /* Obtaining my identificator */
    xsem_wait(mutex, QUI); /* lock */
    int id = *indice;
    *indice += 1;
    xsem_post(mutex, QUI); /* unlock */
    printf("Process %d with file %s has obtained the id %d", getpid(), argv[1], id);

    /* Write the sum at my location */
    sommap[id] = sum;
    /* Increment the total of prime numbers - Mutex needed */
    xsem_wait(mutex, QUI);
    *nump += tot;
    xsem_post(mutex, QUI);

    /* Signal to the main process i have terminated the counting */
    xsem_post(sem1, QUI);
    /* Wait for the main process to start the second phase */
    xsem_wait(sem2, QUI);

    /* When i arrive here all children got their index and the job is done
     * *indice contains the number of children processes */
    long max_sum = 0;
    for (int i = 0; i < *indice; i++)
    {
        if (sommap[i] > max_sum) max_sum = sommap[i];
    }

    if (max_sum == sum) printf("Me process %d got the max sum: %ld", getpid(), max_sum);
    
    xsem_close(sem1, QUI);
    xsem_close(sem2, QUI);
    xsem_close(mutex, QUI);

    return 0;
}
