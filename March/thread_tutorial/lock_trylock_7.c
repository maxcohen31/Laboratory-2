#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

/*
    Differenza tra lock e unlock
    Lo scopo del trylock e far fare ai thread altre cose quando la sezione critica è in uso
*/

pthread_mutex_t mutex;

void *routine(void *arg)
{
    // pthread_mutex_lock(&mutex);
    if (pthread_mutex_trylock(&mutex) == 0) 
    {
        printf("Lock presa\n");
        // i 4 thread iniziano ad eseguire la funzione routine
        // ma solo uno di loro fa il lock del mutex. Gli altri devono attendere
        // con il trylock stiamo provando a prendere il lock anche se 
        // non è garantito il suo ottenimento. Se non lo prendiamo 
        // il codice della sezione critica non deve essere eseguito 
        // quindi è buona pratica controllare lo stato di ritorno
        // del trylock
        sleep(1);
        pthread_mutex_unlock(&mutex);
    }
    else
    {
        printf("Lock non preso\n");
    }
    return NULL;
}

int main(int argc, char **argv)
{
    pthread_t th[4];
    
    pthread_mutex_init(&mutex, NULL);
    for (int i = 0; i < 4; ++i)
    {
        if (pthread_create(&th[i], NULL, &routine, NULL) != 0)
        {
            perror("Creazione thread fallita");
        }
    }
    for (int i = 0; i < 4; ++i)
    {
        if (pthread_join(th[i], NULL) != 0)
        {
            perror("Creazione thread fallita");
        }
    }
    
    pthread_mutex_destroy(&mutex);
    return 0;
}
