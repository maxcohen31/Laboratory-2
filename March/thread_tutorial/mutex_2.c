#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>

int mails = 0;
pthread_mutex_t mutex;

void *routine()
{
    for (int i = 0; i < 1000000; ++i)
    {
        // i mutex nel for loop pesano sulla CPU
        // buona norma è metterli prima del ciclo
        pthread_mutex_lock(&mutex);
        mails++;
        pthread_mutex_unlock(&mutex);
    }
    return NULL;
}


int main(int argc, char **argv)
{
    pthread_t t1;
    pthread_t t2;
    pthread_t t3;
    pthread_t t4;

    // inizializza mutex
    pthread_mutex_init(&mutex, NULL);

    if (pthread_create(&t1, NULL, &routine, NULL) != 0) 
    {
        return 1;
    }
    if (pthread_create(&t2, NULL, &routine, NULL) != 0) 
    {
        return 2;
    }
    if (pthread_create(&t3, NULL, &routine, NULL) != 0) 
    {
        return 3;
    }
    if (pthread_create(&t4, NULL, &routine, NULL) != 0) 
    {
        return 4;
    }
    if (pthread_join(t1, NULL) != 0) 
    {
        return 5;
    }
    if (pthread_join(t2, NULL) != 0) 
    {
        return 6;
    }
    if (pthread_join(t3, NULL) != 0) 
    {
        return 7;
    }
    if (pthread_join(t4, NULL) != 0) 
    {
        return 8;
    }
    pthread_mutex_destroy(&mutex);
    printf("Number of mails: %d\n", mails);

    return 0;
}
