#define _GNU_SOURCE
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>




int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        fprintf(stderr, "Need to pass exactly three arguments: file - numb - range");
    }


    for (int i = 1; i < argc ; i++)
    {
        char *name = NULL;
        int e = asprintf(&name, "%d.tab", atoi(argv[i]));
        if (e == -1)
        {
            fprintf(stderr, "Error opening file");
        }

        FILE *f = fopen(name, "w");
        if (f == NULL)
        {
            fprintf(stderr, "Error opening file");
        }

        int n = atoi(argv[i]);
        for (int i = 1; i <= 10; i++)
        {
            int n3 = i * n;
            fprintf(f, "%d\n", n3);
        }

        fclose(f);
    }

    return 0;

}
