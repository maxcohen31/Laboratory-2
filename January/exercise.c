/*
    Un'azienda ha dotato i propri dipendentidi un sensore che emette un codic enumerico ogni volta che un dipendente
    attraversa la porta d'ingresso/uscita dell'aziendao ne transita nelle vicinanze. L'azienda ha meno di 100 dipendenti.
    Ad ogni attraversamento, il sensore registra ora e minuti del passaggio, insieme al codice del dipendente (codice alfanumerico con al max 10 caratteri).
    Si desidera sviluppare un programma in linguagigo C per il calcolo delle ore lavorative dei dipendenti dell'azienda.
    Il programma riceve su linea di comando un primo parametro, che rappresenta il nome del file contenente gli attraversamenti, ed un secondo
    parametro (opzionale), che rappresenta il codice numerico del dipendente.
    Il file è relativo ai passaggi di una sola giornata, ed è composto da una serie di righe, ciascuna delle quali corrisponde ad un passaggio
    ed è composta da 3 campi: ora, minuti e codice dipendente.
    Se il programma viene invocato con due parametri sulla linea di comando (vi è il codice del dipendente), allora dovrà stampare,
    per il dipendente specificato, il totale dei minuti lavorati. Per trovare la quantità di minuti lavorati si dovrà confrontare
    l'orario del primo passaggio e quello dell'ultimo passaggio.
    Se invece il programma viene invocato con un solo parametro sulla linea di comando (è assente il codice del dipendente) allora il programma 
    dovrà restituire il totale dei dipendenti diversi che hanno lavorato in quel giorno (ovvero che sono passati alemeno una volta dalla porta). 
*/

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#define MASSIMA_LUNGHEZZA 10
#define MAX_DIPENDENTI 5


int calcola_tempo_dipendente(FILE* dipendenti, char *codice_input) 
{
    int ore;
    int minuti;
    int ora_inizio = -1;
    int ora_fine = -1;
    int minuti_inizio = -1; 
    int minuti_fine = -1;
    char codice_dipendente[MASSIMA_LUNGHEZZA + 1];

    while (fscanf(dipendenti, "%d %d %s", &ore, &minuti, codice_dipendente) == 3)
    {
        if (strcmp(codice_input, codice_dipendente) == 0)
        {
            if (ora_inizio == -1 && minuti_inizio == -1)
            {
                ora_inizio = ore;
                minuti_inizio = minuti;
            }
            ora_fine = ore;
            minuti_fine = minuti;
        }
    }
    return ((ora_fine - ora_inizio) * 60) + (minuti_fine - minuti_inizio);
}

int calcola_numero_dipendenti(FILE *dipendenti)
{
    int ore;
    int minuti;
    char codice_dipendente[MASSIMA_LUNGHEZZA + 1];
    char *codici[MAX_DIPENDENTI] = {NULL};
    int totale_dipendenti = 0;

    while (fscanf(dipendenti, "%d %d %s", &ore, &minuti, codice_dipendente) == 3)
    {
        bool visto = false;
        for (int i = 0; i < totale_dipendenti; i++)
        {
            if (strcmp(codici[i], codice_dipendente) == 0)
            {
                visto = true;
                break;
            }
        }
        
        if (!visto && totale_dipendenti < MAX_DIPENDENTI)
        {
            codici[totale_dipendenti] = strdup(codice_dipendente);
            totale_dipendenti++;
        }
    }

    for (int i = 0; i < totale_dipendenti; i++)
    {
        free(codici[i]);
    }

    return totale_dipendenti;
}

int main(int argc, char **argv)
{
    FILE *f = fopen("passaggi.txt", "r");
    if (argc != 2 || argc != 3) 
    {
        printf("Uso: nomefile file chiave(opzionale)");
    }
       
    if (argc == 2)
    {
        int t = calcola_numero_dipendenti(f);
        printf("Il numeri di dipendenti è %d", t);
    }
    if (argc == 3)
    {
        int tempo = calcola_tempo_dipendente(f, argv[2]);
        printf("Il dipendente %s ha lavorato per %d minuti", argv[2], tempo);
    }

    fclose(f);
    return 0;
}
