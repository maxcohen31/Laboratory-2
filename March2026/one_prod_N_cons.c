/*
 * Esempio paradigma 1 produttore - p consumatori
 * Il produttore legge interi dai file sulla linea di
 * comando e li mette sul buffer. i consumatori calcolano 
 * il numero e la somma dei primi che hanno visto
 * 
 * Il produttore quando tutti i consumatori hanno
 * finito su di un certo file calcola e stampa
 * il numero totale e la somma totale dei primi 
 * nel file.
 * 
 * Viene usata una pthread_barrier per sincronizzare
 * produttori e consumatori alla fine di ogni file 
 * 
 * Vedere le pagine man di: 
 *    pthread_barrier_init
 *    pthread_barrier_wait
 * 
 * 
 * */

#include "xerrori.h"
#include <bits/floatn-common.h>
#include <math.h>

#define BUF_SIZE 10
#define THREAD_CONSUMER 4

bool is_prime(int n)
{
    if (n < 2) return false;
    int first_prime = 2;
    while (first_prime <= sqrt(n)) 
    {
        if (n % first_prime == 0) return false;
        first_prime++;
    }
    return true;

}

/* Struct containing input and output parameters shared among all threads */
typedef struct
{
    long sum;
    int numfiles; 
    int quanti;
    int *buffer;
    int *pcindex;
    pthread_mutex_t *mu;
    sem_t *sem_free_slots;
    sem_t *sem_data_items;
    pthread_barrier_t *end_of_file;
} dati;

/* Consumer thread's function */
void *consumer(void *args)
{
    dati *a = (dati*)args;
    fprintf(stderr, "[C] Consumer started\n");

    /* For each file compute how many primes there are */
    for (int i = 0; i < a->numfiles; i++)
    {
        int n = 0;
        int quanti = 0 ;
        long s = 0;

        while (n != -1)
        {
            xsem_wait(a->sem_data_items, __LINE__, __FILE__);
            xpthread_mutex_lock(a->mu, __LINE__, __FILE__);
            n = a->buffer[*(a->pcindex) % BUF_SIZE];
            *(a->pcindex) += 1;
            xpthread_mutex_unlock(a->mu, __LINE__, __FILE__);
            xsem_post(a->sem_free_slots, __LINE__, __FILE__);
            
            if (is_prime(n) && n > 0)
            {
                quanti++;
                s += n;
            }
        }
        fprintf(stderr, "[C] EOF signal received\n");
        a->quanti = quanti;
        a->sum = s;
        /* Waiting before passing to the next file */
        xpthread_barrier_wait(a->end_of_file, __LINE__, __FILE__);
    }
    pthread_exit(NULL);
}

int main(int argc, char **argv)
{
    if (argc < 2)
    {
        fprintf(stderr, "Usage:%s file1 ... fileN", argv[0]);
        exit(1);
    }
    int tot_primes = 0;
    long tot_sum = 0;
    int cindex = 0;                                                                         // read
    int n = 0;
    /* Threads initialization */
    dati t_arr[THREAD_CONSUMER];
    pthread_t thread_arr[THREAD_CONSUMER];
    int buffer[BUF_SIZE];
    int pindex = 0;                                                                         // write
    /* Mutex */
    pthread_mutex_t mu = PTHREAD_MUTEX_INITIALIZER;
    /* Semaphores */
    sem_t sem_free_slots;
    sem_t sem_data_items;
    xsem_init(&sem_free_slots, 0, BUF_SIZE, __LINE__, __FILE__);
    xsem_init(&sem_data_items, 0, 0, __LINE__, __FILE__);
    /* Barrier */
    pthread_barrier_t eof_barrier;
    xpthread_barrier_init(&eof_barrier, NULL, THREAD_CONSUMER+1, __LINE__, __FILE__);

    for (int i = 0; i < THREAD_CONSUMER; i++)
    {
        t_arr[i].numfiles = argc-1;
        t_arr[i].buffer = buffer;
        t_arr[i].pcindex = &cindex;
        t_arr[i].mu = &mu;
        t_arr[i].sem_data_items = &sem_data_items;
        t_arr[i].sem_free_slots = &sem_free_slots;
        t_arr[i].sum = tot_sum;
        t_arr[i].quanti = tot_sum;
        t_arr[i].end_of_file = &eof_barrier;
        xpthread_create(&thread_arr[i], NULL, consumer, &t_arr[i], __LINE__, __FILE__);
    }
    fprintf(stderr, "[P] %d auxiliary threads created\n", THREAD_CONSUMER);
    /* Reading all files in argv */
    for (int i = 1; i < argc; i++)
    {
        fprintf(stderr, "[P] Working on %s\n", argv[i]);
        FILE *f = xfopen(argv[i], "r", __LINE__, __FILE__);
        /* Reading data from file */
        while (fscanf(f, "%d", &n) == 1)
        {
            assert(n > 0);
            xsem_wait(&sem_free_slots, __LINE__, __FILE__);
            buffer[pindex++ % BUF_SIZE] = n;
            xsem_post(&sem_data_items, __LINE__, __FILE__);
        }
        fclose(f);
        fprintf(stderr, "[P] Data from file %s has been written\n", argv[i]);
        /* Writing THREAD_CONSUMER copies of file termination 
         * Each consumer must receive its termination signal */
        for (int k = 0; k < THREAD_CONSUMER; k++)
        {
            xsem_wait(&sem_free_slots, __LINE__, __FILE__);
            buffer[pindex++ % BUF_SIZE] = -1;
            xsem_post(&sem_data_items, __LINE__, __FILE__);
        }
        /* Waiting for all the consumers to finish with the current file */
        xpthread_barrier_wait(&eof_barrier, __LINE__, __FILE__);
        /* Calculate and show the result of file i */
        tot_primes = 0;
        tot_sum = 0;
        for (int j = 0; j < THREAD_CONSUMER; j++)
        {
            tot_primes += t_arr[j].quanti;
            tot_sum += t_arr[j].sum;
        }
        printf("File: %s: Primes found: %d. Sum_of_primes: %ld\n", argv[i], tot_primes, tot_sum);
    }
    /* No more files */
    for (int i = 0; i < THREAD_CONSUMER; i++)
    {
        xpthread_join(thread_arr[i], NULL, __LINE__, __FILE__);
    }
    pthread_mutex_destroy(&mu);
    xsem_destroy(&sem_data_items, __LINE__, __FILE__);
    xsem_destroy(&sem_free_slots, __LINE__, __FILE__);
    xpthread_barrier_destroy(&eof_barrier, __LINE__, __FILE__);

    return 0;
}

