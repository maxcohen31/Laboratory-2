#if !defined(_MONITOR_H)
#define _MONITOR_H

#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>

void prendiBastoncini(int id);
void rilasciaBastoncini(int id);
int  apparecchiaLaTavola(int N);
int  iniziaLaCena(void* (F)(void*));
void fineCena();



#endif /* _MONITOR_H */
