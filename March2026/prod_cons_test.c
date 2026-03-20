/*
 * Esempio paradigma produttore consumatori
 * in cui abbiamo più di un produttore e 
 * più di consumatore
 * 
 * Produttori e consumatori fanno operazioni inutili e veloci 
 * perché lo scopo è misurare l'overhead del paradigma
 * utilizzando semafori o condition variables.
 * 
 * */


#include "xerrori.h"
#include <assert.h>
#include <stdlib.h>
#include <sys/times.h>
#include <unistd.h>

#define BUF_SIZE 10
#define QUI __LINE__,__FILE__

#ifdef USACV
#warning "Usa condition variables"
#define METODO " (CV)"
#else
#define METODO " (SEM)"
#endif

/* Trova il più piccolo divisore maggiore di uno */
int divisore(int n)
{
    assert(n > 1);
    for (int i = 2; i*i <= n; i++)
    {
        if (n % i == 0) return i;
    }
    return n;
}

/* Struct per i thread consumatori */
typedef struct 
{
    int *buffer;
    int *pcindex;
#ifdef USACV
    int *pdati;
    pthread_mutex_t *mu;
    pthread_cond_t *empty;
    pthread_cond_t *full;
#else
    pthread_mutex_t *pmutex_buf;
    sem_t *sem_free_slots;
    sem_t *sem_data_items;
#endif
    long risultato;    
} dati_consumatori;

/* Struct per i thread produttori */
typedef struct
{
    int *buffer;
    int *ppindex;
#ifdef USACV
    int *pdati;
    pthread_mutex_t *mu;
    pthread_cond_t *empty;
    pthread_cond_t *full;
#else
    pthread_mutex_t *pmutex_buf;
    sem_t *sem_free_slots;
    sem_t *sem_data_items;
#endif
    int numeri_interi;    
} dati_produttori;

/* Funzione dei thread consumatori */
void *cbody(void *arg)
{
    dati_consumatori *a = (dati_consumatori*)arg;
    int n;
    do 
    {
#ifdef USACV
        xpthread_mutex_lock(a->mu, QUI);
        while (*(a->pdati) == 0) xpthread_cond_wait(a->empty, a->mu, QUI);
        (*(a->pdati))--;
#else
        xsem_wait(a->sem_data_items, QUI);
        xpthread_mutex_lock(a->pmutex_buf, QUI);
#endif
    /* Parte comune a CV e semafori: lettura dal buffer */
    n = a->buffer[*(a->pcindex) % BUF_SIZE];
    (*(a->pcindex))++;
#ifdef USACV
    /* Segnalo che il buffer non è più pieno */
    xpthread_cond_signal(a->full, QUI);
    xpthread_mutex_unlock(a->mu, QUI);
#else
    xpthread_mutex_unlock(a->pmutex_buf, QUI);
    xsem_post(a->sem_free_slots, QUI);
#endif
    if (n == -1) break;
    a->risultato += divisore(n);
    } while (true);

    pthread_exit(NULL);
}

/* Funzione dei thread produttori */
void *pbody(void *arg)
{
    dati_produttori *a = (dati_produttori*)arg;
    for (int i = 2; i <= a->numeri_interi; i++)
    { 
#ifdef USACV
        xpthread_mutex_lock(a->mu, QUI);
        while (*(a->pdati) == BUF_SIZE) xpthread_cond_wait(a->full, a->mu, QUI);
        (*(a->pdati))++;
#else
        xsem_wait(a->sem_data_items, QUI);
        xpthread_mutex_lock(a->pmutex_buf, QUI);
#endif
    /* Parte comune a CV e semafori: lettura dal buffer */
    a->buffer[*(a->ppindex) % BUF_SIZE] = i;
    (*(a->ppindex))++;
#ifdef USACV
    /* Segnalo che il buffer non è più pieno */
    xpthread_cond_signal(a->full, QUI);
    xpthread_mutex_unlock(a->mu, QUI);
#else
    xpthread_mutex_unlock(a->pmutex_buf, QUI);
    xsem_post(a->sem_free_slots, QUI);
#endif
    }
    pthread_exit(NULL);
}

