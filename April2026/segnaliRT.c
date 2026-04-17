#include "xerrori.h"
#include <bits/types/siginfo_t.h>
#include <bits/types/sigset_t.h>
#include <pthread.h>
#include <signal.h>
#include <stdio.h>
#include <sys/select.h>
#include <time.h>
#include <unistd.h>
#define QUI __LINE__,__FILE__

/* 
    Esempio di invio di segnali anche real-time e di
    invio di una informazione insieme al segnale. 
    l'informazione (un int) è memorizzata in una union 

    Per leggere l'informazione la sigwait è rimpiazzata da sigwaitinfo 
    Nota: è possibile leggere l'informazione del segnale anche usando il campo
    sa_sigaction quando si chiama sigaction() per definire un
    handler di gestione dei segnali asincroni 
*/

/* Thread nullafacente */
void *tbody(void *a)
{
    while (true)
    {
        sleep(50);
        printf("===== Thread: %d svegliato =====", gettid());
    }
    return NULL;
}

/* Thread che effettua la gestione di tutti i segnali 
 * usa sigwaitinfo() per leggere l'informazione associata ai segnali */
void *tgestore(void *a)
{
    sigset_t mask;
    sigfillset(&mask);
    sigdelset(&mask, SIGQUIT); /* Elimino SIGQUIT dall'insieme */
    int s;
    siginfo_t sinfo;
    while (true)
    {
        int e = sigwaitinfo(&mask, &sinfo);
        if (e < 0) perror("Errore sigwaitinfo()");
        s = sinfo.si_signo;
        printf("Thread %d ricevuto segnale %d da %d", gettid(), s, sinfo.si_pid);
        printf("con valore %d\n", sinfo.si_value.sival_int);

        if (s == SIGUSR2)
        {
            /* Manda a se stesso un misto di segnali real-time e standard 
             * possibilmente con valore associato al segnale*/
            union sigval v; /* Union inviata al gestore dei segnali */

            v.sival_int = 1; /* Invia l'intero 1 insieme al segnale */
            e = pthread_sigqueue(pthread_self(), SIGTERM, v);
            if (e != 0) xperror(e, "errore pthread_sigqueue");

            v.sival_int++; /* invia l'intero 2 */
            e = pthread_sigqueue(pthread_self(), SIGRTMAX, v);
            if (e != 0) xperror(e, "errore pthread_sigqueue");

            v.sival_int++; /* Invia 3, 4, etc */
            e = pthread_sigqueue(pthread_self(), SIGRTMIN, v);
            if (e != 0) xperror(e, "errore pthread_sigqueue");

            v.sival_int++;
            /* Nota: raise(SIGINT) è equivalente a pthread_kill(pthread_self(), SIGINT) */
            e = raise(SIGINT);
            if (e != 0) perror("errore raise");

            v.sival_int++; /* Questo segnale viene perso perché è un secondo SIGINT */
            e = pthread_sigqueue(pthread_self(), SIGINT, v);
            if (e != 0) xperror(e, "errore pthread_sigqueue");

            v.sival_int++;
            e = pthread_sigqueue(pthread_self(), SIGRTMAX, v);
            if (e != 0) xperror(e, "errore pthread_sigqueue");

            v.sival_int++;
            // e = sigqueue(getpid(), SIGRTMIN+1, v) /* In questo modo mando il segnale con sigqueue() */
            // if (e != 0) perror(e, "pthread_sigqueue");
            e = pthread_sigqueue(pthread_self(), SIGRTMIN+1, v);
            if (e != 0) xperror(e, "errore pthread_sigqueue");

            /* E' l'ultimo segnale inviato ma verrà gestito prima di tutti i real time */
            v.sival_int++;
            e = pthread_sigqueue(pthread_self(), SIGUSR1, v);
            if (e != 0) xperror(e, "errore pthread_sigqueue");
        }
    }
    pthread_exit(NULL); 
}

int main(int argc, char **argv)
{
    /* Blocco i segnali tranne SIGQUIT */
    sigset_t mask;
    sigfillset(&mask); /* Insieme di tutti i segnali */
    sigdelset(&mask, SIGQUIT); /* Elimino SIGQUIT dall'insieme */
    pthread_sigmask(SIG_BLOCK, &mask, NULL); /* Blocco tutto tranne SIGQUIT */

    /* Visualizzo pid */
    printf("Se vuoi mandarmi dei segnali il mio pid è: %d\n", getpid());

    /* Creazione di nuovi thread che ereditano i settaggi dei segnali */
    pthread_t t[3];
    xpthread_create(&t[0], NULL,  tbody, NULL, QUI);
    xpthread_create(&t[1], NULL,  tbody, NULL, QUI);
    xpthread_create(&t[2], NULL,  tgestore, NULL, QUI);

    while (true)
    {
        sleep(40);
        puts("Main svegliato!");
    }

    return 0;
}
