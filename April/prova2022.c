/*
    Scrivere un programma gcdT che invocato dalla linea di comando scrivendo

    gcdT infile numt

    calcola il massimo comun divisore delle coppie di valori in infile utilizzando numt thread ausiliari secondo il seguente procedimento.

    I thread ausiliari svolgono il ruolo di produttori e devono leggere coppie di interi dal file di testo infile assicurandosi 
    che ogni coppia di infile venga letta da un unico thread.

    I thread ausiliari devono passare le coppie al thread principale utilizzando il meccanismo produttore/consumatore mediante 
    un buffer la cui dimensione deve essere una costante definita con #define. 
    Il thread principale svolge il ruolo di unico consumatore; 
    legge le coppie dal buffer e per ogni coppia calcola il massimo comun divisore con la funzione gcd che trovate nel sorgente C.

    Il thread principale deve salvare i valori dei massimo comun divisore in un array di int e quando tutti i produttori 
    hanno terminato deve ordinare questo array (con qsort) e stampare su stdout i valori ordinati (uno per riga). 
    Ogni altro messaggio del programma deve essere inviato su stderr.

    Si noti che non si sa in anticipo quanti gcd saranno calcolati quindi l'array dei gcd deve essere gestito in maniera dinamica.

    Suggerimenti

        - fa parte dell'esercizio scrivere il makefile che ottiene l'eseguibile compilando separatamente xerrori.c e gcdtT.c

        - Utilizzare un mutex per assicurarsi che i produttori leggano dal file in maniera esclusiva

        - Ogni dato passato da produttori a consumatore consiste di due interi che devono essere processati insieme. 
          Se il buffer consiste in un array di interi è necessaria un modifica allo schema che abbiamo sempre usato, 
          altrimenti come buffer potete usare un array di struct, dove ogni signola struct contiene la coppia di interi

        - Anche in questo caso abbiamo il problema della terminazione: 
          i produttori prima di terminare devono in qualche modo segnalare al consumatore che non ci sono altri dati.

        - L'esercizio ha solo scopo didattico: non è da considerare una soluzione modello per il problema.

*/

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <pthread.h>
#include <assert.h>


#define BUFF_SIZE 20


typedef struct 
{
    int x;
    int y;
} coppia;

/*
    VERSIONE PRELIMINARE: MI BASTA CHE FUNZIONE PER ADESSO 
    Primavera 2025
*/

coppia buffer[BUFF_SIZE]; // buffer condiviso dai thread
int in = 0; // elementi entranti
int out = 0; // elementi uscenti
int total = 0; // elementi totali
int threadDone = 0; // segnala che il thread ha finito
int threadNumber; // numero thread
int size = 0; // dimensione dell'array finale
int *result = NULL; // array contenente tutti i gcd delle coppie
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t notEmptyBuf = PTHREAD_COND_INITIALIZER;
pthread_cond_t notFullBuf = PTHREAD_COND_INITIALIZER;

int gcd(int a, int b)
{
    assert(a >= 0 && b >= 0);
    assert(a != 0 || b != 0);
    if (b == 0)
    {
        return a;
    }
    return gcd(b, a%b);
}

int cmp(const void *a, const void *b)
{
    return (*(int*)a) - (*(int*)b);
}

void *producer(void *args)
{
    // apre il file e legge le coppie
    FILE *f = (FILE*)args;
    int a;
    int b;

    while (true)
    {
        pthread_mutex_lock(&mutex);
        // se non ci sono coppie di interi esco
        if (fscanf(f, "%d %d", &a, &b) != 2)
        {
            pthread_mutex_unlock(&mutex);
            break;
        }
        // se il buffer è pieno aspetto
        while (total == BUFF_SIZE)
        {
            pthread_cond_wait(&notFullBuf, &mutex);
        }
        // scrivo coppia
        buffer[in].x = a;
        buffer[in].y = b;
        in = (in + 1) % BUFF_SIZE;
        total++;
        // sveglio thread e rilascio mutua esclusione
        pthread_cond_signal(&notEmptyBuf);
        pthread_mutex_unlock(&mutex);
    }
    // segnalo al thread che ho finito
    pthread_mutex_lock(&mutex);
    threadDone++;
    pthread_cond_signal(&notEmptyBuf);
    pthread_mutex_unlock(&mutex);

    return NULL;
}

void *consumer(void *args)
{
    while (true)
    {
        pthread_mutex_lock(&mutex);
        // controllo che il buffer non sia vuoto e che il thread abbia finito
        while (total == 0 && threadDone < threadNumber)
        {
            pthread_cond_wait(&notEmptyBuf, &mutex);
        }
        // se il buffer è vuoto e il numero dei thread è uguale ai thread che hanno finito usciamo
        if (total == 0 && threadNumber == threadDone)
        {
            pthread_mutex_unlock(&mutex);
            break;
        }
        // estraggo coppia
        coppia c = buffer[out];
        out = (out + 1) % BUFF_SIZE;
        total--;
        pthread_cond_signal(&notFullBuf);
        pthread_mutex_unlock(&mutex);

        // calcolo gcd e aggiorna l'array result
        int r = gcd(c.x, c.y);
        pthread_mutex_lock(&mutex);
        result = realloc(result, (size + 1) * sizeof(int));
        if (result == NULL)
        {
            fprintf(stderr, "Errore realloc\n");
            exit(EXIT_FAILURE);
        }
        result[size] = r;
        size++;
        pthread_mutex_unlock(&mutex);
    }

    return NULL;
}


int main(int argc, char **argv)
{
    if (argc != 3) 
    {
      printf("Uso: %s file numT\n",argv[0]);
      return 1;
    }

    // apro file 
    FILE *fl = fopen(argv[1], "r");
    if (fl == NULL)
    {
        fprintf(stderr, "Errore apertura file\n");
        exit(EXIT_FAILURE);
    }

    threadNumber = atoi(argv[2]); // thread da creare
    pthread_t arr[threadNumber]; 
    pthread_t threadCons; // thread consumatore

    for (int i = 0; i < threadNumber; ++i)
    {
        if (pthread_create(&arr[i], NULL, &producer, fl))
        {
            perror("Errore creazione thread\n");
        }
    }

    if (pthread_create(&threadCons, NULL, &consumer, NULL) != 0)
    {
        perror("Errore creazione thread consumatore\n");
        exit(1);
    }

    for (int i = 0; i < threadNumber; ++i)
    {
        if (pthread_join(arr[i], NULL) != 0)
        {
            perror("Errore nella join\n");
            exit(2);
        }
    }

    pthread_join(threadCons, NULL);

    fclose(fl);

    // sorting del risultato
    qsort(result, size, sizeof(int), &cmp);
    for (int i = 0; i < size; ++i)
    {
        printf("[%d] gcd: %d\n", i, result[i]);
    }

    free(result);
    return 0;
}
