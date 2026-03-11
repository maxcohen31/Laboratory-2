#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

// thread function
void* threadF(void* id) {

  long myid= (long)id;

  errno = 100 + myid;   // variabile privata per ogni thread

  printf("Hello from thread %ld\n", myid);
  return (void*)((long)errno);
}

int main(int argc, char* argv[]) {
  if (argc != 2) {
    printf("usa: %s nthreads\n", argv[0]);
    return -1;
  }
  const long nthreads = strtol(argv[1], NULL, 10);
  if (nthreads>100) {
    printf("too many threads, lower nthreads\n");
    return -1;
  }

  pthread_t threadid[nthreads];
  for(long i=0;i<nthreads; ++i) 
    if (pthread_create(&threadid[i], NULL, threadF, (void*)i) != 0) {   // thread_create
      perror("pthread_create");
      return -1;
    }
  
  for(long i=0;i<nthreads; ++i) {
    long *status;
    if (pthread_join(threadid[i], (void**)&status) != 0) {   // thread_join
      perror("pthread_join");
      return -1;
    }
    printf("Thread %ld returned with %ld\n", i, (long)status);
  }
  
  return 0;
}