int main(int argc, char **argv)
{
    if (argc != 4)
    {
        printf("Uso:\t%s interi #prod #cons\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    /* Numero di thread produttori e consumatori */
    int num = atoi(argv[1]);
    int tp = atoi(argv[2]);
    int tc = atoi(argv[3]);
    assert(tp > 0);
    assert(tc > 0);

    clock_t start = times(NULL);

    /* Buffer produttori consumatori */
    int buffer[BUF_SIZE];
    int pindex = 0;
    int cindex = 0;
#ifdef USACV
    int dati=0;
    pthread_mutex_t mu = PTHREAD_MUTEX_INITIALIZER;
    pthread_cond_t empty = PTHREAD_COND_INITIALIZER;
    pthread_cond_t full = PTHREAD_COND_INITIALIZER;
#else
    pthread_mutex_t mupbuf = PTHREAD_MUTEX_INITIALIZER;
    pthread_mutex_t mucbuf = PTHREAD_MUTEX_INITIALIZER;
    sem_t sem_free_slots, sem_data_items;
    xsem_init(&sem_free_slots,0,BUF_SIZE,QUI);
    xsem_init(&sem_data_items,0,0,QUI);
#endif
 
    /* Dati thread */
    dati_produttori ap[tp];
    dati_consumatori ac[tc];
    pthread_t prod[tp];       // id thread produttori
    pthread_t cons[tc];

    /* Crea produttori */
    for (int i = 0; i < tp; i++)
    {
        ap[i].ppindex = &pindex;
        ap[i].buffer = buffer;
#ifdef USACV
        ap[i].pdati = &dati;
        ap[i].mu = &mu;
        ap[i].empty = &empty;
        ap[i].full = &full;
#else
        ap[i].pmutex_buf = &mupbuf;
        ap[i].sem_data_items = &sem_data_items;
        ap[i].sem_free_slots = &sem_free_slots;
#endif
        ap[i].numeri_interi = num;
        xpthread_create(&prod[i], NULL, &pbody, &ap[i],QUI);
    }

    for (int i = 0; i < tc; i++)
    {
        ac[i].pcindex = &cindex;
        ac[i].buffer = buffer;
#ifdef USACV
        ac[i].pdati = &dati;
        ac[i].mu = &mu;
        ac[i].empty = &empty;
        ac[i].full = &full;
#else
        ac[i].pmutex_buf = &mupbuf;
        ac[i].sem_data_items = &sem_data_items;
        ac[i].sem_free_slots = &sem_free_slots;
#endif
        ac[i].risultato = 0;
        xpthread_create(&cons[i], NULL, &pbody, &ac[i],QUI);
    }

    /* Attendo i produttori */
    for (int i = 0; i < tp; i++) pthread_join(prod[i], NULL);

    /* Comunico ai consumatori che possono terminare */
    for (int i = 0; i < tc; i++)
    {
#ifdef USACV 
        while (dati == BUF_SIZE) xpthread_cond_wait(&full, &mu, QUI);
        buffer[pindex % BUF_SIZE] = -1;
        pindex++;
        dati++;
        xpthread_cond_signal(&empty, QUI);
        xpthread_mutex_unlock(&mu, QUI);
#else
        xsem_wait(&sem_free_slots, QUI);
        buffer[pindex % BUF_SIZE] = -1;
        pindex++;
        xsem_post(&sem_data_items, QUI);
#endif
    }

    /* Calcola il risultato */
    long r = 0;
    for (int i = 0; i < tc; i++)
    {
        pthread_join(cons[i], NULL);
        r += ac[i].risultato;
    }
    printf(METODO "Totale: %ld (%ld per produttore)\n", r, r/tp);
    /* Deallocazione */
#ifdef USACV
    xpthread_mutex_destroy(&mu,QUI);
    xpthread_cond_destroy(&empty,QUI);
    xpthread_cond_destroy(&full,QUI);
#else  
    xpthread_mutex_destroy(&mupbuf, QUI);
    xpthread_mutex_destroy(&mucbuf, QUI);
    xsem_destroy(&sem_free_slots, QUI);
    xsem_destroy(&sem_data_items, QUI);
#endif
    double tot_time = (double)(times(NULL)-start)/sysconf(_SC_CLK_TCK);
    printf(METODO "Elapsed time: %.3lf secs (%.3lf musec x intero)\n", tot_time, 100000*tot_time/(num*tp));
    return 0;
}
