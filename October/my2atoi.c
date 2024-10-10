

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

