#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>

// what does it print?
int main() {

  printf("The quick brown fox jumped over\n");
  execl("/bin/echo", "/bin/echo", "the", "lazy", "dogs.", NULL);
  perror("execl");
  return -1;


}
