#include <assert.h>
#include <monitor.h>
#include <util.h>

typedef enum { HOFAME=1,MANGIO,PENSO } stato_t;

static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t* cond = NULL; // array di CV private per ogni filosofo
static pthread_t*      th = NULL;   // array di thread
static stato_t*        S  = NULL;   // array per rappresentare lo stato
static int             NFilosofi = 0; // numero di filosofi a cena


void prendiBastoncini(int id) {
  LOCK(&mutex);
  stato_t left = (id == 0)?(NFilosofi-1):(id-1);
  stato_t right= (id+1) % NFilosofi;

  S[id]=HOFAME;
  while(S[left] == MANGIO || S[right] == MANGIO) {
    WAIT(&cond[id],&mutex);
  }
  S[id]=MANGIO;
  UNLOCK(&mutex);
}

void rilasciaBastoncini(int id) {
  LOCK(&mutex);
  stato_t left   = (id == 0)?(NFilosofi-1):(id-1);
  stato_t right  = (id+1) % NFilosofi;
  int rightright = (right+1) % NFilosofi;
  int leftleft   = (left == 0)?(NFilosofi-1):(left-1);

  S[id] = PENSO;
  if (S[left] == HOFAME && S[leftleft] != MANGIO) {
    S[left] = MANGIO;  
    SIGNAL(&cond[left]);
  }
  if (S[right] == HOFAME && S[rightright] != MANGIO) {
    S[right] = MANGIO;   
    SIGNAL(&cond[right]);
  }
  UNLOCK(&mutex);
}

int apparecchiaLaTavola(int N) {
  NFilosofi = N;
  cond   = malloc(N*sizeof(pthread_cond_t));
  th     = malloc(N*sizeof(pthread_t));
  S      = malloc(N*sizeof(stato_t));    
  if (!cond || !th || !S) {
    fprintf(stderr, "malloc fallita\n");
    return -1;
  }   
  for(int i=0;i<N; ++i) {
    S[i]     = PENSO;
    if (pthread_cond_init(&cond[i], NULL) != 0) {
      fprintf(stderr, "pthread_cond_init\n");
      return -1;
    }
  }

  return 0;
}

int iniziaLaCena(void* (Filosofo)(void*)) {
  for(long i=0; i<NFilosofi; ++i)
    if (pthread_create(&th[i], NULL, Filosofo, (void*)i) != 0) {
      fprintf(stderr, "pthread_create failed\n");
      return -1;
    }
  return 0;
}

void fineCena() {
  for(int i=0;i<NFilosofi; ++i) {
    if (pthread_join(th[i], NULL) == -1) {
      fprintf(stderr, "pthread_join failed\n");
    }
  }
  for(int i=0;i<NFilosofi;++i) {
    pthread_cond_destroy(&cond[i]);
  }
  // cleanup
  free(th);
  free(cond);
  free(S);
}



