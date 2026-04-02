/*
 * Program for counting primes in certain intervals by using auxiliary processes
 * and a pipe to communicate the partial sums to the parent process. */

#include "xerrori.h"

#define QUI __LINE__,__FILE__

/* Check whether a number is prime or no */
bool is_prime(int n)
{
    if (n < 2) return false;
    if (n % 2 == 0)
    {
        if (n == 2) return true;
        return false;
    }
    for (int i = 3; i*i <= n; i += 2)
    {
        if (n % i == 0) return false;
    }
    return true;
}

/* Count primes in [a, b) */
int count_primes(int a, int b)
{
    int res = 0;
    for (int i = a; i < b; i++)
    {
        if (is_prime(i)) res++;
    }
    return res;
}

/* Count how many primes there are in the given range [argv[1], argv[2]) */
int main(int argc, char **argv)
{
    if (argc != 4)
    {
        printf("Usage:\t%s n1 n2 p\n", argv[0]);
        exit(1);
    }

    int n0 = atoi(argv[1]);
    int n1 = atoi(argv[2]);
    int p = atoi(argv[3]);
    assert(n0 > 0 && n1 > n0 && p > 0);

    /* Pipe for children to communicate to parent */
    int up[2]; /* Up: the stream is from the children to the parent */
    xpipe(up, QUI);
    /* Creating children */
    for (int i = 0; i < p; i++)
    {
        pid_t pid = xfork(QUI);
        if (pid == 0)
        {
            xclose(up[0], QUI);
            /* Child calculates the range he has to manage */
            int n = (n1 - n0) / p;
            int start = n0 + n * i;
            int end = (i == p - 1) ? n1 : n0 + n * (i+1);
            int tot = count_primes(start, end);
            printf("Child %d: range %d %d, found: %d primes\n", i, start, end, tot);
            ssize_t e = write(up[1], &tot, sizeof(tot));
            if (e != sizeof(int)) termina("Error writing the pipe\n");
            xclose(up[1], QUI);
            exit(0);
        }
    }

    /* Parent */
    int parent_tot = 0;
    /* Read until up[1] is closed */
    while (true)
    {
        int x;
        ssize_t e = read(up[0], &x, sizeof(x));
        if (e == 0) break;
        printf("Read %d from the pipe\n", x);
        parent_tot += x;
    }
    xclose(up[0], QUI);
    printf("Primes p in range %d <= p < %d : %d", n0, n1, parent_tot);
    return 0;
}

