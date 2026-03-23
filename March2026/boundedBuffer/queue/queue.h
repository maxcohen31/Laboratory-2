#ifndef QUEUE_H_
#define QUEUE_H_

#include <pthread.h>

/** Nodo della lista concatenata usata per implementare la coda.
 *
 *  Il campo data contiene il puntatore al dato utente.
 *  La coda non copia ne' libera il dato: memorizza soltanto il puntatore.
 */
typedef struct Node {
    void        *data;
    struct Node *next;
} Node_t;

/** Coda FIFO concorrente protetta da mutex.
 *
 *  Implementazione con nodo sentinella:
 *  - head punta sempre a un nodo sentinella
 *  - se la coda e' vuota, head == tail
 *  - se la coda non e' vuota, il primo elemento reale e' head->next
 *
 *  Il mutex qlock protegge tutti i campi della struttura.
 *  La condition variable qcond e' usata dai thread che fanno pop()
 *  per sospendersi quando la coda e' vuota.
 */
typedef struct Queue {
    Node_t         *head;    // nodo sentinella in testa
    Node_t         *tail;    // ultimo nodo reale, oppure head se la coda e' vuota
    unsigned long   qlen;    // numero di elementi reali presenti nella coda
    pthread_mutex_t qlock;   // protegge head, tail e qlen
    pthread_cond_t  qcond;   // usata per attendere la condizione "coda non vuota"
} Queue_t;

/** Alloca e inizializza una coda vuota.
 *
 *  Deve essere chiamata da un solo thread, tipicamente il main,
 *  prima che la coda venga condivisa con altri thread.
 *
 *  \retval NULL se si verificano errori di allocazione o inizializzazione
 *          (errno viene impostata opportunamente quando possibile)
 *  \retval puntatore alla coda inizializzata in caso di successo
 */
Queue_t *initQueue(void);

/** Distrugge una coda allocata con initQueue().
 *
 *  Precondizione importante:
 *  nessun altro thread deve stare usando la coda quando questa funzione
 *  viene chiamata. In particolare, non devono esserci thread bloccati
 *  in pop(), ne' thread che eseguono push(), top() o length().
 *
 *  La funzione libera i nodi interni della struttura, ma NON libera
 *  i dati utente puntati dai campi data.
 *
 *  \param q puntatore alla coda da distruggere
 */
void deleteQueue(Queue_t *q);

/** Inserisce un dato in fondo alla coda.
 *
 *  Il puntatore data deve essere diverso da NULL. Questa scelta rende
 *  non ambiguo il valore di ritorno di pop(), che usa NULL per segnalare
 *  errore se la coda passata e' NULL.
 *  
 *  \param q    puntatore alla coda
 *  \param data puntatore al dato da inserire
 *
 *  \retval 0  se successo
 *  \retval -1 se errore, con errno impostata opportunamente
 */
int push(Queue_t *q, void *data);

/** Estrae e ritorna l'elemento in testa alla coda.
 *
 *  Se la coda e' vuota, il thread chiamante si sospende sulla
 *  condition variable finche' un altro thread non inserisce almeno
 *  un elemento.
 *
 *  La wait e' racchiusa in un ciclo while perche', al risveglio,
 *  la condizione deve sempre essere ricontrollata.
 *
 *  \param q puntatore alla coda
 *
 *  \retval puntatore al dato estratto
 *  \retval NULL se q == NULL
 */
void *pop(Queue_t *q);

/** Ritorna il dato in testa alla coda senza estrarlo.
 *
 *  Questa operazione NON blocca. Se la coda e' vuota ritorna subito NULL.
 *
 *  \param q puntatore alla coda
 *
 *  \retval puntatore al dato in testa
 *  \retval NULL se la coda e' vuota oppure se q == NULL
 */
void *top(Queue_t *q);

/** Ritorna la lunghezza corrente della coda.
 *
 *  Il valore e' letto in mutua esclusione, quindi e' consistente nel momento
 *  in cui viene osservato. Tuttavia puo' diventare obsoleto subito dopo il
 *  rilascio della lock, se altri thread modificano la coda.
 *
 *  \param q puntatore alla coda
 */
unsigned long length(Queue_t *q);

#endif /* QUEUE_H_ */
