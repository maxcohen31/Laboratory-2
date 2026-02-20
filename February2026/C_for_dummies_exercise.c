/*
    Exercise: Pulling Numbers from a String 
    Blog: C for dummies.
    
    Consider the following string:

    abc10=13!260;1m

    This data could be a command or perhaps a report. Regardless, the parts you want are the values: 10, 13, 260, and 1. 
    I remember performing such a programming task on directory listings when I first learned to code. 
    But the advantage of a directory listing is that the columns are distinct and predictable.
    Therefore, the code relies upon data being available at specific offsets.
    Easy. Such consistency shouldn’t be assumed for this example.

    Your task is to write a function, extract().
    Its job is to locate the first bit of numerical data in a string. 
    For example, if you call extract() on the above string, it returns the location (as a pointer) of the 1 after abc.
    This address is then used to convert the characters 10 into an integer. (Converting the string into an integer isn’t part of the extract() function.)

    But wait! There’s more . . .

    Your task for this exercise isn’t just to yank out the first number found and convert it into a string.
    No, you must code the extract() function so that it can be called repeatedly,
    each time extracting the next value from the same string until it’s exhausted all possible values. 
    The sample output from my solution reflects this condition:

    10
    13
    260
    1

    Remember, the extract() function doesn’t know where a number exists within the string. 
    Its job is to return the address of the first digit found, then to be called again and again to find subsequent digits.
    When the string is exhausted, the function returns NULL.
*/

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *extract(char *s)
{

    /* We need to remember where we left off */
    static char *first_occurence = NULL; 
    if (s != NULL) first_occurence = s;
    if (first_occurence == NULL) return NULL;

    /* Find the first digit */
    while (*first_occurence && !(isdigit(*first_occurence))) first_occurence++; 
    /* Found null terminator */
    if (*first_occurence == 0) return NULL;
    /* start of our number */
    char *start = first_occurence;
    /* Search for other digits */
    while(isdigit(*first_occurence)) first_occurence++;

    return start;
}

int main(int argc, char **argv)
{
    char *input_string = "abc10=13!260;1m";
    char *r = extract(input_string); 
    while (r != NULL)
    {
        printf("%d\n", atoi(r));
        r = extract(NULL);
    }
    return 0;
}



