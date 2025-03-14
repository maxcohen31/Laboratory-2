/*
    Problema PRODUTTORE - CONSUMATORE
	Si consideri un array circolare di caratteri, di dimensione LUN = 20.
	Definiamo 3 thread che operano concorrentemente sull'array:
	- il thread 1 (produttore) esegue per LUN volte le seguenti operazioni:
		a) acquisisce la mutua esclusione sull'array;
		b) Se l'array non è pieno, aggiunge il carattere '-';
		c) rilascia la mutua esclusione;
	- il thread 2 (produttore) esegue per LUN volte le seguenti operazioni:
		a) acquisisce la mutua esclusione sull'array;
		b) Se l'array non è pieno, aggiunge il carattere '+';
		c) rilascia la mutua esclusione;
	- il thread 3 (consumatore) esegue per 2*LUN volte le seguenti operazioni:		
		a) acquisisce la mutua esclusione sull'array;
		b) Se l'array non è vuoto, estrae un carattere (inserendolo in un buffer locale);
		c) una volta letti tutti i 2*LUN caratteri, li stampa;
		d) rilascia la mutua esclusione.
*/

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <pthread.h>
#include <semaphore.h>


#define ARRAY_SIZE 20
#define THREAD_NUM 3

char arr[ARRAY_SIZE];
int head = 0; // Indice di scrittura
int tail = 0; // Indice di lettura

// dati del produttore
typedef struct 
{
    pthread_mutex_t *pmutex;
    sem_t *spazioDisponibile;
    sem_t *datiDaLeggere;
    char charProd;
    int *count;
} pdati;

// dati del consumatore
typedef struct
{
    char *localBuffer;
    int bufferIndex;
    int *count;
    pthread_mutex_t *pmutex;
    sem_t *spazioDisponibile;
    sem_t *datiDaLeggere;
} cdati;

void *produttore(void *args)
{
    pdati *pd = (pdati*)args;
    for (int i = 0; i < ARRAY_SIZE; ++i)
    {
        sem_wait(pd->spazioDisponibile);
        pthread_mutex_lock(pd->pmutex);
        if (*(pd->count) < ARRAY_SIZE)
        {
            arr[head] = pd->charProd;
            head = (head + 1) % ARRAY_SIZE;
            (*(pd->count))++;
        }
        pthread_mutex_unlock(pd->pmutex);
        sem_post(pd->datiDaLeggere);
    }
    pthread_exit(NULL);
}

void *consumatore(void *args)
{
    cdati *cd = (cdati*)args;
    for (int j = 0; j < 2*ARRAY_SIZE; ++j)
    {
        sem_wait(cd->datiDaLeggere); 
        pthread_mutex_lock(cd->pmutex);
        if (*(cd->count) > 0)
        {
            cd->localBuffer[cd->bufferIndex] = arr[tail];
            tail = (tail + 1) % ARRAY_SIZE;
            cd->bufferIndex = ((cd->bufferIndex + 1) % (2 * ARRAY_SIZE));
            (*(cd->count))--;
        }
        pthread_mutex_unlock(cd->pmutex);
        sem_post(cd->spazioDisponibile);
    }

    // Stampa i caratteri dopo aver rilasciato il mutex
    for (int k = 0; k < 2*ARRAY_SIZE; ++k)
    {
        printf("carattere nell'array: %c\n", cd->localBuffer[k]);
    }

    pthread_exit(NULL);
}

int main(int argc, char **argv)
{
    pthread_t th[THREAD_NUM];
    pthread_mutex_t mu = PTHREAD_MUTEX_INITIALIZER;
    int count = 0; // contatore buffer
    sem_t spazioDisponibile;
    sem_t datiDaLeggere;
    sem_init(&spazioDisponibile, 0, ARRAY_SIZE); // buffer vuoto all'inizio
    sem_init(&datiDaLeggere, 0, 0); // buffer vuoto - nessun elemento da leggere

    // crea struct produttore e struct consumatore
    pdati prod1;
    prod1.pmutex = &mu;
    prod1.spazioDisponibile = &spazioDisponibile;
    prod1.datiDaLeggere = &datiDaLeggere;
    prod1.charProd = '+';
    prod1.count = &count; 
    
    pdati prod2;
    prod2.pmutex = &mu;
    prod2.spazioDisponibile = &spazioDisponibile;
    prod2.datiDaLeggere = &datiDaLeggere;
    prod2.charProd = '-';
    prod2.count = &count; 

    // consumatore
    cdati cons;
    cons.spazioDisponibile = &spazioDisponibile;
    cons.datiDaLeggere = &datiDaLeggere;
    cons.pmutex = &mu;
    cons.localBuffer = malloc(2* ARRAY_SIZE * sizeof(char));
    cons.bufferIndex = 0;
    cons.count = &count;

    pthread_create(&th[0], NULL, produttore, &prod1);
    pthread_create(&th[1], NULL, produttore, &prod2);
    pthread_create(&th[2], NULL, consumatore, &cons);

    // Attendere la terminazione dei thread
    for (int i = 0; i < THREAD_NUM; ++i)
    {
        pthread_join(th[i], NULL);
    }
    
    sem_destroy(&spazioDisponibile);
    sem_destroy(&datiDaLeggere);
    pthread_mutex_destroy(&mu);
    free(cons.localBuffer);

    return 0;
}
