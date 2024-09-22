#define _GNU_SOURCE
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>


void build_arrays(int size_array)
{

    assert(size_array > 0 && size_array <= 99999 && "Size must be greater than 0 and lesser or equal than 99999");

    int n;
    printf("Insert a positive n between 1 and 99999 -> ");
    int e = scanf("%d", &n);

    if (e != 1)
    {
        printf("Error. Exit...");
        exit(1);
    }
    if (n <= 0)
    {
        printf("n must be positive. Exit...");
        exit(2);
    }
    if (n > 99999)
    {
        printf("n must be lesser than 99999! Exit...");
        exit(3);
    }

    int *array_three;
    int *array_five;
    int size3 = size_array;
    int size5= size_array;
    int num_in_arr3 = 0;
    int num_in_arr5 = 0; 

    // creating dynamic arrays
    array_three = malloc(size3*sizeof(int));
    array_five = malloc(size5*sizeof(int));

    for (int i = 3; i <= n; i++)
    {
        if ((i % 3 == 0) && (i % 5 != 0))
        {
            // check if space in array_three is avaiable
            if (num_in_arr3 == size3)
            {
                // if not enough space we double the size
                array_three = realloc(array_three, 2*size3*sizeof(int));
            }
            array_three[num_in_arr3] = i; 
            num_in_arr3++;
        }
        if ((i % 5 == 0) && (i % 3 != 0))
        {
            // check if space in array_five is avaiable
            if (num_in_arr5 == size5)
            {
                array_five = realloc(array_five, 2*size5*sizeof(int));
            }
            array_five[num_in_arr5] = i;
            num_in_arr5++;
        }
    }

    // reduce the sizes to their minimum
    size3 = num_in_arr3;
    size5 = num_in_arr5;
    
    // create two dynamic array with new sizes
    array_three = realloc(array_three, size3*sizeof(int));
    if (array_three == NULL) { puts("Malloc failed"); }
    array_five = realloc(array_five, size5*sizeof(int));
    if (array_five == NULL) { puts("Malloc failed"); }

    for (int i = 0; i < num_in_arr3; i++)
    {
        printf("%8d", array_three[i]);
    } 
    
    printf("\n");

    for (int i = 0; i < num_in_arr5; i++)
    {
        printf("%8d", array_five[i]);
    }

    int sum3 = 0;
    int sum5 = 0;

    for (int i = 0; i < num_in_arr3; i++)
    {
        sum3 += array_three[i];
    }
    
    printf("\n");
        
    for (int i = 0; i < num_in_arr5; i++)
    {
        sum5 += array_five[i];
    }
    
    printf("\nSum of elements multiple of 3 is %d", sum3);
    printf("\nSum of elements multiple of 5 is %d", sum5);

    printf("\nLength of array_three is %d", num_in_arr3);
    printf("\nLength of array_five is %d", num_in_arr5);

    free(array_three);
    free(array_five);
    
}

int main(int argc, char *argv[])
{
    // build_arrays(99999);
    build_arrays(100);

    return 0;
}
