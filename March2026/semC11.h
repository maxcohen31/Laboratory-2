/* Implementation of POSIX semaphores using C11 threads library */

#include <threads.h>


/* Semaphore struct */
typedef struct 
{
    int val;
    mtx_t mutex;
    cnd_t cond;
} sem11_t;

/* Semaphore initialization */
int sem11_init(sem11_t *sem, int v)
{
    if (v < 0) return -1;
    sem->val = v;
    
    if (mtx_init(&sem->mutex, mtx_plain) != thrd_success)
    {
        mtx_destroy(&sem->mutex);
        return -1;
    }
    if (cnd_init(&sem->cond) != thrd_success) 
    {
        mtx_destroy(&sem->mutex);
        return -1;
    }
    return 0;
}

/* Wait (decrement) the semaphore */
int sem11_wait(sem11_t *sem)
{
    mtx_lock(&sem->mutex);
    while (sem->val <= 0) cnd_wait(&sem->cond, &sem->mutex);
    sem->val--;
    mtx_unlock(&sem->mutex);
    return 0;
}

int sem11_post(sem11_t *sem)
{
    mtx_lock(&sem->mutex);
    sem->val++;
    cnd_signal(&sem->cond);
    mtx_unlock(&sem->mutex);
    return 0;
}

/* Destroy the semaphore */
int sem11_destroy(sem11_t *sem)
{
    mtx_destroy(&sem->mutex);
    cnd_destroy(&sem->cond);
    return 0;
}
