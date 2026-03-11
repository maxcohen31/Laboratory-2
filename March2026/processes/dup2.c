/*
 * dup2.c
 *
 * Scopo: mostrare come una shell implementa una pipeline del tipo:
 *   ls -l | wc -l
 *
 * Idea:
 * - Si crea una pipe (pipefd[0] = lettura, pipefd[1] = scrittura).
 * - Primo figlio: redirige STDOUT (fd 1) sulla scrittura della pipe con dup2(),
 *   poi esegue "ls -l". L’output di ls finisce nella pipe.
 * - Secondo figlio: redirige STDIN (fd 0) sulla lettura della pipe con dup2(),
 *   poi esegue "wc -l". wc legge dalla pipe e conta le righe.
 * - Il padre chiude entrambe le estremità della pipe e aspetta i figli.
 *
 * Note:
 * - dup2(oldfd, newfd) fa puntare newfd alla stessa open file description di oldfd.
 *   Se newfd era già aperto, viene chiuso prima (operazione atomica). 
 * - E' fondamentale chiudere i file descriptor non usati: se il padre (o un figlio)
 *   lascia aperta l’estremità di scrittura, il lettore non vedrà mai EOF e può
 *   restare bloccato.
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main() {
    int pipefd[2];

    // Creazione della pipe
    if (pipe(pipefd) == -1) {
        perror("pipe");
        exit(EXIT_FAILURE);
    }

    // Primo fork: processo che scrive nella pipe (producer)
    pid_t pid1 = fork();
    if (pid1 < 0) {
        perror("fork");
        exit(EXIT_FAILURE);
    }

    if (pid1 == 0) {
        // Figlio 1: scrive i dati (stdout) nella pipe
        close(pipefd[0]);                // Chiude l'estremità di lettura

        dup2(pipefd[1], STDOUT_FILENO);  // Reindirizza stdout verso la pipe
        close(pipefd[1]);              // Non piu' necessaria

        // Esegue "ls -l"
	//execl("/bin/ls", "ls", "-l", NULL); // vuole un path nel primo argomento (assoluto o relativo) 
        execlp("ls", "ls", "-l", NULL);

        // Se fallisce
        perror("execlp ls");
        exit(EXIT_FAILURE);
    }

    // Secondo fork: processo che legge dalla pipe (consumer)
    pid_t pid2 = fork();
    if (pid2 < 0) {
        perror("fork");
        exit(EXIT_FAILURE);
    }

    if (pid2 == 0) {
        // Figlio 2: legge i dati (stdin) dalla pipe
        close(pipefd[1]);                // Chiude l'estremità di scrittura
        dup2(pipefd[0], STDIN_FILENO);   // Reindirizza stdin dalla pipe
        close(pipefd[0]);                // Non serve piu'

        // Esegue "wc -l"
        execlp("wc", "wc", "-l", NULL);
        // Se execlp fallisce:
        perror("execlp wc");
        exit(EXIT_FAILURE);
    }

    // Processo padre: chiude entrambe le estremità della pipe
    close(pipefd[0]);  
    close(pipefd[1]);  

    // Attende che entrambi i figli terminino
    waitpid(pid1, NULL, 0);
    waitpid(pid2, NULL, 0);

    return 0;
}
