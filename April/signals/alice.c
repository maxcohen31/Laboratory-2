#define _GNU_SOURCE
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdbool.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>

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
    
    // crea processo bob
    pid_t bob = fork();
    if (bob == -1)
    {
        perror("Errore fork bob");
        return 1;
    }
    
    if (bob == 0)
    {
        execl("./bob", "./bob", NULL); // fa partire il programma bob
        perror("Errore execl");
        return 1;
    }
    
    // loop principale per richiamare la partita
    while (true)
    {
        int n;
        printf("Inserisci un numero intero positivo (0 - esci): ");
        if (scanf("%d", &n) != 1)
        {
            fprintf(stderr, "Errore scanf\n");
            return 1;
        }
        
        if (n < 0)
        {
            printf("Inserisci un numero non negativo\n");
            continue;
        }
        
        // mando il numero a bob
        union sigval sv;
        sv.sival_int = n;
        if (sigqueue(bob, SIGRTMIN, sv) == -1)
        {
            perror("Errore sigqueue");
            return 2;
        }
        
        if (n == 0)
        {
            printf("Segnale di terminazione inviato a Bob\n");
            wait(NULL); // aspetta che Bob termini
            printf("Fine gioco\n");
            break;
        }
        
        // loop che gestisce la singola partita
        while (true)
        {
            // attende la risposta di bob
            siginfo_t siginfo;
            if (sigwaitinfo(&mask, &siginfo) == -1)
            {
                perror("Errore sigwait");
                return 3;
            }
            
            int result = siginfo.si_value.sival_int;
            printf("Alice riceve: %d\n", result);
            
            if (result == 0)
            {
                printf("Bob vince!\n");
                break;
            }
            
            // alice fa la sua mossa
            if (result % 2 == 0)
            {
                printf("Numero pari -> Alice dimezza\n");
                result = result / 2;
            }
            else 
            {
                printf("Numero dispari -> Alice forza la mossa\n");
                result = result - 1;
            }
            
            printf("Alice invia: %d\n", result);
            
            // invia la risposta a Bob
            sv.sival_int = result;
            if (sigqueue(bob, SIGRTMIN, sv) == -1)
            {
                perror("Errore sigqueue");
                return 4;
            }
            
            if (result == 0)
            {
                printf("Alice vince!\n");
                break;
            }
        }
    } 
    
    return 0;
}
