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
// indice scrittura
int head = 0; 
// indice lettura
int tail = 0; 


// dati del produttore
typedef struct 
{
    pthread_mutex_t *pmutex;
    sem_t *spazioDisponibile;
    sem_t *datiDaLeggere;
    char charProd;
} pdati;

// dati del consumatore
typedef struct
{
    char *localBuffer;
    int bufferIndex;
    pthread_mutex_t *pmutex;
    sem_t *spazioDisponibile;
    sem_t *datiDaLeggere;
} cdati;

void *produttore(void *args)
{
    pdati *pd = (pdati*)args;
    for (int i = 0; i < ARRAY_SIZE; ++i)
    {
        // aspetto che ci sia spazio disponibile nel buffer
        sem_wait(pd->spazioDisponibile);
        // acquisisco mutua esclusione
        pthread_mutex_lock(pd->pmutex);
        {
            arr[head] = pd->charProd;
            // aumento l'indice scrittura
            head = (head + 1) % ARRAY_SIZE;
        }
        // rilascio la mutua esclusione
        pthread_mutex_unlock(pd->pmutex);
        // segnalo che c'è un nuovo dato
        sem_post(pd->datiDaLeggere);
    }
    pthread_exit(NULL);
}

void *consumatore(void *args)
{
    cdati *cd = (cdati*)args;
    for (int j = 0; j < 2*ARRAY_SIZE; ++j)
    {
        // aspetto che ci sia almeno un dato da leggere
        sem_wait(cd->datiDaLeggere); 
        pthread_mutex_lock(cd->pmutex);
        {
            // consumo un carattere
            cd->localBuffer[cd->bufferIndex] = arr[tail];
            // l'indice di lettura viene incrementato
            tail = (tail + 1) % ARRAY_SIZE;
            // l'indice del buffer viene incrementato
            cd->bufferIndex = ((cd->bufferIndex + 1) % (2 * ARRAY_SIZE));
        }
        pthread_mutex_unlock(cd->pmutex);
        // segnalo che c'è spazio disponibile
        sem_post(cd->spazioDisponibile);
    }

    // stampa i caratteri dopo aver rilasciato il mutex
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
    sem_t spazioDisponibile;
    sem_t datiDaLeggere;
    sem_init(&spazioDisponibile, 0, ARRAY_SIZE); // buffer vuoto all'inizio
    sem_init(&datiDaLeggere, 0, 0); // buffer vuoto - nessun elemento da leggere

    // crea struct produttore e struct consumatore
    pdati prod1 = {&mu, &spazioDisponibile, &datiDaLeggere, '+'};
    pdati prod2 = {&mu, &spazioDisponibile, &datiDaLeggere, '-'}; 

    // consumatore
    cdati cons;
    cons.localBuffer = malloc(2* ARRAY_SIZE * sizeof(char));
    if (cons.localBuffer == NULL)
    {
        fprintf(stderr, "Malloc fallita");
        exit(1);
    }
    cons.pmutex = &mu;
    cons.datiDaLeggere = &datiDaLeggere;
    cons.spazioDisponibile = &spazioDisponibile;
    cons.bufferIndex = 0;

    // creazione dei thread
    pthread_create(&th[0], NULL, produttore, &prod1);
    pthread_create(&th[1], NULL, produttore, &prod2);
    pthread_create(&th[2], NULL, consumatore, &cons);

    // Attendere la terminazione dei thread
    for (int i = 0; i < THREAD_NUM; ++i)
    {
        pthread_join(th[i], NULL);
    }
    
    // libera risorse
    sem_destroy(&spazioDisponibile);
    sem_destroy(&datiDaLeggere);
    pthread_mutex_destroy(&mu);
    free(cons.localBuffer);

    return 0;
}
