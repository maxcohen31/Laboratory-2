/*
    Reference: https://www.youtube.com/@esadecimale

    First implementation
*/


#include <stdio.h>
#include <stdlib.h>

typedef struct 
{
    int *data;
    int capacity; /* total number of elements */
    int size; /* current number of elements */
} Numbers;

/* interface */
Numbers numbers_init(int capacity);
void numbers_append(Numbers *n, int value);
void numbers_destroy(Numbers n);

Numbers numbers_init(int capacity)
{
    Numbers n = { 0 };
    n.capacity = capacity;
    n.size = 0;
    n.data = malloc(n.capacity * sizeof(int));

    return n;
}

void numbers_append(Numbers *n, int value)
{
    if (n->size == n->capacity)
    {
        n->capacity *= 2; /* double the capacity */
        n->data = realloc(n->data, n->capacity * sizeof(int));
    }
    n->data[n->size] = value;
    n->size++;
}

void numbers_destroy(Numbers n)
{
    free(n.data);
}

int main(int argc, char **argv)
{
    Numbers n = numbers_init(5);
    for (int i = 0; i < 5; ++i) { numbers_append(&n, i); }
    for (int i = 0; i < 5; ++i) { printf("%d ", n.data[i]); }
    printf("\n");
    
    for (int i = 0; i < 5; ++i) { numbers_append(&n, i); }
    for (int i = 0; i < 10; ++i) { printf("%d ", n.data[i]); }
    printf("\n");

    return 0;

}
