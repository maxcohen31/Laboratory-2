#include <string.h>
#define _GNU_SOURCE
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>


typedef struct _intero 
{
	int val;
	struct _intero *next;
} intero;


intero *make_intero(int n)
{
    // allocating memory for struct intero
    intero *x = malloc(sizeof(*x));
    if (x == NULL)
    {
        perror("Error allocating memory");
    }

    x->val = n;
    x->next = NULL;

    return x;
}

void destroyer(intero *a)
{
    intero *current = a;
    while (current != NULL)
    {
        intero *next = current->next;
        free(current);
        current = next;
    }
}

void print_list(intero *lis)
{
   
    while (lis != NULL)
    {
        printf("%d -> ", lis->val);
        lis = lis->next; 
    }
    puts("null");
}


intero *insert_head(intero *lis, intero *n)
{
    assert(n != NULL);

    n->next = lis;
    return n; 
} 
    
intero *insert_tail(intero *lis, intero *n)
{
    assert(n != NULL);

    if (lis == NULL)
    {
        return n;
    }

    intero *tail = lis;
    // searching the last element
    while (tail->next != NULL)
    {
        tail = tail->next;
    }
    tail->next = n;
    n->next = NULL;

    return lis;
}


int count_elements(intero *lis)
{
    assert(lis != NULL);

    int counter = 0;
    while (lis != NULL)
    {
        lis = lis->next;
        counter++;
    }
    return counter;
    
}
int confronta_lunghezza(intero *lis1, intero *lis2)
{

    int list1_elements = count_elements(lis1);
    int list2_elements = count_elements(lis2);

    if (list1_elements > list2_elements)
    {
        return 1;
    }
    
    if (list1_elements < list2_elements)
    {
        return -1;
    }
    else
    {
        return 0;
    }

 
}

int list_sum(intero *lis)
{
    int sum = 0;
    while (lis != NULL)
    {
        sum += lis->val;
        lis = lis->next;
    }
    return sum;
}


int main(int argc, char **argv)
{
    if (argc < 2) 
    {
        printf("Usage example: ./%s 2 3 4", argv[0]);
    }

    intero *l1 = NULL;
    intero *l2 = NULL;
    
    for (int i = 1; i < argc; i++)
    {
        int val = atoi(argv[i]);
        intero *node = make_intero(val);

        if (node->val % 2 == 0)
        {
            l1 = insert_tail(l1, node);
        }
        else 
        {
            l2 = insert_tail(l2, node);
        }
    }

    print_list(l1);
    puts("===============================");
    print_list(l2);
     
    puts("===== Compare Lists =====");
    
    if (confronta_lunghezza(l1, l2) == 1)
    {
        printf("Even elements: ");
        print_list(l1);
    }

    if (confronta_lunghezza(l1, l2) == -1)
    {
        printf("Odd integers");
        print_list(l2);
    }

    if (confronta_lunghezza(l1,l2) == 0)
    {
        int s1 = list_sum(l1);
        int s2 = list_sum(l2);

        if (s1 > s2) 
        {
            print_list(l1);
        } 
        else if (s1 < s2)
        {
            print_list(l2);
        }
        else 
        {
            printf("Lists shared the same amount of elements -> Sum of elements of both lists are equal -> no print\n");
        }

    }


    destroyer(l1);
    destroyer(l2);

    return 0;
}
