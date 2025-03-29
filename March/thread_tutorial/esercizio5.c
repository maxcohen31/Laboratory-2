/*
    Esercizio 2

    Scrivere un programma C multithreading contaCaratteri.c che prende in ingresso un
    pathname nomefile, un carattere c e due interi from, to e conta quante
    volte compare il carattere c nel file nomefile nella porzione indicata dai
    due interi: a partire da from byte fino a to

*/


#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define BUFFER_SIZE 80
#define NUM_THREADS 3

typedef struct 
{
    char *pathname; // file path
    pthread_mutex_t *mu; // mutex
    char target; // carattere da cercare
    int start; // range start
    int end; // range end
    int *count; // contatore occorrenze del carattere
} thrd;

// Thread function
void *routine(void *args) 
{
    thrd *t = (thrd *)args;
    int localCount = 0; // contatore locale per questo thread
    FILE *source = fopen(t->pathname, "r");
    
    if (source == NULL) {
        perror("Errore apertura file nel thread");
        return NULL;
    }
    
    // posizionamento all'inizio del range assegnato
    fseek(source, t->start, SEEK_SET);
    
    // allocazione buffer locale
    char buffer[BUFFER_SIZE];
    size_t bytesRead;
    long currentPos = t->start;
    
    // lettura blocchi
    while (currentPos < t->end) {
        // calcola quanti byte leggere in questo ciclo
        int bytesToRead = BUFFER_SIZE;
        if (currentPos + bytesToRead > t->end) {
            bytesToRead = t->end - currentPos;
        }
        
        // leggi il blocco
        bytesRead = fread(buffer, 1, bytesToRead, source);
        if (bytesRead <= 0) 
        {
            break;
        }
        
        // conta occorrenze nel blocco
        for (int i = 0; i < bytesRead; i++) {
            if (buffer[i] == t->target) {
                localCount++;
            }
        }
        
        currentPos += bytesRead;
    }
    
    // aggiorna contatore
    pthread_mutex_lock(t->mu);
    *(t->count) += localCount;
    pthread_mutex_unlock(t->mu);
    
    fclose(source);
    return NULL;
}

int main(int argc, char **argv) {
    
    if (argc != 5) 
    {
        fprintf(stderr, "Usage: %s <file> <char> <start> <end>\n", argv[0]);
        return EXIT_FAILURE;
    }
    
    char *filename = argv[1];
    char target = argv[2][0];
    int startFile = atoi(argv[3]);
    int endFile = atoi(argv[4]);
    
    // verifica che il file esista e che le posizioni siano valide
    FILE *file = fopen(filename, "r");
    if (file == NULL) 
    {
        perror("Errore apertura file");
        exit(EXIT_FAILURE);
    }
    
    // verifica dimensione file
    fseek(file, 0, SEEK_END);
    long fileSize = ftell(file);
    fclose(file);
    
    if (startFile < 0 || endFile > fileSize || startFile >= endFile) 
    {
        fprintf(stderr, "Errore: range non valido\n");
        exit(EXIT_FAILURE);
    }
    
    int totalCount = 0;
    pthread_t threads[NUM_THREADS];
    thrd threadData[NUM_THREADS];
    pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
    
    // calcola la dimensione della porzione per ciascun thread
    int rangeSize = (endFile - startFile) / NUM_THREADS;
    
    for (int i = 0; i < NUM_THREADS; i++) {
        threadData[i].pathname = filename;
        threadData[i].mu = &mutex;
        threadData[i].target = target;
        threadData[i].count = &totalCount;
        
        // Assegna range per ogni thread
        threadData[i].start = startFile + (i * rangeSize);
        
        // se il range fosse 0-100, la divisione produrrebbe 33,33 byte
        // il thread 1 prende 0-33 byte e il thread 2 prende 33-66 byte
        // l'ultimo si prende il resto
        if (i == NUM_THREADS - 1) 
        {
            threadData[i].end = endFile;
        } 
        else 
        {
            threadData[i].end = startFile + ((i + 1) * rangeSize);
        }
        
        if (pthread_create(&threads[i], NULL, &routine, &threadData[i]) != 0) {
            perror("Errore creazione thread");
            return EXIT_FAILURE;
        }
    }
    
    // join dei thread
    for (int i = 0; i < NUM_THREADS; i++) 
    {
        pthread_join(threads[i], NULL);
    }
    
    printf("Trovate %d occorrenze del carattere '%c'\n", totalCount, target);
    
    pthread_mutex_destroy(&mutex);
    return 0;
}

