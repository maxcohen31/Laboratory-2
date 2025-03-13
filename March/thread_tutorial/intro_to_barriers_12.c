#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <time.h>   
#include <unistd.h>
#include <semaphore.h>

/*
    Una barriera è un meccanismo che permette a thread multipli di aspettare 
    fino a quando la stessa non li lascia passare
    
    Esempio:
    
            3
       ->   |
       ->   |
            |

       La barriera verrà tolta solo quando ci saranno 3 thread
*/

pthread_barrier_t barrier;

void *routine(void *arg)
{
    while (true)
    {
        printf("Waiting at the barrier...\n");
        pthread_barrier_wait(&barrier);
        printf("We passed the barrier\n");
    }
}


int main(int argc, char **argv)
{
    pthread_t th[2];    
    pthread_barrier_init(&barrier, NULL, 3); // il terzo parametro è il contatore dei thread

    for (int i = 0; i < 2; ++i)
    {
        if (pthread_create(&th[i], NULL, &routine, NULL) != 0)
        {
            perror("Creazione thread fallita");
        }
    }
    
    for (int i = 0; i < 2; ++i)
    {
        if (pthread_join(th[i], NULL) != 0)
        {
            perror("Join fallita");
        }
    }

    pthread_barrier_destroy(&barrier);
    return 0;;
}
