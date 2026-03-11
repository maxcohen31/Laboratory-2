// *******************************
// **** Do not run this code! ****
// *******************************

#include <sys/types.h>
#include <unistd.h>

int main() {
  while (fork() >=0)
    ;
  return 0;
}
