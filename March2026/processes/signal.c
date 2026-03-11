// sigint.c
// Esempio minimale: gestione di SIGINT (Ctrl+C) con flag atomico.

#include <signal.h>
#include <stdio.h>
#include <unistd.h>

static volatile sig_atomic_t stop = 0;

// Dentro un signal handler bisogna fare pochissimo:
// evitare printf/malloc ecc, in generale possono essere fatte solo
// chiamate async-signal-safe. Qui ci limitiamo a settare un flag di tipo
// sig_atomic_t, che è l’approccio tipico e sicuro
//
static void on_sigint(int signo) {
  (void)signo; // evita warning di parametro non usato
  stop = 1;    // operazione sicura dentro un signal handler
}

int main(void) {
  // Collega SIGINT al nostro handler
  //
  // Nota: per installare un signal handler è preferibile usare
  // sigaction() invece di signal(). sigaction() è lo standard POSIX,
  // ha una semantica più prevedibile tra sistemi, permette di impostare
  // una signal mask durante l’esecuzione dell’handler e flag utili
  // (come ad esempio, SA_RESTART), riducendo comportamenti dalla semantica
  // più obsoleta di signal()
  if (signal(SIGINT, on_sigint) == SIG_ERR) {
    perror("signal");
    return -1;
  }
  
  printf("PID=%ld. Premi Ctrl+C per fermare.\n", (long)getpid());
  int r;
  while (!stop) {
    r=write(STDOUT_FILENO, ".", 1);
    (void)r;
    sleep(1);
  }  
  r=write(STDOUT_FILENO, "\nRicevuto SIGINT, esco.\n", 24);
  return r;
}
