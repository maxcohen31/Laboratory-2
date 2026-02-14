/*
    Scrivere un programma abr.c che legge dalla linea di comando il nome di un file di testo e partendo da un ABR vuoto esegue le seguenti operazioni su tutte le linee del file:

    - utilizzando la funzione getline legge la linea copiandola in un buffer b
    - esegue b[strlen(b)-1]='\0' per cancellare il carattere \n in fondo alla linea
    - utilizzando la funzione strtok tokenizza una copia della linea rispetto al separatore " " (spazio) e salva una copia del secondo token in una stringa z
    - aggiunge all'ABR un nuovo node n con z come campo chiave e b come campo linea
    
    Casi particolari:
    
    se il nuovo node è uguale ad un node già presente nell'albero non deve essere inserito ma semplicemente deallocato e il programma deve stampare un messaggio su stderr
    se non esiste il secondo token. come campo chiave si deve usare la stringa "MANCA".
 */

#define _GNU_SOURCE
#include <assert.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

/* Data structure -*/
typedef struct node 
{
    char *chiave;
    char *linea;
    struct node*left;
    struct node *right; 
} node;

node *crea_node(char *s1, char *s2)
{
    node *a = malloc(sizeof(*a));
    a->chiave = strdup(s1);
    a->linea = strdup(s2);
    a->left = NULL;
    a->right = NULL;
    return a;
}

/* Free a node */
void destroy_node(node *a)
{
    if (a != NULL)
    {
        destroy_node(a->left);
        destroy_node(a->right);
        free(a->chiave);
        free(a->linea);
        free(a);
    }
}

void node_stampa(const node *a, FILE *f) 
{
  fprintf(f,"<%-14s> <%s>\n", a->chiave, a->linea);
}

/* Reverse in-order traversal */
void print_abr(node *r)
{
    if (r == NULL) return;
    print_abr(r->right);
    node_stampa(r, stdout);
    print_abr(r->left);
}

/* Take two nodes and compare thier keys. 
 * If the keys are equal then compares their lines */
int compare_nodes(node *a, node *b)
{
    int res = strcmp(a->chiave, b->chiave);
    if (res != 0) return res;
    return strcmp(a->linea, b->linea);
}

/* Insert a node into our BST */
node *insert(node *root, node *a)
{
    assert(a != NULL);
    a->left = NULL;
    a->right = NULL;
    if (root == NULL)
    {
        return a;
    }
    int res = compare_nodes(root, a);
    // If the keys are the same, we need to sort the nodes by comparing the lines
    if (res == 0) 
    {
        node_stampa(a, stderr);
        destroy_node(a);
    }
    else if (res < 0) root->left = insert(root->left, a);
    else root->right = insert(root->right, a);
    return root;
}

/* Take a file a parameter and build a BST out of it */
node *make_abr(FILE *f)
{
    node *root = NULL;
    char *buffer = NULL; // used by getline()
    size_t n = 0; // used by getline()
    while (true) 
    {
        ssize_t e = getline(&buffer, &n, f);
        // EOF reached
        if (e < 0)
        {
            // If the buffer is not empty we free it
            if (buffer != NULL) free(buffer);
            break; // quit the reading routine
        }
        buffer[strlen(buffer)-1] = 0; // remove '\n'
        // copy of the buffer
        char *b_copy = strdup(buffer);
        // extract tokens
        char *token = strtok(buffer, " "); // read first token 
        // read second token if exists
        char *z = strtok(NULL, " ");
        if (z == NULL) z = "MANCA";

        node *new = crea_node(z, b_copy);
        root = insert(root, new);
        free(b_copy);
    }
    return root;
}

int main(int argc, char **argv)
{
    if (argc != 2) 
    {
        fprintf(stderr, "Usage: </.program_name> <filename>");
        exit(1);
    }
    
    FILE *file = fopen(argv[1], "r");
    if (file == NULL) 
    {
        fprintf(stderr, "Error reading file");
        exit(2);
    }

    node *abr = make_abr(file);
    print_abr(abr);
    destroy_node(abr);
    fclose(file);

    return 0;
}
