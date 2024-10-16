#define _GNU_SOURCE
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

void swap(int *x, int *y)
{
    int tmp = *x;
    *x = *y;
    *y = tmp;
}

int partition(int a[], int p, int r)
{
    int pivot = a[p];
    int i = p - 1;
    int j = r + 1;

    do 
    {
        do 
        {
            j--; // decreasing j since the pivot is lesser than a[j]
        } 
        while (a[j] > pivot);

        do 
        {
            i++; // increasing i since a[i] is lesser than pivot element
        }
        while (a[i] < pivot);
        
        if (i < j)
        {
            swap(&a[i], &a[j]);
        }
    } 
    while (i < j); // main cycle

    return j;
}

void quickSort(int arr[], int l, int r) {

    if (l < r) 
    {
        int part = partition(arr, l, r);

        quickSort(arr, l, part - 1);
        quickSort(arr, part + 1, r);
    }
}


int main(int argc, char *argv[])
{
    int arr[7] = {10, 4, 3, 6, 7, 9, 1};
    int n = (sizeof(arr) / sizeof(arr[0]));

    quickSort(arr, 0, n  - 1);

    for (int i = 0; i < n; i++)
    {
        printf("%d ", arr[i]);
    }

    return 0;
}
