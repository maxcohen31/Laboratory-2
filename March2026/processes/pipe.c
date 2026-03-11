//
// Esempio base di utilizzo di una comuncazione IPC con pipe senza nome.
//
// 1) Il padre crea una pipe: ottiene due file descriptor, uno per leggere (entry 0) e uno per scrivere (entry 1).
// 2) Fa fork(): il figlio eredita la tabella dei file descriptor aperti del padre (la tabella viene copiata).
// 3) Si decide una direzione: qui il padre scrive, il figlio legge, ma potrebbe anche essere al contrario.
// 4) Si chiudono gli estremi non usati in ciascun processo, altrimenti il lettore può non vedere mai EOF.
//
// Nota chiave su EOF su pipe:
// read() ritorna 0 (EOF) SOLO quando TUTTI gli estremi di scrittura della pipe sono stati chiusi
// in TUTTI i processi che li hanno aperti. Se anche solo un processo tiene aperto canale[1],
// il lettore può rimanere bloccato in read() aspettando nuovi dati.
//
//
#define _GNU_SOURCE
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <limits.h>
#include <errno.h>

int main(int argc, char *argv[]) {    
    int canale[2];
    
    if (pipe(canale) == -1) {
      perror("pipe");
      return -1;
    }    
    pid_t pid;
    pid = fork();
    if (pid == -1) { perror("fork"); return -1; }
    if (pid == 0) {

      // se commentiamo la seguente linea di codice, il programma
      // non termina. Perche'?
      close(canale[1]);
      
      while(1) {
	char buf[PIPE_BUF];

	int r= read(canale[0], buf, sizeof(buf));
	if (r==-1) {
	  perror("read");
	  return -1;
	}
	if (r==0) break;  // EOF

	fprintf(stdout, "msg (%d): %s", r, buf);
      }      
      return 0;
    }
    const char msg1[] = "ciao mondo!\n";
    if (write(canale[1], msg1, strlen(msg1)+1) == -1) {
      perror("write");
      return -1;
    }

    // aspetto un po' prima di mandare il messaggio
    sleep(2);

    const char msg2[] = "Goodbye!\n";
    if (write(canale[1], msg2, strlen(msg2)+1) == -1) {
      perror("write");
      return -1;
    }
    
    // se commentiamo la seguente linea di codice, il programma
    // non termina. Perche'?
    close(canale[1]);
    
    if (waitpid(pid, NULL, 0) == -1) {
      perror("waitpid");
      return -1;
    }
    return 0;
}
