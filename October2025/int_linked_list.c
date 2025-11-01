/*
    Scrivere un funzione int confronta_lunghezza(intero *lis1, intero *lis2);
    che date due liste di interi lis1 e lis2 (possibilemente anche vuote) restituisce -1 se lis1 ha meno elementi di lis2, 
    1 se lis1 ha più elementi di lis2, 0 se hanno lo stesso numero di elementi (le liste vuote si considerano avere 0 elementi).
    
    Si scriva poi un main che legge dalla linea di comando un numero qualsiasi di interi e crea due liste contenenti rispettivamente gli interi pari 
    e gli interi dispari passati sulla linea di comando (si noti che le liste possono essere vuote).
    
    Successivamente il main deve invocare confronta_lunghezza e stampare la lista più lunga; 
    se le liste hanno la stessa lughezza deve stampare quella la cui somma degli elementi è maggiore, se anche la somma degli elementi è la stessa non deve stampare nulla.
    
    Il programma deve poi deallocare tutta la memoria utilizzata e terminare.
*/

#define _GNU_SOURCE
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>


/* struct representing a list'node */
typedef struct _intero {
  int valore;
  struct _intero *next;
} intero;


/* function for creating a node of the list */
intero *makeNode(int val)
{
    intero *node = malloc(sizeof(intero));
    if (!node)
    {
        fprintf(stderr, "Error: failed allocation for node in makeNode function\n");
    }
    node->valore = val; 
    node->next = NULL;

    return node;
}

/* freeing the alllocated memory used for building up the list */
void destroList(intero *list)
{
    intero *head = list;
    while (head != NULL)
    {
        intero *nextNode = head->next;
        free(head);
        head = nextNode;
    };
}

/* printing the list */
void printList(intero *list)
{
    while (list != NULL)
    {
        printf("%d ", list->valore);
        list = list->next;
    }
    printf("\n");
}

/* function that inserts a given node at the end of our list */
intero *insertTail(intero *list, intero *node)
{
    if (list == NULL)
    {
        return node; 
    }

    intero *current = list; /* save the current node */
    while (current->next != NULL)
    {
        current = current->next; /* searching for the last node */
    }

    current->next= node; /* next of current becomes the new node */
    node->next = NULL; /* new node's next filed becomes NULL */

    return list;
}

/* functions that inserts a node at the beginning of our list */
intero *insertHead(intero *list, intero* node)
{
    if (list == NULL)
    {
        list = node;
    }
    node->next = list;

    return node;
}

/* length of a list */
int length(intero *list)
{
    if (list == NULL)
    {
        return 0;
    }

    int counter = 0; /* gives us the number of elements in our list */
    intero *current = list;
    while (current != NULL) 
    {
        current = current->next;
        counter++;
    }

    return counter;
}

/* summing element of a list */
int sum(intero *list)
{
    if (list == NULL)
    {
        return 0;
    }

    int result = 0;
    intero *current = list;
    while (current != NULL)
    {
        result += current->valore;
        current = current->next;
    }
    return result;

}

/* compare lengths of two lists. Returns: 1  if list1 is longer, -1 if list2 is longer, 0 if they have the same length. */
int compareLists(intero *list1, intero *list2)
{
    int len1 = length(list1);
    int len2 = length(list2);

    if (len1 < len2) { return -1; }
    if (len2 < len1) { return 1; }
    else { return 0; }

}

int main(int argc, char **argv)
{
    if (argc < 3)
    {
        fprintf(stderr, "Usage:\t %s <int1> <int2> ... <intN>\n", argv[0]);
    }

    intero *listEven = NULL;
    intero *listOdd = NULL;

    for (int i = 1; i < argc; i++)
    {
        int val = atoi(argv[i]);
        intero *node = makeNode(val); 

        if (val % 2 == 0)
        {
            listEven = insertTail(listEven, node);
        }
        else 
        {
            listOdd = insertTail(listOdd, node);
        }
    }

    /* comparing our lists */
    int cmp = compareLists(listEven, listOdd); /* stores comparison betwenn lists */
    if (cmp == 1) { printList(listEven); } /* listEven contains more elements than listOdd */
    else if (cmp == -1) { printList(listOdd); } /* listOdd contains more elements than listEven */
    else 
    {
        printf("Lengths are the same: calculating the sums... \n");
        /* summming the elements and return the list with the larger sum */
        int sumEven = sum(listEven);
        int sumOdd= sum(listOdd);

        if (sumEven > sumOdd) { puts("list of even elements has the largest sum\n"); printList(listEven); }
        else if (sumEven < sumOdd) { puts("list of odd elements has the largest sum\n"); printList(listOdd); }
        else { printf("Lists share the same sum: avoid printing\n") ;}
    }

    destroList(listEven);
    destroList(listOdd);

    return 0;
}
