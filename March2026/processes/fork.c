#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>

/* quanti pid distinti vengono stampati? 
 * La prima fork crea un processo P1. Quindi abbiamo P0 (padre) e il figlio P1.
 * P0 e P1 stampano il proprio pid. 
 * Adesso entrambi i processi chiamano fork(): abbiamo 4 distinti Pid. 
 * Tutti e quattro i processi chiamano printf. */

int main() {

  fork();
  printf("Pid %d\n", getpid());
  fork();
  printf("Pid %d\n", getpid());

  return 0;
}
