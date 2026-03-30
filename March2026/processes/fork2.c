#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>

/* che numero stampa? 66 poi newline perché il processo padre termina per ultimo */

int main() {
  int val = 5;
  pid_t pid;
  if ((pid=fork()) == 0) wait(NULL); /* non bloccante - nessun processo aspetta */
  val++;

  printf("%d", val);

  if (pid) printf("\n");
  return 0;
}
