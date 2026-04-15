/*
 * Esempio base di gestione asincrona di un segnale 
 * utilizzando un handler definito con sigaction 
 */

#include "xerrori.h"
#include <signal.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>

#define QUI __LINE__,__FILE__

/* Variabili globali utilizzate dal main e dal signal handler */
volatile sig_atomic_t tot_signali = 0;
volatile sig_atomic_t continua = 0;

void handler(int s)
{
    tot_signali++;
    if (s != SIGUSR1) kill(getpid(), SIGUSR1); /* Manda SIGUSR1 a se stesso */
    /* La printf non va usata in un signal handler! */
    printf("Segnale %d ricevuto dal processo %d", s, getpid());
    if (s == SIGUSR2) continua = 0; /* Forza l'uscita dal loop infinito del main */
}

int main(int argc, char **argv)
{
    /* Definisce signal handler */
    struct sigaction sa;
    sa.sa_handler = &handler;
    
    /* Setta sa.sa_mask che è la maschera dei segnali da bloccare
     * durante l'esecuzione di un handler() */
    sigfillset(&sa.sa_mask); /* Tutti i segnali nella maschera */
    sigdelset(&sa.sa_mask, SIGUSR1); /* tranne SIGUSR1 */
    sigaction(SIGUSR1, &sa, NULL); /* Handler per USR1 */
    sigaction(SIGUSR2, &sa, NULL); /* Handler per USR2 */

    /* Definisco una variabile dove salvo il settaggio attuale per SIGINT */
    struct sigaction oldsa;
    sigaction(SIGINT, &oldsa, &sa); /* Stesso handler per Ctrl-C */

    /* Visualizza il pid */
    printf("Se vuoi mandarmi dei segnali il mio pid è %d\n", getpid());
    continua = 1;
    do 
    {
        sleep(100);
        puts("Loop1: svegliato");
    } while (continua != 0); 
    printf("Ricevuti: %d segnali (uscito dal primo loop)\n", tot_signali);
   
    /* Rimetti la vecchia gestione di SIGINT */
    sigaction(SIGINT, &oldsa, NULL);
    /* Da ora SIGINT interrompe l'esecuzione come per default */
    puts("Vecchio SIGINT ripristinato");

    /* Rientro nel loop, per uscire serve un altro segnale usr2
     * oppure un SIGINT che ora fa terminare il programma */
    continua = 1;
    do 
    {
        pause(); /* Aspetta un segnale */
        puts("Loop2: svegliato");
    } while (continua);
    printf("Ricevuti: %d segnali (uscito dal secondo loop)\n", tot_signali);

    return 0;
}
