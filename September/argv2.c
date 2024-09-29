#define _GNU_SOURCE
#include <stdio.h>


int main(int argc, char *argv[])
{
    int i;
    int t;

    // stampa tutti gli argomenti passati al programma
    printf("argc: %d\n", argc);
    for (t = 0; t < argc; t++)
    {
        i = 0;
        while (argv[t][i])
        {
            putchar(argv[t][i]);
            i++;
        }
        puts("\n");
    }

    return 0;
}
