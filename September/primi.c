#define _GNU_SOURCE
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>

// Per ottenere la lista delle directory nelle quali i file .h
// vengono cercati dal compilatore scrivere sulla linea di comando
//  gcc -xc /dev/null -E -Wp,-v 2>&1 | sed -n 's,^ ,,p'


bool is_prime(int n)
{
    if (n <= 1) 
    {
        return false;
    } 

    int start = 2; // 2 is prime
    
    while (start <= sqrt(n))
    {
        if (n % start == 0) 
        {
            return false;
        }
        start++;
    }

    return true;
}

int main(int argc, char *argv[])
{
    
    int n;
    printf("Insert a positive number: ");
    int e = scanf("%d", &n);

    if (e != 1) 
    { 
        puts("Missing value"); 
        exit(1);
    }
    if (e < 1) 
    { 
        puts("Value must be positive"); 
        exit(2);
    }

    // creating a dynamic array 
    int size= 10;
    int number_in_arr = 0;
    int *a; 
    
    a = malloc(size*sizeof(int));
    if (a == NULL)
    {
        printf("Malloc failed");
    }

    for (int i = 0; i < n; i++)
    {
        // check if prime
        if (is_prime(i))
        {
            // check if size is equal to capacity; if not so realloc 
            if (size == number_in_arr)
            {
                a = realloc(a, 2*size*sizeof(int)); // doubling the capacity
                if (a == NULL) 
                {
                    printf("Malloc failed");
                }
            }
            a[number_in_arr] = i;
            number_in_arr++;
        }
    }

    // Reducing the array to it's minimum capacity
    size = number_in_arr;
    a = realloc(a, size*sizeof(int));

    // print the primes table
    for (int i = 0; i < number_in_arr; i++)
    {
        printf("%8d", a[i]);
    }

    printf("\n");

    free(a);

    return 0;
}
