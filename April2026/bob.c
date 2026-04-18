#include "xerrori.h"
#define QUI __LINE__,__FILE__

/* Bob is the child process */

/* Make a move */
int make_move(int n)
{
    if (n % 2 == 0) return n / 2;
    return n - 1;
}

bool is_valid_move(int prev, int received)
{
    if (prev % 2 == 0) return (received == prev / 2) || (received == prev - 1);
    else return (received == prev - 1);
}

int main(int argc, char **argv)
{
    int alice_pid = getppid();
    /* Blocking SIGRTMIN */
    sigset_t set;
    sigemptyset(&set);
    sigaddset(&set, SIGRTMIN);

    if (sigprocmask(SIG_BLOCK, &set, NULL) == -1) xtermina("sigprocmask() error", QUI);

    union sigval sv;
    siginfo_t siginfo;
    
    /* Waiting for Alice to send N */
    while (true)
    {
        int e = sigwaitinfo(&set, &siginfo);
        if (e < 0) perror("sigwaitinfo() error");

        int n = siginfo.si_value.sival_int;
        if (n == -1)
        {
            printf("Termination signal received. Exit...\n");
            break;
        }

        printf("[B] Bob receives %d\n", n);

        /* Bob's move */
        int m = make_move(n);
        printf("[B] Bob sends %d\n", m);
        sv.sival_int = m;
        if (sigqueue(alice_pid, SIGRTMIN, sv) == -1) xtermina("sigqueue() error", QUI);
        
        if (m == 0)
        {
            printf("[B] Bob wins!\n");
            break;
        }
        int prev = m;

        /* Alternate moves */
        while (true)
        {
            /* Wait for Alice to make a move */
            if (sigwaitinfo(&set, &siginfo) == -1) xtermina("sigwaitinfo() error", QUI);
            int got = siginfo.si_value.sival_int;

            /* Is a valid move? */
            if (!(is_valid_move(prev, got)))
            {
                fprintf(stderr, "[B] Invalid move from Alice!\n");
                exit(1);
            }

            if (got == 0)
            {
                printf("[B] Alice wins!\n");
                break;
            }

            printf("[B] Bob receives %d\n", got);

            /* Bob's reply */
            m = make_move(got);
            prev = m;
            printf("[B] Bob sends %d\n", m);
            sv.sival_int = m;
            if (sigqueue(alice_pid, SIGRTMIN, sv) == -1) xtermina("sigqueue() error", QUI);
            if (m == 0)
            {
                printf("[B] Bob wins!\n");
                continue;
            }
        }
    }

    return 0;
}
