#include "xerrori.h"
#include <stdio.h>
#include <unistd.h>

/*
    Modificando scrittore.c scrivere un programma xscrivi.out che invocato dalla linea di comando scrivendo
    
    Vedere anche xlettore.py

    xscrivi.out nome i1 i2 i3 .. ik
    esegue le seguenti azioni:
    
    scrive su stdout il valore argc
    crea la named pipe nome nel caso non esista
    se argc>3 crea un nuovo processo con fork e gli fa eseguire una nuova istanza del programma xscrivi.out 
    passandogli come argomento i primi argc-1 parametri (vedi esempio sotto)
    apre la pipe nome in scrittura e scrive su nome tutti gli interi tra 1 e ik
    visualizza su stdout la somma degli interi scritti nella pipe, chiude la pipe e termina.
    Esempio: se si invoca
    
    xscrivi.out tubo 10 15
    il programma deve:
    
    visualizzare il valore 4 (argc),
    creare la pipe tubo,
    lanciare l'esecuzione di una nuova istanza di xscrivi.out con parametri tubo 10
    scrivere su tubo gli interi tra 1 e 15, visualizzare la somma 120 e terminare.
    La nuova istanza di xscrivi.out (che riceve i parametri tubo 10) deve:
    
    visualizzare il valore 3 (il suo argc)
    creare la pipe tubo (probabilmente la troverà già esistente)
    dato che argc==3 non deve creare altri processi
    scrivere su tubo gli interi tra 1 e 10, visualizzare la somma 55 e terminare.
    Per lanciare la nuova versione di xscrivi.out dopo la fork dovete usare la funzione execv, 
    non potete usare la execl fatta a lezione. Guardare la pagina man per i dettagli.

    Per la lettura dei dati scritti nella pipe, modificare lo script lettore.py 
    in modo che al termine stampi la somma di tutti gli interi letti dalla pipe; 
    nell'esempio di sopra dovrebbe stampare 175 (somma di 120 + 55). 
    Per evitare affollamento sul terminale, fare partire lettore.py da un'altra finestra. 
*/

#define QUI __LINE__, __FILE__

int main(int argc,char **argv)
{
    if (argc < 3)
    {
        printf("Usage:\t%s pipe_name i1 i2 ... ik\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    printf("%d\n", argc);

    /* Create the pipe used to communicate */
    int e = mkfifo(argv[1], 0660);
    if (e == 0) puts("Pipe succesfully created\n");
    else if (errno == EEXIST) puts("Pipe already exists\n");
    else xtermina("Error creating the pipe\n", QUI);
 
    if (argc > 3)
    {
        pid_t pid = xfork(QUI);
        if (pid == 0) /* Child */
        {
            /* We don't know how many arguments are going to be passed.
             * We need to allocate memory for each of them */
            char **arguments = malloc(argc * sizeof(char*));
            if (arguments == NULL) xtermina("Malloc failed", QUI);     
            /* Fill up our arguments array */
            arguments[0] = argv[0];
            for (int i = 1; i < argc-1; i++) arguments[i] = argv[i];
            /* Inserting NULL at the end of the array */
            arguments[argc-1] = NULL;
            if (execv(argv[0], arguments) == -1)
            {
                xtermina("execv error", QUI);
            }
        }
    } 

    int end = atoi(argv[argc-1]);
    int s = 0; /* counter */
    /*Open file descriptor */
    int fd = open(argv[1], O_WRONLY);
    if (fd < 0) xtermina("Error opening named pipe", QUI);
    /* Write integers into the pipe */
    for (int i = 0; i <= end ; i++)
    {
        ssize_t e = write(fd, &i, sizeof(i));
        if (e != sizeof(i)) xtermina("Error writing into the pipe", QUI);
        s += i;
    }

    printf("[%2d] Process -> sum: %d\n", getpid()%100, s);
    printf("[%2d] xscrittore done\n", getpid()%100);
    xclose(fd, QUI);
       
    return 0;
}
