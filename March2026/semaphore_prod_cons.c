/*
 * Esempio semplice paradigma produttore consumatori
 * Il produttore legge interi da un file e i consumatori calcolano 
 * la somma dei primi
 * 
 * Usare lo script numeri.py per generare lunghi elenchi di interi 
 * positivi su cui testare il programma
 * 
 * Programma di esempio del paradigma 1 producer 1 consumer
 * i dati letti dal file vengono messi su un buffer in cui il producer scrive 
 * e i consumer leggono. In principio il buffer va bene di qualsiasi dimensione: 
 * piu' e' grande maggiore e' il lavoro pronto da svolgere nel caso
 * il produttore rimanga bloccato (ad esempio a leggere dal disco)
 * 
 * */

#include "xerrori.h"
#include <math.h>
#include <stdio.h>

#define BUFF_SIZE 20
#define CONSUMERS 3


typedef struct 
{
    long somma;
    int *buffer;
    int *pcindex;
    sem_t *sem_free_slots;
    sem_t *sem_data_items;
    pthread_mutex_t *pmutex;
    int quanti;
} data;

/* Function that returns true if n is a prime. False otherwise */
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

void *consumer(void *args)
{
    data *d = (data*)args;
    d->quanti = 0;
    d->somma = 0;
    int n = 0;

    while (n != -1)
    {
        xsem_wait(d->sem_data_items, __LINE__, __FILE__);       // Wait for at least one data to consume

        xpthread_mutex_lock(d->pmutex, __LINE__, __FILE__);     // Acquire lock
        n = d->buffer[*(d->pcindex) % BUFF_SIZE];               // Read a number from the buffer
        (*d->pcindex)++;
        xpthread_mutex_unlock(d->pmutex, __LINE__, __FILE__);
        xsem_post(d->sem_free_slots, __LINE__, __FILE__);

        /* Check if the number read is positive and prime */
        if (n > 0 && is_prime(n))
        {
            d->quanti++;
            d->somma += n;
        }
    }
    puts("Consumer is about to end...");
    pthread_exit(NULL);
}


int main(int argc, char **argv)
{
#if 0
    dati d;
    pthread_mutex_t mu; // 40 byte 
    sem_t sem; // 32 byte
    
    printf("Size of dati is %zu\n", sizeof(d));
    printf("Size of mutex is %zu\n", sizeof(mu));
    printf("Size of sem is %zu\n", sizeof(sem));
#endif

    if (argc != 2) 
    {
        printf("Usage:\t <%s> <file>", argv[0]);
        exit(1);
    }

    int buffer[BUFF_SIZE];
    int tot_primes = 0;
    long result = 0;
    int n = 0;
    /* Thread related variables */
    int cindex = 0;
    int pindex = 0;
    pthread_mutex_t mu = PTHREAD_MUTEX_INITIALIZER;
    pthread_t thread_arr[CONSUMERS];
    data d[CONSUMERS];                                                                  // Array of data structs
    /* Semaphores */
    sem_t sem_free_slots; 
    sem_t sem_data_items;
    xsem_init(&sem_free_slots, 0, BUFF_SIZE, __LINE__, __FILE__);                       // At the beginnig there are 20 free slots
    xsem_init(&sem_data_items, 0, 0, __LINE__, __FILE__);                               // Zero items in the buffer

    for (int i = 0; i < CONSUMERS; i++)
    {
        d[i].buffer = buffer;
        d[i].somma = result;
        d[i].pcindex = &cindex;
        d[i].pmutex = &mu;
        d[i].sem_free_slots = &sem_free_slots;
        d[i].sem_data_items = &sem_data_items;
        xpthread_create(&thread_arr[i], NULL, &consumer, d+i, __LINE__, __FILE__);      // Instead &d[i] we use pointers arithmetic
    }
    puts("Auxiliary threads created");

    /* File operations */
    FILE *f = fopen(argv[1], "r");
    if (f == NULL)
    {
        perror("Error opening file");
        return 1;
    }
    while (fscanf(f, "%d", &n) == 1)
    {
        assert(n > 0);
        xsem_wait(&sem_free_slots, __LINE__, __FILE__);
        buffer[pindex % BUFF_SIZE] = n;
        pindex++;
        xsem_post(&sem_data_items, __LINE__, __FILE__);
    }
    puts("Data from file has been written");

    /* Termination signal */
    for (int i = 0; i < CONSUMERS; i++)
    {
        xsem_wait(&sem_free_slots, __LINE__, __FILE__);
        buffer[pindex % BUFF_SIZE] = -1;
        pindex++;
        xsem_post(&sem_data_items, __LINE__, __FILE__);
    }
    puts("Final values written");

    for (int i = 0; i < CONSUMERS; i++)
    {
        xpthread_join(thread_arr[i], NULL, __LINE__, __FILE__);
        tot_primes += d[i].quanti;
        result += d[i].somma;
        
    }
    xpthread_mutex_destroy(&mu, __LINE__, __FILE__);
    xsem_destroy(&sem_free_slots, __LINE__, __FILE__);
    xsem_destroy(&sem_data_items, __LINE__, __FILE__);
    fprintf(stderr, "Found %d primes with sum: %ld\n", tot_primes, result);
    return 0;
}
