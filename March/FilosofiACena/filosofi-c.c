#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>
#include <stdlib.h>
#include <errno.h>
#include <stdatomic.h>

#include <monitor.h>


static long ITER = 2000;  // durata della cena

// utility function
static inline long atomicDec(long *v) {  
  long r;
#if 0 // con mutex
  static pthread_mutex_t iter_lock = PTHREAD_MUTEX_INITIALIZER;  
  pthread_mutex_lock(&iter_lock);
  r = --(*v);
  pthread_mutex_unlock(&iter_lock);
#else // con operazione atomica
  r = atomic_fetch_sub(v, 1);
#endif
  return r;
}

void Mangia(int myid, unsigned int *seed) {
  fprintf(stdout, "%d -- Mangio\n", myid);
  long r = rand_r(seed) % 800000;
  // per perdere un po' di tempo si puo'
  // usare anche il seguente ciclo for
  // for(volatile long i=0;i<r; ++i);

  struct timespec t={0, r};
  nanosleep(&t,NULL);
  fprintf(stdout, "%d -- Finito di mangiare\n", myid);
}
void Pensa(int myid, unsigned int *seed) {
  fprintf(stdout, "%d -- Penso\n", myid);
  long r = rand_r(seed) % 1000000;
  //for(volatile long i=0;i<r; ++i);

  struct timespec t={0, r};
  nanosleep(&t,NULL);
  fprintf(stdout, "%d -- Finito di pensare\n", myid);
}


// thread filosofo
void *Filosofo(void *arg) {
  int myid = (long)arg;
  // un seed diverso per ogni filosofo
  unsigned int seed = myid * time(NULL);

  int mangia = 0;
  int pensa  = 0;
  
  for( ;; ) {
    if (atomicDec(&ITER) <= 0) break;
    
    Pensa(myid, &seed);    
    ++pensa;
    prendiBastoncini(myid);
    Mangia(myid, &seed);
    ++mangia;
    rilasciaBastoncini(myid);
  }
  fprintf(stdout, "Filosofo %d:  ho mangiato %d volte e pensato %d volte\n", myid, mangia, pensa);
  fflush(stdout);
  return NULL;
}

int main(int argc, char *argv[]) { 
  int N = 5;
  if (argc > 1) {
    N = atoi(argv[1]);
    if (N > 100) {
      fprintf(stderr, "N ridotto a 100\n");
      N=100;
    }
  }

  if (apparecchiaLaTavola(N)<0) {  // tavola con N filosofi
    exit(EXIT_FAILURE);
  }
  if (iniziaLaCena(Filosofo)<0) { // ogni filosofo esegue la funzione Filosofo
    exit(EXIT_FAILURE);
  }

  fineCena();
  return 0;   
}
