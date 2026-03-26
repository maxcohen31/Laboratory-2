// garatisce l'ordine FIFO
//
#define _POSIX_C_SOURCE 200112L
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>

#include <queue.h>

static int  N;
static int  stop;
static unsigned long t0; // tempo iniziale

static inline unsigned long getusec() {
    struct timeval tv;
    gettimeofday(&tv,NULL);
    return (unsigned long)(tv.tv_sec*1e6+tv.tv_usec);
}

static pthread_mutex_t mutex    = PTHREAD_MUTEX_INITIALIZER;
static int activeReaders = 0;
static int state = -1;  // -1 next-one/initial,  0 lettura,  1 scrittura 

typedef struct _ordering {
  pthread_cond_t ord;
  int rw;   // reader 0, writer 1
} ordering_t;

Queue_t* orderingQ;

void startRead(int id) {
  pthread_mutex_lock(&mutex);
  // c'e' uno scrittore oppure c'e' qualcuno in coda
  if ( top(orderingQ)!= NULL || state > 0 ) { 
    ordering_t* o = malloc(sizeof(ordering_t));
    assert(o);
    
    pthread_cond_init(&o->ord, NULL);
    o->rw = 0;    
    if (push(orderingQ, o)<0) abort();

    do 
      pthread_cond_wait(&o->ord, &mutex);
    while(state > 0);    // finche' ci sono scrittori aspetto

    assert(state == -1 || state == 0);
    ordering_t* otmp = pop(orderingQ);
    assert(o == otmp);
    pthread_cond_destroy(&o->ord);
    free(o);

    o = top(orderingQ);
    if (o && o->rw == 0) { // c'e' un altro lettore in attesa dopo di me
      pthread_cond_signal(&o->ord);
    }    
  }
  activeReaders++;
  state = 0;  // lettura
  pthread_mutex_unlock(&mutex);
}
void doneRead(int id) {
  pthread_mutex_lock(&mutex);
  activeReaders--;
  assert(activeReaders>=0);
  if (activeReaders==0) { // sono l'ultimo lettore
    state = -1; // resetto lo stato 
    if (length(orderingQ)>0) { //  c'e' uno scrittore in coda lo faccio partire
      ordering_t* o = top(orderingQ);
      assert(o && o->rw==1);
      pthread_cond_signal(&o->ord);
    } 
  }
  pthread_mutex_unlock(&mutex);
}

void startWrite(int id) {
  pthread_mutex_lock(&mutex);
  if (state >= 0) {  // c'e' uno scrittore oppure dei lettori
    ordering_t* o = malloc(sizeof(ordering_t));
    pthread_cond_init(&o->ord, NULL);
    o->rw = 1;
    if (push(orderingQ, o)<0) abort();

    do {
      pthread_cond_wait(&o->ord, &mutex);
      // vengo svegliato con stato=-1 per avere precedenza su eventuali lettori che arrivano prima che riprendo la lock
    } while(state >= 0);    
    assert(state == -1);
    ordering_t* otmp = pop(orderingQ);
    assert(o == otmp);
    pthread_cond_destroy(&o->ord);
    free(o);
  }
  state = 1;  // scrittura
  pthread_mutex_unlock(&mutex);
}
void doneWriter(int id) {
  pthread_mutex_lock(&mutex);
  assert(state == 1);

  // setto lo stato ad un valore -1 per non far passare avanti
  // lettori che potrebbero arrivare e trovano la mutex libera
  // anche se ci sono lettori che si stanno svegliando .....
  state = -1;

  ordering_t* o = top(orderingQ); 
  if (o!=NULL) // c'e' qualcuno in coda
    pthread_cond_signal(&o->ord);
  pthread_mutex_unlock(&mutex);
}

void work(long us) {
  struct timespec t={0, us*1000};
  nanosleep(&t, NULL);
}

void *Reader(void* arg) {
  long id = (long)arg;
  while(stop>0) {
    startRead(id);
    
    printf("READER%ld ENTRATO in sezione critica t=%.2f\n",id, (getusec()-t0)/1000.0);
    work(2000);
    printf("READER%ld USCITO dalla sezione critica\n",id);
    
    doneRead(id);
  }
  printf("READER%ld TERMINATO\n", id);  
  return NULL;
}

void *Writer(void* arg) {
  long id = (long)arg;

  for(int i=0;i<N;++i) {
    startWrite(id);

    printf("WRITER%ld ENTRATO in sezione critica t=%.2f\n",id, (getusec()-t0)/1000.0);
    work(6000);
    printf("WRITER%ld USCITO dalla sezione critica\n",id);

    if (i+1 == N) --stop;
    
    doneWriter(id);
  }
  printf("WRITER TERMINATO\n");
  
  return NULL;
}

int main(int argc, char* argv[]) {
  int R = 5;
  int W = 2;
  N = 100;
  if (argc > 1) {
    if (argc != 4) {
      fprintf(stderr, "usage: %s [#R #W N]\n", argv[0]);
      return -1;
    }
    R = atoi(argv[1]);
    W = atoi(argv[2]);
    N = atoi(argv[3]);
  }  
  // a reasonable limit 
  if (R>100) R = 100;
  if (W>100) W = 100;

  stop = W;
  orderingQ = initQueue();
  t0   = getusec();
  
  pthread_t* readers     = malloc(R*sizeof(pthread_t));
  pthread_t* writers     = malloc(W*sizeof(pthread_t));
  if (!readers || !writers) {
    fprintf(stderr, "not enough memory\n");
    return -1;
  }

  
  for(long i=0; i<W; ++i)
    if (pthread_create(&writers[i], NULL, Writer, (void*)i) != 0) {
      fprintf(stderr, "pthread_create Writer failed\n");
      return -1;
    }
  for(long i=0; i<R; ++i)
    if (pthread_create(&readers[i], NULL, Reader, (void*)i) != 0) {
      fprintf(stderr, "pthread_create Reader failed\n");
      return -1;
    }		
  for(long i=0;i<R; ++i)
    if (pthread_join(readers[i], NULL) == -1) {
      fprintf(stderr, "pthread_join failed\n");
    }
  for(long i=0;i<W; ++i)
    if (pthread_join(writers[i], NULL) == -1) {
      fprintf(stderr, "pthread_join failed\n");
    }
  
  free(writers);
  free(readers);
  deleteQueue(orderingQ);
  return 0;   
  
}
