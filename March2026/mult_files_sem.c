/*
 * Esempio paradigma produttori consumatori
 * con produttori e consumatori multipli
 * 
 * I produttori leggono interi dai file e i consumatori
 * calcolano il numero e la somma dei primi
 * 
 * il numero di produttori è uguale al numero di file di input
 * il numero di consumatori è stabilito all'inizio del main()   
 * Il risultato deve essere uguale a quello di mult_files_cond_var.c
 * */


#include "xerrori.h"

#define BUF_SIZE 10

typedef struct 
{
    long somma;
    int quanti;
    int *buffer;
    int *pcindex; /* cindex pointer */
    pthread_mutex_t *mu;
    sem_t *free_slots;
    sem_t *sem_data_items;
} consumer_data;

typedef struct 
{
    char *filename;
    int *buffer;
    int *ppindex; /* pindex pointer */
    pthread_mutex_t *mu;
    sem_t *free_slots;
    sem_t *sem_data_items;
} producer_data;


bool is_prime(int n)
{
    if (n < 2) return false;
    if (n % 2 == 0) return (n == 2);
    for (int i = 3; i*i <= n; i += 2)
    {
        if (n % i == 0) return false;
    }
    return true;
}

/* Consumer's function */
void *cbody(void *arg)
{
    consumer_data *c = (consumer_data*)arg;

    int n = 0;
    c->quanti = 0;
    c->somma = 0;
    fprintf(stderr, "[C] consumer started\n");

    while (n != -1)
    {
        xsem_wait(c->sem_data_items, __LINE__, __FILE__);
        xpthread_mutex_lock(c->mu, __LINE__, __FILE__);
        n = c->buffer[*(c->pcindex) % BUF_SIZE];
        (*(c->pcindex))++; 
        xpthread_mutex_unlock(c->mu, __LINE__, __FILE__);
        xsem_post(c->free_slots, __LINE__, __FILE__);
        
        if (is_prime(n) && n > 0)
        {
            c->quanti++;
            c->somma += n;
        }
    }
    pthread_exit(NULL);
}

/* Producer's function */
void *pbody(void *arg)
{
    producer_data *p = (producer_data*)arg;
    
    fprintf(stderr, "[P] producer started with file %s\n", p->filename);
    FILE *f = fopen(p->filename, "r");
    if (!f) 
    {
        perror("Error opening file\n");
        pthread_exit(NULL);
    }

    int n;
    while (fscanf(f, "%d", &n) == 1)
    {
        assert(n > 0); /* n must be positive */
        xsem_wait(p->free_slots, __LINE__, __FILE__);
        xpthread_mutex_lock(p->mu, __LINE__, __FILE__);
        p->buffer[(*p->ppindex) % BUF_SIZE] = n;
        (*p->ppindex) += 1;
        xpthread_mutex_unlock(p->mu, __LINE__, __FILE__);
        xsem_post(p->sem_data_items, __LINE__, __FILE__);
    }
    fclose(f); 
    pthread_exit(NULL);
}


int main(int argc, char **argv)
{
    if (argc < 2)
    {
        fprintf(stderr, "Usage:\t%s file1 [file2 ... fileK]", argv[0]);
        exit(1);
    }
    
    int pindex = 0;
    int cindex = 0;
    int tot_primes = 0;
    long sum = 0;
    
    int num_cons = 5;
    assert(num_cons > 0);
    int num_prods = argc - 1;
    assert(num_prods > 0);
    int buffer[BUF_SIZE];
    
    pthread_mutex_t cmu = PTHREAD_MUTEX_INITIALIZER;
    pthread_mutex_t pmu = PTHREAD_MUTEX_INITIALIZER;
    sem_t free_slots;
    sem_t sem_data_items;
    xsem_init(&free_slots, 0, BUF_SIZE, __LINE__, __FILE__);
    xsem_init(&sem_data_items, 0, 0, __LINE__, __FILE__);

    producer_data thp_arr[num_prods];
    pthread_t tp[num_prods];
    for (int i = 0; i < num_prods; i++)
    {
        thp_arr[i].buffer = buffer;
        thp_arr[i].ppindex = &pindex;
        thp_arr[i].mu = &pmu;
        thp_arr[i].sem_data_items = &sem_data_items;
        thp_arr[i].free_slots = &free_slots;
        thp_arr[i].filename = argv[i+1];
        xpthread_create(&tp[i], NULL, pbody, &thp_arr[i], __LINE__, __FILE__);
    }

    pthread_t tc[num_cons];
    consumer_data th_arr[num_cons];
    for (int i = 0; i < num_cons; i++)
    {
        th_arr[i].buffer = buffer;
        th_arr[i].free_slots = &free_slots;
        th_arr[i].sem_data_items = &sem_data_items;
        th_arr[i].mu = &cmu;
        th_arr[i].pcindex = &cindex;
        xpthread_create(&tc[i], NULL, cbody, &th_arr[i], __LINE__,__FILE__);
    }
    fprintf(stderr, "Producer threads created\n");


    fprintf(stderr, "Consumer threads created\n");

    /* Producer threads join */
    for (int i = 0; i < num_prods; i++) xpthread_join(tp[i], NULL, __LINE__, __FILE__);

    /* Consumers end - for each consumer thread we write the termination value */
    for (int i = 0; i < num_cons; i++)
    {
        xsem_wait(&free_slots, __LINE__, __FILE__);
        buffer[pindex % BUF_SIZE] = -1;
        pindex++;
        xsem_post(&sem_data_items, __LINE__, __FILE__);
    }
    fprintf(stderr, "Termination values written\n");

    for (int i = 0; i < num_cons; i++)
    {
        xpthread_join(tc[i], NULL, __LINE__, __FILE__);
        tot_primes += th_arr[i].quanti;
        sum += th_arr[i].somma;
    }
    
    xpthread_mutex_destroy(&pmu, __LINE__, __FILE__);
    xpthread_mutex_destroy(&cmu, __LINE__, __FILE__);
    xsem_destroy(&free_slots, __LINE__, __FILE__);
    xsem_destroy(&sem_data_items, __LINE__, __FILE__);
    fprintf(stderr, "Found %d primes with sum %ld\n", tot_primes, sum);
    return 0;
}
