#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>

// quanti pid distinti vengono stampati?

int main() {

  fork();
  printf("Pid %d\n", getpid());
  fork();
  printf("Pid %d\n", getpid());

  return 0;
}
