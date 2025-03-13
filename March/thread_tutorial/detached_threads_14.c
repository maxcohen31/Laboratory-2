#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>

/*
    E' possibile avere dei thread separati dal main thread
    Un thread separato non è più "joinabile", inoltre fanno il clear delle loro risorse.

*/

#define THREAD_NUM 2

void *routine()
{
    sleep(1);
    printf("Finito\n");
}

int main(int argc, char **argv)
{
    pthread_t th[THREAD_NUM];
    pthread_attr_t detachedThread;
    pthread_attr_init(&detachedThread);
    pthread_attr_setdetachstate(&detachedThread, PTHREAD_CREATE_DETACHED);
    
    int i;
    for (i = 0; i < THREAD_NUM; i++) 
    {
        if (pthread_create(&th[i], &detachedThread, &routine, NULL) != 0) {
            perror("Failed to create thread");
        }
        // pthread_detach(th[i]);
    }

    for (i = 0; i < THREAD_NUM; i++) 
    {
        if (pthread_join(th[i], NULL) != 0) {
            perror("Failed to join thread");
        }
    }
    pthread_attr_destroy(&detachedThread);
    pthread_exit(0);
}

