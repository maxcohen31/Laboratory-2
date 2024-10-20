/*
    Convertitore da decimale a esadecimale
*/

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>


char *reverse_string(char* string)
{
    for (int i = 0; i < strlen(string) / 2; i++)
    {
        char tmp = string[i];
        string[i] = string[strlen(string) - i - 1];
        string[strlen(string) - i - 1] = tmp;
    }

    return string;
}

char *converter(int n)
{
    int i = 0;
    char *hex = malloc(20*sizeof(char));
    
    while (n > 0)
    {
        int r = n%16;
        if (r < 10)
        {
            // (53 % 16) = 5
            // 5 + 48 = 53 
            // ASCII 53 = 5
            hex[i] = (char)(r + 48); 
        }
        else
        {
            hex[i] = (char)(r + 55);
        }
        n /= 16;
        i++;
    }

    hex[i] = '\0';
    reverse_string(hex);

    return hex;
}

int main(int argc, char *argv[])
{

    int n;
    printf("Insert a number: ");

    int e = scanf("%d", &n);
    if (e != 1)
    {
        puts("Error scanf");
        exit(1);
    }

    char *h_str = converter(n);
    printf("Converted numb: %s\n", h_str);
    free(h_str);

    return 0;
}
