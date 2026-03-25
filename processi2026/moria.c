/*
 *  Example of fork() to create children precesses and 
 *  how to read the return values with waitpid 
 * */



#include "xerrori.h"

int main(int argc, char **argv)
{
    int n = 3; /* Number of children to generate */
    for (int i = 0; i < n; i++)
    {
        pid_t p = xfork(__LINE__, __FILE__);
        if (p == 0) /* Child */
        {
            printf("I am %d, prince of Moria, %d-son of %d\n", getpid(), i, getppid());
            sleep(35 - (10*i));
            return ((3*i)+1); /* Same as exit() because we are inside main */
        }
    }

    /* Parent */
    printf("I am %d, king beneath the mountain, son of %d\n", getpid(), getppid());
    sleep(40);

    int tot_drakes = 0;
    for (int i = 0; i < n; i++)
    {
        int status;
        pid_t p = xwait(&status, __LINE__, __FILE__);
        printf("The quest of %d has terminated\n", p);
        if (WIFEXITED(status)) 
        {
            tot_drakes += WEXITSTATUS(status); /* Only 8 bits are read */
            printf("Killed drakes by this child: %d\n", WEXITSTATUS(status));
        }
        else printf("Child not terminated with exit\n");
    }
    printf("Done\n");
    return tot_drakes;
}


