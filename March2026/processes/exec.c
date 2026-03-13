#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>

// what does it print?
int main() {
    
    /* The newline '\n' flush the buffer and the printed result is the entire statement */
    printf("The quick brown fox jumped over\n"); 
    /* Replace the current process with /bin/echo */
    execl("/bin/echo", "/bin/echo", "the", "lazy", "dogs.", NULL);
    perror("execl"); // run only if execl fails
    return -1;


}
