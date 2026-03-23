#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <errno.h>
#include <pthread.h>

#include "queue.h"

/**
 * @file queue.c
 * @brief Implementazione di una coda FIFO concorrente con mutex + condition variable.
 *
 * La coda usa un nodo sentinella in testa. Questa scelta semplifica push() e pop():
 * - se la coda e' vuota, head e tail coincidono sul nodo sentinella
 * - il primo elemento reale, se esiste, e' sempre head->next
 */

/* ------------------- funzioni di utilita' -------------------- */

/* Per semplicita' didattica assumiamo che le primitive pthread, una volta
 * inizializzate correttamente, non falliscano durante il normale utilizzo.
 * In particolare non gestiamo esplicitamente i valori di ritorno di lock,
 * unlock, wait e signal.
 */
static inline Node_t *allocNode(void)   { return malloc(sizeof(Node_t)); }
static inline Queue_t *allocQueue(void) { return malloc(sizeof(Queue_t)); }
static inline void freeNode(Node_t *n)  { free(n); }

/* Acquisisce e rilascia la lock che protegge l'intera struttura dati. */
static inline void LockQueue(Queue_t *q)   { pthread_mutex_lock(&q->qlock); }
static inline void UnlockQueue(Queue_t *q) { pthread_mutex_unlock(&q->qlock); }

/* pthread_cond_wait() rilascia atomicamente la lock e sospende il thread.
 * Quando il thread viene risvegliato, la funzione ritorna solo dopo avere
 * riacquisito la lock.
 */
static inline void WaitOnQueue(Queue_t *q) {
    pthread_cond_wait(&q->qcond, &q->qlock);
}

/* Segnala che la coda non e' piu' vuota e poi rilascia la lock.
 * Le due operazioni sono insieme per comodita', perche' questo 
 * pattern compare spesso in push().
 */
static inline void SignalQueueAndUnlock(Queue_t *q) {
    pthread_cond_signal(&q->qcond);
    pthread_mutex_unlock(&q->qlock);
}

/* ------------------- interfaccia della coda ------------------ */

Queue_t *initQueue(void) {
    Queue_t *q = allocQueue();
    if (!q) return NULL;

    /* Creiamo la coda inizialmente vuota con un nodo sentinella. */
    q->head = allocNode();
    if (!q->head) {
        free(q);
        return NULL;
    }

    q->head->data = NULL;
    q->head->next = NULL;
    q->tail = q->head;
    q->qlen = 0;

    if (pthread_mutex_init(&q->qlock, NULL) != 0) {
        perror("pthread_mutex_init");
        freeNode(q->head);
        free(q);
        return NULL;
    }

    if (pthread_cond_init(&q->qcond, NULL) != 0) {
        perror("pthread_cond_init");
        pthread_mutex_destroy(&q->qlock);
        freeNode(q->head);
        free(q);
        return NULL;
    }

    return q;
}

void deleteQueue(Queue_t *q) {
    if (!q) return;

    /* Precondizione: nessun altro thread sta usando la coda.
     * Per questo motivo qui non prendiamo la lock.
     */
    while (q->head != q->tail) {
        Node_t *p = q->head;
        q->head = q->head->next;
        freeNode(p);
    }

    /* Libera anche l'ultimo nodo, che in questo momento coincide con il
     * nodo sentinella finale.
     */
    freeNode(q->head);

    pthread_mutex_destroy(&q->qlock);
    pthread_cond_destroy(&q->qcond);
    free(q);
}

int push(Queue_t *q, void *data) {
    if (q == NULL || data == NULL) {
        errno = EINVAL;
        return -1;
    }

    /* Allochiamo il nuovo nodo fuori dalla sezione critica,
     * cosi' la lock resta acquisita per meno tempo.
     */
    Node_t *n = allocNode();
    if (!n) return -1;

    n->data = data;
    n->next = NULL;

    LockQueue(q);

    /* Inserimento in fondo alla coda. tail punta sempre all'ultimo nodo reale,
     * oppure al sentinella se la coda era vuota.
     */
    q->tail->next = n;
    q->tail = n;
    q->qlen += 1;

    /* Dopo l'inserimento la condizione "coda non vuota" e' vera.
     * Se qualche consumatore era bloccato in pop(), possiamo risvegliarne uno.
     */
    SignalQueueAndUnlock(q);
    return 0;
}

void *pop(Queue_t *q) {
    if (q == NULL) {
        errno = EINVAL;
        return NULL;
    }

    LockQueue(q);

    /* Se la coda e' vuota il thread si sospende.
     * Usiamo un ciclo while, non un if, perche' al risveglio la condizione
     * deve sempre essere ricontrollata.
     */
    while (q->head == q->tail) {
        WaitOnQueue(q);
    }

    /* Invariante: se la coda non e' vuota, il primo dato reale e' in head->next. */
    assert(q->head->next != NULL);

    Node_t *old_head = q->head;         /* vecchio nodo sentinella */
    Node_t *first = q->head->next;      /* primo nodo reale */
    void *data = first->data;

    /* Facciamo avanzare head al nodo reale appena estratto.
     * Dopo l'operazione, quel nodo diventera' il nuovo sentinella.
     */
    q->head = first;
    q->qlen -= 1;

    /* Se dopo l'estrazione qlen diventa 0, allora head e tail coincidono.
     * Questo schema evita casi speciali nel mantenimento della coda.
     */
    assert((q->qlen == 0) == (q->head == q->tail));

    UnlockQueue(q);

    /* Il vecchio sentinella non serve piu'.
     * Lo liberiamo fuori dalla sezione critica.
     */
    freeNode(old_head);
    return data;
}

void *top(Queue_t *q) {
    if (q == NULL) {
        errno = EINVAL;
        return NULL;
    }

    LockQueue(q);

    /* top() non blocca: se la coda e' vuota ritorna subito NULL. */
    if (q->head == q->tail) {
        UnlockQueue(q);
        return NULL;
    }

    assert(q->head->next != NULL);
    void *data = q->head->next->data;
    UnlockQueue(q);
    return data;
}

unsigned long length(Queue_t *q) {
    if (q == NULL) {
        errno = EINVAL;
        return 0;
    }

    /* Leggiamo qlen in mutua esclusione per ottenere una fotografia coerente
     * dello stato della coda in questo istante.
     */
    LockQueue(q);
    unsigned long len = q->qlen;
    UnlockQueue(q);
    return len;
}
