/*
 * redirect.c
 *
 * Uso:
 *   redirect OUTFILE comando [arg1 arg2 ...]
 *
 * Esempio:
 *   redirect out.txt ls -l
 *
 * Cosa fa:
 * 1) apre (o crea) OUTFILE
 * 2) dup2(fd, STDOUT_FILENO) per far puntare stdout al file
 * 3) execvp(comando, argv_del_comando)
 *
 * Nota: Solo stdout viene rediretto. stderr resta sul terminale.
 */

#define _POSIX_C_SOURCE 200112L
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
    if (argc < 3) {
        fprintf(stderr, "Uso: %s OUTFILE comando [arg...]\n", argv[0]);
        fprintf(stderr, "Esempio: %s out.txt ls -l\n", argv[0]);
        return 2;
    }

    const char *outfile = argv[1];
    char **cmd_argv = &argv[2];          /* cmd_argv[0] = comando */

    /* Apri o crea il file, sovrascrive se esiste gia' */
    int fd = open(outfile, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd == -1) {
        fprintf(stderr, "open(%s): %s\n", outfile, strerror(errno));
        return 1;
    }

    /* Redirigi stdout (fd 1) sul file */
    if (dup2(fd, STDOUT_FILENO) == -1) {
        perror("dup2");
        close(fd);
        return 1;
    }

    /* Non serve piu' tenere fd aperto, stdout ora punta al file */
    close(fd);

    /* Esegui il comando */
    execvp(cmd_argv[0], cmd_argv);

    /* Se siamo qui, execvp ha fallito */
    if (errno == ENOENT) {
        fprintf(stderr, "%s: command not found\n", cmd_argv[0]);
        return 127;
    }
    if (errno == EACCES) {
        fprintf(stderr, "%s: permission denied\n", cmd_argv[0]);
        return 126;
    }
    perror("execvp");
    return 126;
}
