/*
    Scrivere una funzione "int mioa2toi(const char *s, int *n)"

    che data una stringa s la interpreta come intero decimale (allo stesso modo di atoi). 
    Se la conversione avviene con successo il valore ottenuto deve essere scritto in *n e la funzione deve restituire il valore 0; 
    se invece la conversione non ha successo la variabile n non deve essere modificata e la funzione deve restituire un intero positivo che indica 
    l'errore che si è verificato secondo il seguente schema:

    se la stringa è vuota o contiene solamente spazi
    se viene incontrato un carattere che non sia uno fra +-0123456789 (esempio, la stringa "234s7")
    se il segno (+ o -) compare più di una volta, o compare in posizione non corretta (esempio le stringhe "+-34", "-3-4", o "-34+")
    se compare esattamente un segno, ma nessuna cifra (ad esempio la stringa "-")
    La conversione deve ignorare eventuali spazi iniziali e deve terminare non appena viene incontrato uno spazio. Quindi l'input "  +34 21" deve restituire il valore 0 
    (conversione OK) e scrivere in *n il valore 34, mentre l'input "  + 34 21"deve restituire il valore 4 
    (conversione fallita perché ha letto il segno e poi lo spazio ha fatto interrompere la conversione prima che incontrasse una qualsiasi cifra.

    Per quanto riguarda la parte di calcolo, a parte la gestione del segno e degli errori, 
    osservate che se l'input è "XYZ" e la stringa "XY" è stata convertita nel valore t allora "XYZ"vale 
    10t +z dove z
    è il valore tra 0 e 9 rappresentato dal carattere Z. Ricordo che i codice ascii di caratteri tra 0 e 9 sono gli interi tra 48 e 57.

    it took me 4 hours...

 */

#define _GNU_SOURCE
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

char *remove_leading_spaces(const char *string)
{
    int c_space = 0;
    char* result = strdup(string);

    // count spaces
    while (result[c_space] == ' ' || result[c_space] == '\t' || result[c_space] == '\n')
    {
        c_space++;
    }

    if (c_space != 0)
    {
        int c = 0;
        while (result[c_space+c] != '\0')
        {
            // shift the string to the left
            // 1st iteration: result[0] = result[0+3] and so on...
            result[c] = result[c_space+c];
            c++;
        }
        result[c] = '\0';
    }
 
    return result;
}

char* remove_after_space(const char* string)
{
    char* result = strdup(string);
    int no_space_char = 0;

    while (result[no_space_char] != ' ' && result[no_space_char] != '\0')
    {
        no_space_char++;
    }

    // trim the string
    if (result[no_space_char == ' '])
    {
        result[no_space_char] = '\0';
    }
  
    return result;

}

int mio2toi(const char *s, int *n)
{
    int count_space = 0;
    char *new_s = remove_leading_spaces(s);
    new_s = remove_after_space(new_s);
    // int converted;

    while (s[count_space] == ' ')
    {
        count_space++;
    }

    // check for empty string
    if (count_space == strlen(s))
    {
        *n = 1;
    }

    
    // check for invalid character
    for (int i = 0; i < strlen(new_s); i++)
    {
        if ((new_s[i] < 48 || new_s[i] > 57) && (new_s[i] != '+' && new_s[i] != '-'))
        {
           *n = 2;
        }
    }

    // check for invalid + or - pos
    int count_s = 0;
    int cypher = 0;
    for (int i = 0; i < strlen(new_s); i++) 
    {
        if (new_s[i] == '+' || new_s[i] == '-')
        {
            count_s++;
        }
    }

    if (count_s >= 2 || (count_s >= 2 && cypher >= 2))
    {
        *n = 3;
    }
    
    if (count_s == 1 && cypher == 0)
    {
        *n = 4;
    }


    return *n;
}


int main(int argc, char *argv[])
{
  for(int i=1;i<argc;i++) {
    int n, e;
    e = mio2toi(argv[i],&n);
    if(e!=0) printf("Errore conversione: %d\n",e);
    else printf("Mia funzione: %d, libreria: %d\n",n,atoi(argv[i]));
  }
  return 0;
} 

