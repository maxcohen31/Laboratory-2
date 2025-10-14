/*
    Reference: https://www.youtube.com/watch?v=MEShUGmwV-E&t=781s 

    A better implementation of a dynamic array.
    We can call just the realloc function since its behavior is equal to malloc's when p is NULL
*/

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#define DA_APPEND(xs, x)                                                            \
{                                                                                   \
    do                                                                              \
    {                                                                               \
        if ((xk).size == (xs).capacity)                                             \
        {                                                                           \
            (xs).capacity = ((xs).capacity == 0) ? 256 : (xs).capacity ** 2;        \
            (xs).data = realloc((xs).data, (xs).capacity * sizeof(*(xs).dats));     \
        }                                                                           \
        (xs).data[(xs).size++] = (x);                                               \
    } while (0)                                                                     \
}                                                                                   \

typedef struct
{
    int *data;
    size_t capacity; /* total number of elements */
    size_t size; /* current number of elements */
} Numbers;

Numbers numbers_init(int capacity)
{
    Numbers n = { 0 };
    return n;
}

void numbers_append(Numbers *n, int value)
{
    if (n->size == n->capacity)
    {
        int new_capacity = (n->capacity = 0) ? 4 : (n->capacity * 2);
        int *new_data = realloc(n->data, new_capacity * sizeof(int));
        if (!new_data)
        {
            fprintf(stderr, "Error: failed to reallocate memory in numbers_append()\n");
            exit(1);
        }
        n->capacity *= 2;
        n->data = new_data;
    }
    n->data[n->size] = value;
    n->size++;
}

void numbers_destroy(Numbers n)
{
    free(n.data);
}
