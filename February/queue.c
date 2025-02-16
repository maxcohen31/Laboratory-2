/*
    A simple queue implementation 
*/

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <stdbool.h>

// the block of the queue
// a queue is being built by a collection of these structures 
typedef struct _node 
{
    int value;
    struct _node *next;
} node;

// queue main structure
typedef struct _queue
{
    struct _node *front;
    struct _node *rear;
    int size;
} queue;


// The simplest queue is the empty one
queue *queueInit()
{
    // initialiazing the queue structure
    queue *q = malloc(sizeof(*q)); 
    if (q == NULL)
    {
        fprintf(stderr, "Error allocating memory!");
        exit(1);
    }

    q->front = NULL;
    q->rear = NULL;
    q->size = 0;

    return q;
}

void destroyQueue(queue *q)
{
    if (q == NULL) 
    {
        fprintf(stderr, "Error! Queue is NULL!");
        exit(2);
    }

    while (q->front != NULL)
    {
        // saving the first element 
        node *tmp = q->front;
        // looping over elements 
        q->front = q->front->next; 
        free(tmp);
    }

    q->rear = NULL;
    q->size = 0;
    free(q);
}

bool isEmpty(queue *q)
{
    return q->size == 0;
}

bool enqueue(queue *q, int v)
{
    // create a node
    node *n = malloc(sizeof(*n));
    if (n == NULL)
    {
        fprintf(stderr, "Memory allocation for given node gone wrong!");
        exit(3);
    }
    // initialize node's value and node's next
    n->value = v;
    n->next = NULL;

    // check if queue is empty
    // If it is then front and rear are the same
    if (isEmpty(q))
    {
        q->front = n;
        q->rear = n;
    }
    // otherwise rear becomes the new node 
    else 
    {
        q->rear->next = n;
        q->rear = n;
    }

    q->size++;
    return true;
}

int dequeue(queue *q)
{
    if (isEmpty(q))
    {
        fprintf(stderr, "Queue is empty. Cannot pop any element");
        exit(4);
    }

    // save the front
    // this element will be popped
    node *tmp = q->front;
    // queue has one elment
    if (q->front == q->rear)
    {
        q->front = NULL;
        q->rear = NULL;
    }
    // queue has more than one element
    // next node becomes the front
    else 
    {
        q->front = q->front->next;
    }

    q->size--;
    int d = tmp->value;

    free(tmp);
    return d;

}

void printQueue(queue *q)
{
    if (q->size == 0)
    {
        printf("Empty queue!");
        exit(4);
    }

    node *tmp = q->front;
    while (tmp != NULL)
    {
        printf("%d -> ", tmp->value);
        tmp = tmp->next;
    }
    printf("NULL");
}


int main(int argc, char **argv)
{

    queue *qu = queueInit();
    printf("Queue is empty: %d\n", isEmpty(qu));
    enqueue(qu, 1);
    enqueue(qu, 2);
    enqueue(qu, 5);
    printf("Pop: %d\n", dequeue(qu));
    printQueue(qu);
    destroyQueue(qu);

    return 0;
}
