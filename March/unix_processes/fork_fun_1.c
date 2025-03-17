#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h> // Linux specific library


int main(int argc, char **argv)
{
    // two processes created: main and the forked one
    int id = fork();
    fork();
    if (id == 0)
    {
        printf("Hi, from child process!\n");
    }
    else
    {
        printf("Hello from main process\n");
    }
    return 0;
}
