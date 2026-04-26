/*
 * Esempio di implementazione di uno stack lock-free utilizzando
 * una variabile atomica (il puntatore all'inzio dello stack)
 * e l'operazione atomic_compare_exchange_weak()
 * 
 * Se viene definita la costante USE_MUTEX il sorgente 
 * gestisce il puntatore all'inizio dello stack utilizzando 
 * un mutex tradizionali.
 * 
 * il comando make crea gli eseguibili per entrambe le versioni
 * 
 */

#include <sched.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <stdatomic.h>
#include <pthread.h>
#include <sys/times.h>


/* Nodo dello stack */
typedef struct Node
{
    int value;
    struct Node *next;
} Node;

/* Struttura che rappresenta la cima dello stack
 * utilizziamo un puntatore atomico perché le operazioni
 * devono agire su di esso in modo atomico */
#ifndef USE_MUTEX
typedef struct
{
    Node *_Atomic head;
} LockFreeStack;
#else
typedef struct
{
    Node* head;
    pthread_mutex_t *mu;
} LockFreeStack;
#endif

/* Struttura di input per i singoli thread */
typedef struct 
{
    LockFreeStack stack;
    atomic_int pop_count;
    atomic_long pop_sum;
    int num_push_threads;
    int num_pop_threads;
    int num_values_per_push;
    atomic_int push_id;
} SharedData;


#ifndef USE_MUTEX
/* Push: inserisce atomicamente un nuovo nodo in testa allo stack */
void push(LockFreeStack *stack, int value)
{
    Node *new_node = malloc(sizeof(*new_node));
    if (new_node == NULL)
    {
        perror("malloc");
        exit(EXIT_FAILURE);
    }
    new_node->value = value;

    Node *old_head = atomic_load(&stack->head); /* legge l'attuale cima dello stack */
    do
    {
        new_node->next = old_head; /* Aggiorna il puntatore next per il nuovo nodo */
    } while (!atomic_compare_exchange_weak(&stack->head, &old_head, new_node));
}

/* Pop: rimuove atomicamente l'elemento in testa allo stack 
 * Restituisce 1 se l'operazione ha avuto successo; 0 altrimenti */
int pop(LockFreeStack *stack, int *value)
{
    Node *old_head;
    /* Il ciclo riprova se un altro thread ha modificato head tra load e swap */
    do 
    {
        old_head = atomic_load(&stack->head);
        if (old_head == NULL) return 0; /* Stack vuoto */
    } while (!atomic_compare_exchange_weak(&stack->head, &old_head, old_head->next));

    *value = old_head->value;
    free(old_head);
    return 1;
}
#else
/* Pop e push basate sui mutex */
void push(LockFreeStack *stack, int value)
{
    Node *new_node = malloc(sizeof(new_node));
    if (!new_node)
    {
        perror("malloc");
        exit(EXIT_FAILURE);
    }
    new_node->value = value;
    pthread_mutex_lock(stack->mu);
    new_node->next = stack->head;
    stack->head = new_node;
    pthread_mutex_unlock(stack->mu);
}

int pop(LockFreeStack *stack, int *value)
{
    pthread_mutex_lock(stack->mu);
    Node *old_head = stack->head;
    if (old_head == NULL) 
    {
        pthread_mutex_unlock(stack->mu);
        return 0; /* Stack vuoto */
    }
    stack->head = old_head->next;
    pthread_mutex_unlock(stack->mu);
    *value = old_head->value;
    free(old_head);
    return 1;
}
#endif

/* ================== Parte comune ad atomic e mutex =================== */
void *push_thread(void *args)
{
    SharedData *a = (SharedData*)args;
    int thread_id = atomic_fetch_add(&a->push_id, 1);
    /* Ogni thread inserisce una serie di valori basati sul proprio ID */
    for (int i = 0; i < a->num_values_per_push; i++)
    {
        int value = thread_id * a->num_values_per_push + i;
        push(&a->stack, value);
    }
    pthread_exit(NULL);
}

void *pop_thread(void *args)
{
    SharedData *a = (SharedData*)args;
    while (1)
    {
        int value;
        if (pop(&a->stack, &value))
        {
            atomic_fetch_add(&a->pop_count, 1);
            atomic_fetch_add(&a->pop_sum, value);
        }
        else
        {
            int tot_pushed = a->num_push_threads * a->num_values_per_push;
            int tot_popped = atomic_load(&a->pop_count);
            if (tot_popped >= tot_pushed) break;
        }
        sched_yield(); /* Se lo stack è vuoto rilascia la CPU */
    }
    pthread_exit(NULL);
}

int main(int argc, char **argv)
{
    if (argc != 4)
    {
        fprintf(stderr, "Uso:\t%s <num_push_threads> <num_pop_threads> <num_values_per_push>\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    
    int num_push_threads = atoi(argv[1]);
    int num_pop_threads = atoi(argv[2]);
    int num_value_per_push = atoi(argv[3]);

    /* Inizio misurazione a runtime */
    clock_t start = times(NULL);

    /* Inizializzazione della struttura condivisa */
    SharedData data = {
        .stack = { .head = NULL },
        .pop_count = ATOMIC_VAR_INIT(0),
        .pop_sum = ATOMIC_VAR_INIT(0),
        .num_push_threads = num_push_threads,
        .num_pop_threads = num_pop_threads,
        .num_values_per_push = num_value_per_push,
        .push_id = ATOMIC_VAR_INIT(0)
    };
#ifdef USE_MUTEX 
    pthread_mutex_t mu = PTHREAD_MUTEX_INITIALIZER;
    data.stack.mu = &mu;
#endif

    pthread_t push_threads[data.num_push_threads];
    pthread_t pop_threads[data.num_pop_threads];

    /* Creazione thread produttore */
    for (int i = 0; i < data.num_push_threads; i++)
    {
        if (pthread_create(&push_threads[i], NULL, push_thread, &data) != 0)
        {
            perror("pthread_create");
            exit(EXIT_FAILURE);
        }
    }

    /* Aspettiamo i thread produttori che abbiano finito */
    for (int i = 0; i < data.num_push_threads; i++) pthread_join(push_threads[i], NULL);

    /* Creazione thread consumatore */
    for (int i = 0; i < data.num_pop_threads; i++)
    {
        if (pthread_create(&pop_threads[i], NULL, pop_thread, &data) != 0)
        {
            perror("pthread_create");
            exit(EXIT_FAILURE);
        }
    }

    /* Aspettiamo che i thread abbiano finito */
    for (int i = 0; i < data.num_pop_threads; i++) pthread_join(pop_threads[i], NULL);

    clock_t end = times(NULL);

    /* Verifico il numero di push e pop eseguiti */
    int tot = atomic_load(&data.pop_count);
    printf("Totale dei valori pushati: %d\n", data.num_push_threads * data.num_values_per_push);
    printf("Totale dei pop: %d\n", tot);

    /* Verifica che lo stack sia vuoto */
    if (data.stack.head == NULL) printf("Stack vuoto\n");
    else printf("Lo stack non è vuoto\n");

    /* Verifica che sono stati letti i valori correnti */
    long sum = atomic_load(&data.pop_sum);
    printf("Somma dei valori poppati: %ld\n", sum);
    long expected_sum = data.num_push_threads * data.num_values_per_push;
    printf("Somma attesa: %ld\n", (expected_sum * (expected_sum - 1)) / 2);

    /* Termina mostrando il tempo di esecuzione */
    double elapsed_time = (double)(end - start) / sysconf(_SC_CLK_TCK);
    printf("Tempo passato: %.3f millisecondi\n", elapsed_time);

    return 0;
}
