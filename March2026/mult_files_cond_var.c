/*
 * Esempio paradigma produttore consumatori
 * in cui abbiamo più di un produttore e più di consumatore
 * 
 * Soluzione che utilizza le condition variabiles invece dei semafori
 * Notare che si usano due condition variables associate 
 * allo stesso mutex
 * 
 * I produttori leggono gli interi dai file e li scrivono 
 * nel buffer, i consumatori calcolano il numero di 
 * primi e la loro somma
 * 
 * I risultati devono essere gli stessi ottenuti con multip_primi_files.c
 * 
 * */


#include "xerrori.h"
#include <pthread.h>
#include <time.h>

#define BUF_SIZE 10
#define QUI __LINE__, __FILE__

bool is_prime(int n)
{
    if (n < 2) return false;
    if (n % 2 == 0) return (n == 2);
    for (int i = 3; i*i <= n; i+=2)
    {
        if (n % i == 0) return false;
    }
    return true;
}

/* Consumer's struct */
typedef struct
{
    long somma;
    int quanti;
    int *buffer;
    int *pcindex;
    int *pdati;
    pthread_mutex_t *mu;
    pthread_cond_t *empty;
    pthread_cond_t *full; 
} consumer_data;

/* Producer's struct */
typedef struct
{
    char *filename;
    int *buffer;
    int *ppindex;
    int *pdati;
    pthread_mutex_t *mu;
    pthread_cond_t *empty;
    pthread_cond_t *full; 
} producer_data;


/* Consumer's function */
void *consumer(void *arg)
{
    consumer_data *a = (consumer_data*)arg;
    a->quanti = 0;
    a->somma = 0;
    fprintf(stderr, "[C] Consumer started!\n");
    int n = 0;
    
    while (n != -1)
    {
        xpthread_mutex_lock(a->mu, QUI);
        /* Buffer is empty: waiting */
        while (*(a->pdati) == 0)
        {
            xpthread_cond_wait(a->empty, a->mu, QUI);
        }
        n = a->buffer[*(a->pcindex) % BUF_SIZE];
        *(a->pcindex) += 1;
        *(a->pdati) -= 1;
        /* Signaling that the buffer is no longer full */
        xpthread_cond_signal(a->full, QUI);
        xpthread_mutex_unlock(a->mu, QUI);
        
        if (is_prime(n) && n > 0)
        {
            a->quanti++;
            a->somma += n;
        }
    }
    pthread_exit(NULL);
}

/* Producer's function */
void *producer(void *arg)
{
    producer_data*a = (producer_data*)arg;
    fprintf(stderr, "[P] Producer started!\n");
    int n;
    
    /* Open the file received from main */
    FILE *f = fopen(a->filename, "r");
    if (f == NULL) pthread_exit(NULL);
        /* Reading file */
    while (fscanf(f, "%d", &n) == 1)
    {
        xpthread_mutex_lock(a->mu, QUI);
        while(*(a->pdati) == BUF_SIZE)
        {
            xpthread_cond_wait(a->full, a->mu, QUI);
        }
        a->buffer[*(a->ppindex) % BUF_SIZE] = n;
        *(a->ppindex) += 1;
        *(a->pdati) += 1;
        /* Signaling that the buffer is no longer empty */
        xpthread_cond_signal(a->empty, QUI);
        xpthread_mutex_unlock(a->mu, QUI);  
    } 
    fclose(f);
    pthread_exit(NULL);
}

int main(int argc, char **argv)
{
    if (argc < 2) 
    {
        printf("Usage\t%s file1 [file2 ... fileK]\n", argv[0]);
        exit(1);
    }

    int num_of_consumers = 3;
    assert(num_of_consumers > 0);
    
    int num_of_producers = argc - 1;
    assert(num_of_producers > 0);
    
    int tot_primes = 0;
    long tot_sum = 0;
    
    /* Producers and consumers thread */
    int buffer[BUF_SIZE];
    int pindex = 0;
    int cindex = 0;
    int data = 0;
    pthread_mutex_t mu = PTHREAD_MUTEX_INITIALIZER;  
    pthread_cond_t empty = PTHREAD_COND_INITIALIZER;  
    pthread_cond_t full = PTHREAD_COND_INITIALIZER;  

    /* Threads's data */
    producer_data prod[num_of_producers];
    consumer_data cons[num_of_consumers];
    pthread_t th_prod[num_of_producers];
    pthread_t th_cons[num_of_consumers];

    /* Consumers creation */
    for (int i = 0; i < num_of_consumers; i++)
    {
        cons[i].pcindex = &cindex;
        cons[i].pdati = &data;
        cons[i].buffer = buffer;
        cons[i].mu = &mu;
        cons[i].empty = &empty;
        cons[i].full = &full;
        xpthread_create(&th_cons[i], NULL, &consumer, &cons[i], QUI);
    }
    fprintf(stderr, "Consumer threads created\n");

    /* Producers creation */
    for (int i = 0; i < num_of_producers; i++)
    {
        prod[i].ppindex = &pindex;
        prod[i].pdati = &data;
        prod[i].buffer = buffer;
        prod[i].mu = &mu;
        prod[i].empty = &empty;
        prod[i].full = &full;
        prod[i].filename = argv[i+1];
        xpthread_create(&th_prod[i], NULL, &producer, &prod[i], QUI);
    }

    /* Waiting for producers to finish */
    for (int i = 0; i < num_of_producers; i++) pthread_join(th_prod[i], NULL);

    /* Comunicating to consumers they can finish */
    for (int i = 0; i < num_of_consumers; i++)
    {
        xpthread_mutex_lock(&mu, QUI);
        while (data == BUF_SIZE) xpthread_cond_wait(&full, &mu, QUI);
        buffer[pindex % BUF_SIZE] = -1;
        pindex++;
        data++;
        xpthread_cond_signal(&empty, QUI);
        xpthread_mutex_unlock(&mu, QUI);
    }

    /* Waiting for consumers to finish */
    for (int i = 0; i < num_of_consumers; i++) 
    {
        pthread_join(th_cons[i], NULL);
        tot_primes += cons[i].quanti;
        tot_sum += cons[i].somma;
    }

    /* Deallocation */
    xpthread_mutex_destroy(&mu, QUI);
    xpthread_cond_destroy(&empty, QUI);
    xpthread_cond_destroy(&full, QUI);
    fprintf(stderr, "Found %d primes with sum: %ld\n", tot_primes, tot_sum);
    return 0;
}


