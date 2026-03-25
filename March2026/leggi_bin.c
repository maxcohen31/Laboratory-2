/*
 *  Example of reading the dimension of a file and 
 *  how to read from a binary file using fread.
 */

#include <stddef.h>
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>
#include <string.h>
#include <errno.h>


void termina(const char *msg);
 

int main(int argc, char **argv)
{
    if (argc != 2)
    {
        printf("Usage:\t%s <file_name>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    char *file_name = argv[1];

    FILE *f = fopen(file_name, "rb");
    if (f == NULL) termina("Error opening file\n");

    /* Reading all integers from file and store them into an array */

    /* File's dimension */
    int e = fseek(f, 0, SEEK_END);
    if (e != 0) termina("Fseek error");
    /* Where am i? */
    long file_len = ftell(f);
    if (file_len < 0) termina("Ftell error");
    if (file_len % 4 != 0) termina("File does not contain int32\n");
    /* Number of integers in the file */
    int n = file_len / 4;
    if (n == 0) termina("Empy file");
    int *a = malloc(n * sizeof(*a));
    if (a == NULL) termina("Error allocating memory for the array\n");
    /* Rewind */
    rewind(f);

    /* Reading all integers from the file */
    /* fread: 
     * - a is where we store the chunks of data 
     * - sizeof(int) is the dimension of each block in bytes 
     * - n is the number of items that will be read from the input stream 
     * - f pointer to our input stream */
    size_t m = fread(a, sizeof(int), n, f);
    if (n != m) termina("Fread error");
    /* Closing file */
    if (fclose(f) == EOF) termina("Error closing file\n");

    /* Prints out the integers */
    for (int i = 0; i < n; i++) printf("%20d", a[i]);
    puts("");
    free(a);
     
    return 0;
}

void termina(const char *msg)
{
    if (errno == 0) fprintf(stderr, "%s\n", msg);
    else 
    {
        perror(msg);
        exit(1);
    }
}
