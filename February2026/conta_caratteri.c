/* 
 * Scrivere un programma C contaCaratteri2.c 
 * che crea 2 thread il primo thread cerca il carattere nella prima parte del file, 
 * il secondo thread nella seconda parte */

#include "xerrori.h"

#define THREAD_NUM 2

typedef struct 
{
    char *file_name;
    long start;
    long end;
    int char_sum;
    char c;
} data;

void *tbody(void *arg)
{
    data *dt = (data*)arg;
    int sum = 0;                                    
    int ch;
    
    FILE *fl = fopen(dt->file_name, "r");
    if (fl == NULL) pthread_exit(0);
    printf("Thread reading from %ld to %ld byte\n", dt->start, dt->end);
    fseek(fl, dt->start, SEEK_SET);
    for (long i = dt->start; i < dt->end; i++)
    {
        ch = getc(fl);
        if (ch == EOF) break;
        if (ch == dt->c) sum++;
    }
    dt->char_sum = sum;
    printf("Thread starting from byte %ld has finished\n", dt->start);

    fclose(fl);
    pthread_exit(0);
}

int main(int argc, char **argv)
{

    if (argc != 3) 
    {
        fprintf(stderr, "Usage: <executable> <filename> <character> \n");
        exit(1);
    }
    
    /* Get mid point */
    FILE *f = fopen(argv[1], "r");
    if (f == NULL) 
    {
        termina("Error opening file");
        exit(3);
    } 
    fseek(f, 0, SEEK_END);
    long size = ftell(f);
    long mid = size / 2;
    fclose(f);
    
    data data_threads[THREAD_NUM];
    pthread_t arr[THREAD_NUM];
    int result = 0;

    for (int i = 0; i < THREAD_NUM; i++)
    {
        data_threads[i].file_name = argv[1];
        data_threads[i].start = (i == 0) ? 0 : mid;
        data_threads[i].end = (i == 1) ? size : mid;
        data_threads[i].c = argv[2][0];
        data_threads[i].char_sum = result;
        xpthread_create(&arr[i], NULL, &tbody, &data_threads[i], __LINE__, __FILE__);
    }
    
    for (int i = 0; i < THREAD_NUM; i++)
    {
        xpthread_join(arr[i], NULL, __LINE__, __FILE__);
        result += data_threads[i].char_sum;
    }

    printf("Total character '%c' found in file: %d", argv[2][0], result);
    return 0;
}
