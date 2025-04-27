#define _GNU_SOURCE
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>

int main(int argc, char **argv)
{    
    // blocca il segnale SIGRTMIN
    sigset_t mask;
    sigemptyset(&mask);
    sigaddset(&mask, SIGRTMIN);
    if (sigprocmask(SIG_BLOCK, &mask, NULL) == -1)
    {
        perror("sigprocmask");
        return 1;
    }
    
    printf("Bob è in attesa...\n");
    
    while (true)
    {
        // attendo segnale da Alice
        siginfo_t siginfo;
        if (sigwaitinfo(&mask, &siginfo) == -1)
        {
            perror("Errore sigwaitinfo");
            return 2;
        }
        
        int receivedNumber = siginfo.si_value.sival_int;
        printf("Bob riceve: %d\n", receivedNumber);
        
        // Se ricevo 0, termino il programma
        if (receivedNumber == 0)
        {
            printf("Bob termina su richiesta di Alice\n");
            break;
        }
        
        // Bob fa la sua mossa
        if (receivedNumber % 2 == 0)
        {
            printf("Numero pari -> Bob dimezza\n");
            receivedNumber = receivedNumber / 2;
        }
        else 
        {
            printf("Numero dispari -> Bob forza la mossa\n");
            receivedNumber = receivedNumber - 1;
        }
        
        printf("Bob invia: %d\n", receivedNumber);
        
        // mando la mossa ad Alice
        union sigval sv;
        sv.sival_int = receivedNumber;
        if (sigqueue(getppid(), SIGRTMIN, sv) == -1)
        {
            perror("Errore sigqueue");
            return 3;
        }
        
        if (receivedNumber == 0)
        {
            printf("Bob vince!\n");
            // Non terminare, aspetta un nuovo numero da Alice
        }
    }
    
    return 0;
}
