#define _GNU_SOURCE
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/wait.h>
#include <unistd.h>

/*
 *  Simulazione dell'operatore "|" tramite processi.
 *  
 *  dup2() -> senza dup2 il processo ping continuerà a scrivere sul suo stdout, cioè il terminale e non sulla pipe
 *            Il processo grep invece leggerà dallo stdin normale, il terminale e non la pipe
*/

int main(int argc, char **argv)
{
    int fd[2];
    if (pipe(fd) == -1)
    {
        return 1;
    }

    // creiamo due processi
    pid_t pid1 = fork();
    if (pid1 < 0)
    {
        return 2;
    }

    if (pid1 == 0)
    {
        // processo figlio - esegue ping
        // exec sostituisce tutto dentro il processo corrente.
        // Questa chiamata a exec non restituisce niente e quindi non abbiamo bisogno
        // del ramo else per il processo padre perché sappiamo che dopo l'if 
        // tutto sarà eseguito dal processo main
        // il processo figlio verrà sostituito con la chiamata a ping
        // dup2() prende il primo fd1 e lo duplica in fd2. fd2 punterà a fd1
        dup2(fd[1], STDOUT_FILENO); // redirect stdout sulla pipe
        close(fd[0]); // chiudo lettura - inutile per chi scrive
        close(fd[1]); // chiuso perchè altrimento avremmo due file descriptor puntare alla stessa pipe. Ce ne basta uno 
        execlp("ping", "ping", "-c", "5", "google.com", NULL);
    }

    pid_t pid2 = fork();
    if (pid2 < 0)
    {
        return 3;
    }

    // secondo processo figlio - grep
    if (pid2 == 0)
    {
        dup2(fd[0], STDIN_FILENO); // redirect sullo stdin
        close(fd[0]); // chiude lettura dopo dup2
        close(fd[1]); // chiude scrittura - inutile per chi legge
        execlp("grep", "grep", "rtt", NULL);
    }

    // senza queste il programma non termina dato che lo scrittore del main resta comunque aperto
    // il processo padre eredita i file descriptor della pipe
    // La pipe non si chiuderebbe mai (per grep), perché ci sarebbe ancora un processo con fd[1] aperto.
    // se non li chiudesse:
    // - la pipe non si chiuderebbe mai a causa di grep, perché ci sarebbe ancora un processo con fd[1] aperto
    // - grep aspetta EOF (fine input), ma non lo riceve mai se c’è ancora qualcuno che può scrivere.
    close(fd[0]);
    close(fd[1]);

    waitpid(pid1, NULL, 0);
    waitpid(pid2, NULL, 0);
    return 0;
}
