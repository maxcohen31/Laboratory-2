#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

/*
    WAIT vs BROADCAST
    nell'esempio precedente con le variabili di condizione avevamo una macchina car(). 
    Ma se ne avessimo 4 cosa?
    Se fai partire il programma senza le dovute modifiche noterai che solo
    le prime due macchine vengono rifornite. 
    Signal sveglia un solo thread 
*/

// variabile globale che verrà condivisa dai thread -> mutex
int fuel = 0;
pthread_mutex_t mutexFuel;
pthread_cond_t condFuel;

void *fuelFilling(void *arg)
{

    for (int i = 0; i < 5; ++i)
    {
        pthread_mutex_lock(&mutexFuel);
        fuel += 30;
        printf("Ho messo benzina... %d\n", fuel);
        pthread_mutex_unlock(&mutexFuel);
        // pthread_cond_signal(&condFuel); // manda il segnale al thread che sta aspettando in car()
        pthread_cond_broadcast(&condFuel);
        sleep(1);
    }
    return NULL;

}

void *car(void *arg)
{
    // la macchina però parte con rifornimento negativo. Come risolviamo questo aspetto?
    // dobbiamo aspettare che ci siano almeno 40 unità di benzina nella macchina 
    // per fare in modo che il messaggio venga stampato
    // se facessimo un ciclo while(fuel < 40) then print("messo benzina");sleep(1);
    // questo non finirà mai. Il problema sta nel lock in fuelFillin() che non viene mai rilasciato
    // abbiamo bisogno di una variabile di condizione
    pthread_mutex_lock(&mutexFuel);
    while (fuel < 40)
    {
        printf("Aspettando la benzina\n");
        pthread_cond_wait(&condFuel, &mutexFuel);  // aspetta un segnale da un altro thread
    }                                              // che può cambiare o no la condizione
                                                   // Equivalente a:
                                                   // pthread_mutex_unlock(&mutexFuel)
                                                   // e aspetta un segnale per condFuel
                                                   // e poi chiama pthread_cond_lock(&mutexFuel)
    fuel -= 40;
    printf("Rifornimento fatto, mi resta %d. Partoooooo\n", fuel);
    pthread_mutex_unlock(&mutexFuel);   
    return NULL;                       
}

int main(int argc, char **argv)
{
    /*
        L'ordine di esecuzione è semplicemente il seguente:
        auto1:  lock -> cond_wait -> unlock
        auto2:  lock -> cond_wait -> unlock
        pompa1: lock
        auto3:  lock (waiting on lock)
        auto4:  lock (waiting on lock)
    */
    pthread_t th[6];
    pthread_mutex_init(&mutexFuel, NULL);
    pthread_cond_init(&condFuel, NULL);
    for (int i = 0; i < 6; ++i) 
    {
        if (i == 4 || i == 5) // le auto vengo create prima
                              // 2 pompe e 4 auto
        {
            if (pthread_create(&th[i], NULL, &fuelFilling, NULL) != 0)
            {
                perror("Thread non creato");
            }
        }
        else
        {
            if (pthread_create(&th[i], NULL, &car, NULL) != 0)
            {
                perror("Thread non creato");
            }
        }
    }

    for (int i = 0; i < 6; ++i)
    {
        if (pthread_join(th[i], NULL) != 0)
        {
            perror("Join dei thread fallita");
        }
    }

    pthread_mutex_destroy(&mutexFuel);
    pthread_cond_destroy(&condFuel);

    return 0;

}
