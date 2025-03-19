#include "xerrori.h"

#define QUI __LINE__,__FILE__

int main(int argc, char **argv)
{
    int n = 3; // figli da generare
    for (int i = 0; i < n; ++i)
    {
        // fork del processo
        pid_t p = xfork(QUI);
        if (p == 0)
        {
            printf("Io sono %d, principe di Moria, figlio %d-esimo di %d\n", getpid(), i, getppid());
            sleep(35 - (10 * i));
            return ((3 * i) + 1); // equivalente a exit perché siamo nel main
        }
    }

    // genitore
    printf("Io sono %d, re sotto la montagna, figlio di %d\n", getpid(), getppid());
    
    int totDraghi = 0;
    for (int i = 0; i < n; ++i)
    {
        int status;
        pid_t p = xwait(&status, QUI);
        printf("E' terminata la missione del figlio %d\n", p);

        if (WIFEXITED(status))
        {
            totDraghi += WEXITSTATUS(status);
            printf("Draghi uccisi da questo figlio: %d\n", WEXITSTATUS(status));
        }
        else
        {
            printf("Figlio non terminato con exit\n");
        }
    }
    printf("Finito\n");

    return totDraghi;
} 
