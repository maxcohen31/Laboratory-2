/* Crea N processi zombie.
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>

int main(int argc, char *argv[]) {
    if (argc != 2) {
	fprintf(stderr, "usa: %s N\n", argv[0]);
	return -1;
    }

    errno = 0;
    char *end = NULL;
    long n = strtol(argv[1], &end, 10);
    if (errno != 0 || end == argv[1] || *end != '\0' || n < 0 || n > 100) {
      fprintf(stderr, "N deve essere un intero tra 0 e 100\n");
      return -1;
    }
    for(int i=0;i<n; ++i) {
      pid_t p = fork();
      if (p == 0) _exit(0);
      if (p < 0) {
	perror("fork");
	break;
      }
    }
    
    printf("fra 40s il programma terminera'\n");
    sleep(40);
    return 0;
}
