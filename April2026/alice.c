/*
   Si consideri il seguente gioco tra Alice e Bob: si parte da un intero N, a turno ogni giocatore può:

    se N pari sottrarre 1 da N oppure dimezzarlo
    se N è dispari può solo sottrarre 1 (la mossa è forzata)
    vince che raggiunge il valore zero. Ad esempio, se il valore iniziale è 12 una possibile sequenza di gioco è:
    
    Bob: 12 -> 6
    Alice: 6 -> 5
    Bob: 5 -> 4
    Alice: 4 -> 2
    Bob: 2 -> 1
    Alice 1 -> 0 e vince

    Scrivere una coppia di programmi alice e bob che effettuano un certo numero di partite secondo il seguente schema:
    
    alice legge dalla linea di comando il valore iniziale N e lo comunica a bob con un segnale
    
    bob attende il valore N e successivamente esegue la prima mossa
    
    I programmi devono fare mosse valide (non importa se giocano male) e comunicare la mossa scelta all'avversario utilizzando un segnale (la mossa va comunicata anche quando è forzata)
    
    Chi riceve la mossa deve verificarne la correttezza e inviare la sua mossa di risposta ancora con un segnale
    
    Quando la partita finisce ogni programma deve stampare un messaggio ha vinto Alice oppure ha vinto Bob e poi si torna al punto 1 per una eventuale nuova partita.
    
    Idealmente i due programmi devono fare sempre mosse valide, accorgersi che la partita è finita e stampare lo stesso messaggio alla fine di ogni partita.
    
    Dovete scrivere due sorgenti diversi alice.c e bob.c; 
    l'utente deve eseguire dalla linea di comando alice che deve far partire bob con una fork + exec; 
    questo è necessario perchè ogni programma deve poter conoscere il pid dell'altro per inviare i segnali. 
    Inviate i segnali con la funzione sigqueue(3) per inviare un intero, e sigwaitinfo(2) per la ricezione dei segnali.
*/


#include "xerrori.h"
#define QUI __LINE__,__FILE__

/* Make a move */
int move(int n)
{
    if (n % 2 == 0) return n / 2;
    return n - 1;
}

/* Check if Bob's move is a valid one */
bool is_valid_move(int prev, int received)
{
    if (prev % 2 == 0) return (received == prev / 2 || received == prev - 1);
    else return (received == prev - 1);
}

/* Alice is the parent process */
int main(int argc, char **argv)
{
    if (argc < 2)
    {
        fprintf(stderr, "Usage:\t%s N\n", argv[0]);
        exit(1);
    }

    /* Blocking SIGRTMIN */
    sigset_t set;
    sigemptyset(&set);
    sigaddset(&set, SIGRTMIN);

    if (sigprocmask(SIG_BLOCK, &set, NULL) == -1) xtermina("sigprocmask() error", QUI);

    /* Creating Bob */
    pid_t bob = fork();
    if (bob < 0) xtermina("fork() error", QUI);
    if (bob == 0) 
    {
        execl("./bob.out", "./bob.out", NULL);
        termina("execl error");
    }
    
    printf("[A] Bob is being created\n");

    union sigval v;
    siginfo_t siginfo;
    int n = atoi(argv[1]);
    /* Sending n to Bob */
    v.sival_int = n;
    if (sigqueue(bob, SIGRTMIN, v) == -1) xtermina("sigqueue() error", QUI);

    int prev = n; /* Used for checking the legality of a move */
    
    while (true)
    {
        /* Waiting for Bob's move */
        if (sigwaitinfo(&set, &siginfo) == -1) xtermina("sigwaitinfo() error", QUI);
        int received = siginfo.si_value.sival_int;

        printf("[A] Alice receives %d\n", received);

        /* Is a valid move? */
        if (!is_valid_move(prev, received)) 
        {
            fprintf(stderr, "[A] Invalid move by Bob!\n");
            exit(2);
        }

        if (received == 0)
        {
            printf("[A] Bob wins!\n");
            break;
        }

        /* Alice's move */
        int alice_move = move(received);
        prev = alice_move;
        
        printf("[A] Alice sends %d\n", alice_move);
        v.sival_int = alice_move;
        if (sigqueue(bob, SIGRTMIN, v) == -1) xtermina("sigqueue error()", QUI);

        if (alice_move == 0)
        {
            printf("[A] Alice wins!\n");
            break;
        }
    }
    
    printf("[A] Termination signal sent to Bob\n");
    v.sival_int = -1;
    sigqueue(bob, SIGRTMIN, v);
    wait(NULL);

    return 0;
}
