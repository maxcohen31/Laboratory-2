#define _GNU_SOURCE
#include <stddef.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

/*
    Utilizzare il programma scrivi_primi_bin.c per creare un file binario contenente tutti i primi minori di 10000.

    Scrivere un nuovo programma ricbin.c che invocato scrivendo
    
    ricbin.out file_primi file_interi 
    procede nel seguente modo:
    
    . legge il contenuto di file_primi in un array int a[] che quindi conterrà tutti i primi minori di 10000
    . crea 3 thread consumatori
    . apre il file di testo file_interi legge tutti gli interi in esso contenuti e li scrive sul buffer produttori/consumatori
    
    I consumatori leggono degli interi dal buffer e per ogni intero letto determinano se è primo con il seguente procedimento:
    
    se l'intero è minore di 10000 verifica se è primo cercandolo nell'array a[ ] utilizzando obbligatoriamente la funzione bsearch(3)
    se l'intero è maggiore di 10000 lo divide per tutti gli elementi di a[ ] fino a quando non incontra un a[i] 
    il cui quadrato è maggiore dell'intero. L'intero sarà primo solamente se nessuno di questi elementi di a[ ] lo divide con resto 0.
    
    Ogni consumatore deve tenere traccia del numero dei primi che ha ricevuto e della loro somma, la somma in una variabile di tipo long (64 bit). 
    Tali valori devono essere poi passati al thread principale che deve stampare su stdout il numero totale di primi 
    e salvare in formato binario sul file somma.bin la somma dei primi (scrivendo quindi 8 byte). 
    Per visualizzare il valore scritto nel file potete usare il comando od -An -td8 somma.bin.
    
    Per testare il programma usare lo script interi.py. Scrivendo
    
    interi.py 1000 > 1000int.txt
    viene creato il file 1000int.txt contenente 1000 interi e viene visualizzato quanti sono i primi contenuti in esso e la loro somma. 
*/

#define BUF_SIZE 10
#define CONS_THREAD 3

/* Consumer's struct */
typedef struct 
{
    int *cindex;           /* Current buffer position */
    int *buffer;
    int *primes;
    int primes_dim;
    int received;          /* number of primes encountered */
    long sum;
    pthread_mutex_t *mu;
    sem_t *empty;
    sem_t *full;
} consumer_t;

/* ===================================================== */

/* Function used by the consumer thread */ 
int compare(const void *a, const void *b)
{
    return (*((int*)a) - *((int*)b));
}

bool find_prime(int *arr, int dim, int n)
{
    if (n < 2) return false;
    if (n % 2 == 0) return n == 2;
    if (n < 10000) 
    {
        int *el = (int*)bsearch(&n, arr, dim, sizeof(int), compare);
        return el != NULL;
    }
    else 
    {
        for (int i = 0; i < dim; i++)
        {
            long s = arr[i];
            if (s * s > n) break;
            if (n % s == 0) return false;
        }
    }
    return true; 
}
/* ====================================================== */

/* Consumer thread */
void *cons_thread(void *arg)
{
    consumer_t *c = (consumer_t*)arg;
    long sum = 0;
    int received = 0;
    int n = 0;
    printf("[C] Consumer %d has started\n", gettid() % 100);

    while (n != -1)
    {
        sem_wait(c->full);
        pthread_mutex_lock(c->mu);
        n = c->buffer[*(c->cindex) % BUF_SIZE];
        (*(c->cindex))++;
        pthread_mutex_unlock(c->mu);
        sem_post(c->empty);
        
        if (find_prime(c->primes, c->primes_dim, n))
        { 
            sum += n;
            received += 1;
        }
    }
    c->sum = sum;
    c->received = received;
    printf("[C] Consumer %d has finished\n with sum %ld\n", gettid() % 100, c->sum);
    pthread_exit(NULL);
}

int main(int argc, char **argv)
{
    if (argc != 3)
    {
        fprintf(stderr, "Usage:\t%s <prime_file> <integers_file>", argv[0]);
        exit(EXIT_FAILURE);
    }
    
    /* Make the array of primes */
    FILE *fp = fopen(argv[1], "rb");
    if (fp == NULL)
    {
        perror("Error opening file");
        exit(1);
    }

    /* Go to end of file */
    fseek(fp, 0, SEEK_END);
    long file_size = ftell(fp);
    rewind(fp);
    /* How many integers? */
    size_t count = file_size / sizeof(int);
    int *a = malloc(count * sizeof(int));
    if (a == NULL)
    {
        perror("Memory error");
        exit(2);
    }
    size_t e = fread(a, sizeof(int), count, fp);
    if (e != count)
    {
        perror("Error fread");
    }

    fclose(fp);

    /* Buffer */
    int buff[BUF_SIZE];
    int cindex = 0;
    int pindex = 0;
    long result = 0;
    int tot_primes = 0;

    /* Thread stuff */
    pthread_t thread_arr[CONS_THREAD];
    pthread_mutex_t mu = PTHREAD_MUTEX_INITIALIZER;
    sem_t full;
    sem_t empty;
    sem_init(&full, 0, 0);
    sem_init(&empty, 0, BUF_SIZE);

    /* Array of consumer structs */
    consumer_t consumer_arr[CONS_THREAD];
    for (int i = 0; i < CONS_THREAD; i++)
    {
        consumer_arr[i].primes = a;
        consumer_arr[i].primes_dim = count;
        consumer_arr[i].cindex = &cindex;
        consumer_arr[i].buffer = buff;
        consumer_arr[i].empty = &empty;
        consumer_arr[i].full = &full;
        consumer_arr[i].sum = result;
        consumer_arr[i].mu = &mu;
        consumer_arr[i].received = tot_primes;
        pthread_create(&thread_arr[i], NULL, cons_thread, consumer_arr + i);
    }

    /* Working on argv[2] - argv[2] is not a binary file */
    int n;
    FILE *fp2 = fopen(argv[2], "r");
    if (fp2 == NULL)
    {
        perror("Error opening file [argv2]");
        exit(3);
    }
    /* Reading integers from the file */
    while (true)
    {
        int e = fscanf(fp2, "%d", &n);
        if (e != 1) break;
        assert(n > 0);
        sem_wait(&empty);
        buff[pindex++ % BUF_SIZE] = n;
        sem_post(&full);
    }
    printf("[P] Data has been written\n");
    fclose(fp2);
 

    /* Inserting the stop value into the buffer */
    for (int i = 0; i < CONS_THREAD; i++)
    {
        sem_wait(&empty);
        buff[pindex++ % BUF_SIZE] = -1;
        sem_post(&full);
    }

    /* Join the threads */
    for (int i = 0; i < CONS_THREAD; i++)
    {
        if (pthread_join(thread_arr[i], NULL) != 0)
        {
            perror("Error joining threads");
            exit(3);
        }
    }

    /* Calculate the sum */
    for (int i = 0; i < CONS_THREAD; i++)
    {
        tot_primes += consumer_arr[i].received;
        result += consumer_arr[i].sum;
    }

    printf("Primes encountered: %d\nSum: %ld\n", tot_primes, result);


    sem_destroy(&full);
    sem_destroy(&empty);
    pthread_mutex_destroy(&mu);

    printf("Total sum: %ld\n", result);
    return 0;
}
