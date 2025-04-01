#include "xerrori.h"
#define QUI __LINE__,__FILE__

// esempio base di gestione asincrona dei segnali 
// cioè utilizzando un handler


// variabili globali utilizzate da main e dal signal handler 
volatile sig_atomic_t tot_segnali = 0;
// il perche' della keyword volatile lo abbiamo visto a lezione
volatile sig_atomic_t continua = 1;


// funzione che viene invocata quando viene ricevuto 
// un segnale USR1 USR2 o INT (Control-C)
void handler(int s)
{
    
    tot_segnali++;
    if(s!=SIGUSR1) 
    {
        kill(getpid(),SIGUSR1); // manda SIGUSR1 a se stesso  
    }
    // la printf non si dovrebbe usare in un handler
    printf("Segnale %d ricevuto dal processo %d\n", s, getpid());
    if(s==SIGUSR2) 
    {
        // forza uscita dal loop infinito del main()
        continua = 0;
  }
}


int main(int argc, char *argv[])
{
    // definisce signal handler 
    struct sigaction sa;
    sa.sa_handler = &handler;
    // setta sa.sa_mask che è la maschera di segnali da bloccare
    // durante l'esecuzione di handler().
    sigfillset(&sa.sa_mask);          // tutti i segnali nella maschera ...
    // sigdelset(&sa.sa_mask,SIGUSR1);   // ... tranne SIGUSR1
    sigaction(SIGUSR1,&sa,NULL);  // handler per USR1
    sigaction(SIGUSR2,&sa,NULL);  // stesso handler per USR2
    // definisco variabile dove salvo il settaggio attuale per SIGINT
    struct sigaction oldsa;
    sigaction(SIGINT,&sa,&oldsa);   // stesso handler per Control-C

    // visualizza il pid
    printf("Se vuoi mandarmi dei segnali il mio pid e': %d\n", getpid());
    
    continua = 1;
    do {   // loop apparentemente senza uscita
        sleep(100);     // la sleep viene interrotta dai segnali 
        puts("loop1: svegliato");           
    } while(continua!=0); 
    printf("Ricevuti: %d segnali (uscito primo loop)\n", tot_segnali);
    
    // rimetti la vecchia gestione di SIGINT 
    sigaction(SIGINT,&oldsa,NULL);
    // ora SIGINT interrompe l'esecuzione come per default
    puts("Vecchio SIGINT ripristinato");

    // rientro nel loop, per uscire serve un altro segnale usr2
    // oppure un SIGINT...
    continua = 1;
    do { // loop apparentemente senza uscita
        pause();    // attende un segnale
        puts("loop2: svegliato");           
    } while(continua);
    printf("Ricevuti: %d segnali (secondo loop)\n", tot_segnali);
    return 0;
}
