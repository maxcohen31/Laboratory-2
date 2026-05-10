/*
 *   scrittore.c
 *  
 *   - apre in lettura la memoria condivisa esame (non deve eseguire ftruncate perché la dimensione è stabilita dal lettore)
 *   - apre in scrittura il file outfile il cui nome è stato passato in argv[1]
 *   - invia il segnale SIGUSR1 a lettore.out per far partire il processo di lettura
 *   - inizia un loop infinito nel quale attende la ricezione di uno dei segnali SIGRTMIN, SIGRTMIN+1 e SIGRTMAX:
 *   - se riceve SIGRTMIN copia la stringa che si trova nella memoria condivisa in un'altra zona di memoria, 
 *     converte i caratteri in minuscolo la scrive nel file outfile e invia il segnale SIGUSR1 a lettore.out per far leggere la linea successiva
 *   - se riceve SIGRTMIN+1 devono essere fatte le stesse operazioni tranne che la stringa va convertita in maiuscolo
 *   - se riceve SIGRTMAX esce dal loop
 *   - uscito dal loop il programma stampa su stdout il numero di linee scritte su outfile, chiude i file, dealloca la memoria e termina.
 *   
 * */

#include <assert.h>
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <stdbool.h>
#include <pthread.h>
#include <signal.h>
#include <semaphore.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>

#define SHM_NAME "/esame"

/* -----------------------------------------------------*/

/* Utility function - Converts a string to lowercase */
char *to_lower(char *s)
{
    assert(s != NULL);
    int i = 0;
    while (s[i] != 0)
    {
        if (s[i] >= 'A' && s[i] <= 'Z') s[i] = s[i] | 32;
        i++;
    }
    return s;
}

/* Converts a string to uppercase */
char *to_upper(char *s)
{
    assert(s != NULL);
    int i = 0;
    while (s[i] != 0)
    {
        if (s[i] >= 'a' && s[i] <= 'z') s[i] = s[i] & ~32;
        i++;
    }
    return s;
}

/* ------------------------------------------------------ */

typedef struct 
{
    int keep_going;   /* Variable used to terminate the loop in tha main */
    sigset_t set;     /* Set of signals */
    sem_t *sem;       /* Semaphore variable */
    char *shm;        /* Pointer to shared memory */
    int lines;        /* Lines counter */
    int s;            /* Signal received */
} writer_data;

void *writer(void *args)
{
    writer_data *w = (writer_data*)args;
    printf("[W]: Writer started with pid %d\n", getpid()%100);
    while (true)
    {
        /* Received signal */
        int s; 
        /* Wait on a pending signal. Stores it number in s */
        int sig_res = sigwait(&w->set, &s);
        printf("[W]: Writer received signal %d from %d\n", s, getppid()%100);
        if (sig_res != 0)
        {
            perror("sigwait");
            break;
        }
        if (s == SIGRTMIN)
        {
            to_lower(w->shm); /* A new line is inserted in shared memory */
            sem_post(w->sem); /* Notifies the main */
        }
        if (s == SIGRTMIN+1)
        {
            to_upper(w->shm);
            sem_post(w->sem);
        }
        if (s == SIGRTMAX) 
        {
            w->keep_going = 1; /* Exit the main loop in main */
            sem_post(w->sem);  /* Notifies the main thread */
        }
    }
    return NULL;
}

int main(int argc, char **argv)
{ 
    int esame = shm_open(SHM_NAME, O_RDWR, 0644);
    if (esame  == -1)
    {
        perror("shm_open");
        exit(1);
    }
   
    char *shared_mem = mmap(NULL, 1000, PROT_READ|PROT_WRITE, MAP_SHARED, esame, 0);
    if (shared_mem == MAP_FAILED)
    {
        perror("mmap");
        exit(2);
    }
    close(esame);

    FILE *f = fopen(argv[1], "w");
    if (f == NULL)
    {
        fprintf(stderr, "Error opening file\n");
        exit(3);
    }
    
    /* Blocking all signals */
    sigset_t mask;
    sigfillset(&mask);
    pthread_sigmask(SIG_BLOCK, &mask, NULL);
    
    sigset_t set;
    sigemptyset(&set);
    sigaddset(&set, SIGUSR1);    /* Sent by reader */
    sigaddset(&set, SIGRTMAX);   /* Sent to writer nad makes it stop */
    sigaddset(&set, SIGRTMIN);   /* Even length line in shared memory */
    sigaddset(&set, SIGRTMIN+1); /* Odd length line in shared memory */


    /* Threads creation */
    pthread_t wr;
    writer_data wr_data;
    sem_t semaphore;
    sem_init(&semaphore, 0, 0);
    wr_data.shm = shared_mem;
    wr_data.keep_going = 0;
    wr_data.lines = 0;
    wr_data.sem = &semaphore;
    wr_data.set = set;
     
    if (pthread_create(&wr, NULL, writer, &wr_data) == -1)
    {
        perror("pthread_create");
        exit(3);
    }
    
    /* Sends SIGUSR1 to reader. Reader is blocked on the barrier waiting for it to come */
    kill(getppid(), SIGUSR1);

    while (true)
    {
        sem_wait(&semaphore);
        printf("[W]: Woken up by thread\n");
        if (wr_data.keep_going) break;
        /* Writing on outfile */
        fprintf(f, "%s", wr_data.shm);
        wr_data.lines++;
        /* Sending SIGUSR1 to reader */
        kill(getppid(), SIGUSR1);
    }
    
    printf("Total lines: %d\n", wr_data.lines);

    pthread_detach(wr);

    munmap(shared_mem, 1000);
    fclose(f);
    sem_destroy(&semaphore);
   
    return 0;
}
