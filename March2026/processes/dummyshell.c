#define _POSIX_C_SOURCE 200112L
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>     
#include <errno.h> 
#include <sys/wait.h>  
#include <sys/types.h> 

#define MAX_INPUT_LINE    1024     /* massima lunghezza della stringa del comando */
#define MAX_PARAM          256     /* numero massimo di parametri consentiti per un comando */
#define MALLOC(ptr, sz)                                  \
    do {                                                 \
        (ptr) = (char *)malloc((sz));                    \
        if ((ptr) == NULL) {                             \
            perror("malloc");                            \
            exit(EXIT_FAILURE);                          \
        }                                                \
    } while (0)

static void print_prompt(void) {
    printf("> ");
    fflush(stdout);
}

static void cleanargv(char *argv[]) {
    for (int i = 0; argv[i] != NULL; ++i) {
        free(argv[i]);
        argv[i] = NULL;
    }
}

static void build_args(char *line, char *argv[]) {
    int i = 0;
    char *saveptr = NULL;

    argv[0] = NULL;

    char *token = strtok_r(line, " \t", &saveptr);
    if (!token) return;

    while (token && i < MAX_PARAM - 1) {
        size_t len = strlen(token) + 1;
        MALLOC(argv[i], len);
        memcpy(argv[i], token, len);
        ++i;
        token = strtok_r(NULL, " \t", &saveptr);
    }
    argv[i] = NULL;
}

static int builtin_cd(char *argv[]) {
    /* cd [dir] */
    const char *target = NULL;

    if (argv[1] == NULL) {
        target = getenv("HOME");
        if (!target) {
            fprintf(stderr, "cd: HOME non impostata\n");
            return -1;
        }
    } else if (argv[2] != NULL) {
        fprintf(stderr, "cd: troppi argomenti\n");
        return -1;
    } else {
        target = argv[1];
    }

    if (chdir(target) == -1) {
        fprintf(stderr, "cd: %s: %s\n", target, strerror(errno));
        return -1;
    }
    return 0;
}

int main(void) {
    pid_t pid;
    int status = 0;
    char inputline[MAX_INPUT_LINE];
    char *argv[MAX_PARAM];

    while (1) {
        print_prompt();

        ssize_t n = read(STDIN_FILENO, inputline, MAX_INPUT_LINE - 1);
        if (n == 0) break; /* EOF (Ctrl-D) */
        if (n < 0) {
            if (errno == EINTR) continue;
            perror("read");
            continue;
        }

        inputline[n] = '\0';
        if (n > 0 && inputline[n - 1] == '\n') inputline[n - 1] = '\0';

        build_args(inputline, argv);
        if (argv[0] == NULL) continue;

        if (strcmp(argv[0], "exit") == 0) {
            cleanargv(argv);
            break;
        }

        if (strcmp(argv[0], "cd") == 0) {
            (void)builtin_cd(argv);
            cleanargv(argv);
            continue;
        }

        pid = fork();
        if (pid < 0) {
            perror("fork");
            cleanargv(argv);
            continue;
        }

        if (pid == 0) {
            execvp(argv[0], argv);   

            /* execvp fallita */
            if (errno == ENOENT) {
                fprintf(stderr, "%s: command not found\n", argv[0]);
                _exit(127);
            }
            if (errno == EACCES) {
                fprintf(stderr, "%s: permission denied\n", argv[0]);
                _exit(126);
            }

            perror("execvp");
            _exit(126);
        }

        while (waitpid(pid, &status, 0) < 0) {
            if (errno == EINTR) continue;
            perror("waitpid");
            break;
        }

        if (WIFEXITED(status)) {
            printf("Processo %d terminato con exit(%d)\n", (int)pid, WEXITSTATUS(status));
        } else if (WIFSIGNALED(status)) {
            printf("Processo %d terminato da segnale %d\n", (int)pid, WTERMSIG(status));
        }

        cleanargv(argv);
    }

    printf("shell done ....\n");
    return 0;
}
