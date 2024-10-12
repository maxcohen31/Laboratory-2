/*
    Scrivere un programma C, che legge dalla linea di comando il nome di un file di testo contenente interi e stampa su stdout la somma degli interi compresi tra 1 e 1000 
    che non compaiono nell'array. 
    Ad esempio, se il file di input contiene tutti gli interi tra 1 e 1000 tranne il 13, il 17, e il 49 il valore che deve essere stampato è 79 (la somma 13+17+49). 
    Si noti che tutti i valori nel file non compresi tra 1 e 1000 non influenzano il risultato, e che il fatto che un valore compaia più volte non influenza il risultato: 
    a noi interessa la somma dei valori tra 1 e 1000 che non sono nel file di input. 
    (Suggerimento: per svolgere l'esercizio non è necessario salvare gli interi del file in un array, 
    è invece necessario utilizzare un array di 1000 elementi per tenere conto di quali valori tra 1 e 1000 sono presenti nel file).


 */

#define _GNU_SOURCE
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>

FILE *make_file(char *text)
 {
     FILE* integers = fopen(text, "w");
     if (integers == NULL)
     {
         fprintf(stderr, "Error opening file\n");
     }
 
     for (int i = 1; i <= 1000; i++)
     {
         if (i == 13 || i == 17 || i == 49)
         {
             continue;
         }
         fprintf(integers, "%d\n", i);
     }
 
     fclose(integers);
 
     return integers;
 }
 
int find_sum(FILE* f)
{

    int n;
    int sum = 0;
    bool a[1001] = {false};
    
    while (fscanf(f, "%d", &n) != EOF)    
    {
        if (n >= 1 && n <= 1000)
        {
            a[n] = true; 
        }
    }


    for (int i = 1; i < 1000; i++)
    {
        if (!a[i])
        {
            sum += i;
        }
    }

    return sum;

}


int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        fprintf(stderr,"Not enough arguments");
    }

    make_file(argv[1]);
    FILE* f = fopen(argv[1], "r");
    
    if (f == NULL)
    {
        fprintf(stderr, "Error opening file\n");
        exit(EXIT_FAILURE);
    }
    int s = find_sum(f);

    printf("S: %d", s);

    fclose(f);

    return 0;
}
