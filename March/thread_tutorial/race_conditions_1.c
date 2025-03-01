#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>


/*
    RACE CONDITIONS
                    
                 t1        t2
    read mails   29        23
    increment    29        30
    write mails  30        24

    Se il sistema operativo blocca t2 mentre t1 lavora su mails quando t2 riprenderà l'esecuzione
    si avrà che t2 leggerà dal registro 23; la variabile è però a 30 dato che t1 stava lavorando
    ma t2 incrementerà 23 e non 30 generando così un risultato sbagliato. Come nel nostro caso.
    Perchè con 100 iterazioni invece funzionava? Semplicemente perché t1 finiva prima che t2 iniziasse
    l'esecuzione.
    Per aggirare questo problema abbiamo bisogno dei MUTEX
    

 */
int mails = 0;

void* routine() {
    for (int i = 0; i < 1000000; i++) 
    {
        mails++;
        // read mails
        // increment
        // write mails
    }
    return NULL;
}

int main(int argc, char* argv[]) {
    pthread_t p1, p2, p3, p4;
    if (pthread_create(&p1, NULL, &routine, NULL) != 0)
    {
        return 1;
    }
    if (pthread_create(&p2, NULL, &routine, NULL) != 0) 
    {
        return 2;
    }
    if (pthread_join(p1, NULL) != 0) 
    {
        return 3;
    }
    if (pthread_join(p2, NULL) != 0) 
    {
        return 4;
    }

    printf("Number of mails: %d\n", mails);
    return 0;
}
