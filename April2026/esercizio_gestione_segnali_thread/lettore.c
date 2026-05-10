/*
 * lettore.c
 *   
 *   Crea un array di char in memoria condivisa di lunghezza 1000 di nome esame
 *   mediante fork+exec lancia l'eseguibile scrittore.out passandogli come argomento sulla linea di comando la stringa outfile
 *   attende l'arrivo di un segnale SIGUSR1 da parte dello scrittore per dare il via alla lettura
 *   apre in lettura il file infile e legge le linee una alla volta utilizzando getline; per ogni linea:
 *   
 *   - sia n la lunghezza della linea calcolata con strlen: se n>999 il lettore ignora la linea e passa alla successiva,
 *   altrimenti copia la linea con strcpy nella memoria condivisa.
 *   
 *   - se n è pari invia il segnale SIGRTMIN al processo scrittore.out, 
 *     se n è dispari invia il segnale SIGRTMIN+1 sempre al processo scrittore.out.
 *   
 *   - dopo l'invio del segnale, il lettore attende la ricezione del segnale SIGUSR1 da parte dello scrittore; 
 *     l'arrivo di tale segnale indica che la memoria condivisa è stata letta e può essere sovrascritta; 
 *     si passa quindi alla linea successiva del file
 *   
 *   - terminata la lettura del file, il programma invia a scrittore.out il segnale SIGRTMAX, 
 *   prenota la cancellazione della memoria condivisa, chiude i file, dealloca la memoria, e termina.
 *
 *   La barriera è un suggerimento nel testo dell'esercizio; così come l'uso dei semafori.
 *
 *   Vincoli: 
 *   - entrambi i programmi (lettore.c, scrittore.c) devono effettuare la gestione dei segnali utilizzando un thread separato che utilizza la funzione sigwait
 *   - nel programma lettore.out il thread gestore di segnali deve utilizzare un semaforo o una barriera 
 *      per segnalare al thread principale che è stato ricevuto il segnale SIGUSR1
 *   - nel programma scrittore.out il thread principale è quello che deve scrivere la stringa modificata sul file, 
 *      il thread gestore è quello che deve effettuare la conversione in maiuscolo o minuscolo e deve usare 
 *      un meccanismo di sincronizzazione a vostra scelta per indicare al thread principale la stringa da scrivere nel file 
 *      oppure la necessità di terminare (perché è stato ricevuto SIGRTMAX)
 *   - non è ammesso usare variabili globali
 * */


#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <signal.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdbool.h>
#include <fcntl.h>

#define SHM_NAME "/esame"
#define SIZE_EXAM 1000

typedef struct 
{
    sigset_t set;           /* Set of signals */
    pthread_barrier_t *br;  /* Barrier used to wake up main */
} data;

/* Reader thread 
 * Wait for SIGUSR1 to come. 
 * When SIGUSR1 arrives call pthread_barrier() 
 * If SIGRTMAX is received then the thread stops 
 * */
void *reader(void *args)
{
    printf("[R] Reader started\n");
    data *t = (data*)args;
    while (true)  
    {
        int s; /* Received signal */
        int e = sigwait(&t->set, &s);
        if (e != 0) 
        {
            fprintf(stderr, "[Error]: sigwait\n");
            exit(10);
        }
        printf("[R]: Signal %d received from %d\n", s, getpid()%100);
        pthread_barrier_wait(t->br); 
    }
    printf("[R]: Reader %d has finished!\n", getpid()%100);
    return NULL;
}

int main(int argc, char **argv)
{
    /* Shared memory region */
    int fd = shm_open(SHM_NAME, O_CREAT|O_RDWR, 0644); /* Opens shm for read/write or creates a new one */
    if (fd == -1)
    {
        perror("esame");
        exit(1);
    }
    /* Dimension of shared memory set to 1000 */
    if (ftruncate(fd, SIZE_EXAM) < 0)
    {
        perror("ftruncate");
        exit(2);
    }
    
    char *buf = mmap(NULL, SIZE_EXAM, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);
    if (buf == MAP_FAILED)
    {
        perror("mmap");
        exit(3);
    }
    close(fd);
    
    /* Blocking all signals before the fork() calling */
    sigset_t mask;
    sigfillset(&mask);
    pthread_sigmask(SIG_BLOCK, &mask, NULL); 

    /* We only have interest in SIGUSR1 */
    sigset_t set;
    sigemptyset(&set);
    sigaddset(&set, SIGUSR1);

    /* Forking */
    int pid_writer = fork();
    if (pid_writer < 0) 
    {
        perror("fork");
        exit(EXIT_FAILURE);
    }
    else if (pid_writer == 0) /* Child */
    {
        execl("./scrittore.out", "./scrittore.out", argv[2], (char*)NULL);
        perror("execl");
        exit(4);
    }

    /* Thread creation */
    pthread_t th;
    pthread_barrier_t barrier;
    pthread_barrier_init(&barrier, NULL, 2); /* Reader and main threads */
    data th_arr;

    th_arr.set = set;
    th_arr.br = &barrier;

    if (pthread_create(&th, NULL, &reader, &th_arr) != 0)
    {
        perror("pthread_create");
        exit(5);
    }
    
    /* Waiting for writer */
    pthread_barrier_wait(th_arr.br); 
    /* Opening infile and start reading */
    FILE *f = fopen(argv[1], "r");
    if (f == NULL)
    {
        perror("fopen");
        exit(6);
    }

    char *line = NULL; /* buffer for a line found in infile */
    size_t line_len= 0;
    ssize_t byte;
    while ((byte = getline(&line, &line_len, f)) != -1)
    {
        size_t n = strlen(line);
        if (n > 999) continue;
        /* Copy the line onto shared memory so that writer can read it */
        strcpy(buf, line);
        int sig = (n % 2 == 0) ? SIGRTMIN : SIGRTMIN + 1;
        kill(pid_writer, sig);
        /* Waitint for writer to finish */
        pthread_barrier_wait(&barrier); 
    }
    /* Sends SIGRTMAX to writer and makes it stop */
    kill(pid_writer, SIGRTMAX);

    if (pthread_detach(th) != 0)
    {
        perror("pthread_join");
        exit(7);
    }
        
    if (munmap(buf, SIZE_EXAM) == -1)
    {
        perror("mmap");
        exit(8);
    }

    shm_unlink(SHM_NAME);
    fclose(f);
    free(line);

    return 0;
}
