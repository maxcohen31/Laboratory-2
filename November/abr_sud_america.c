#define _GNU_SOURCE   // avverte che usiamo le estensioni GNU 
#include <stdio.h>    // permette di usare scanf printf etc ...
#include <stdlib.h>   // conversioni stringa exit() etc ...
#include <stdbool.h>  // gestisce tipo bool
#include <assert.h>   // permette di usare la funzione ass
#include <string.h>   // funzioni per stringhe
#include <errno.h>    // rischiesto per usare errno
#include <math.h>

// Scopo del programma:
// imparare a costruire, visualizzare e distruggere le liste in C


// prototipi delle funzioni che appaiono dopo il main()
void termina(const char *messaggio);


// definizione struct che rappresenta
// una città con nome, e coordinate 
// + campi left/right per costruire l'albero 
typedef struct capit 
{
    char *nome;
    char* nazione;
    int kpop;
    struct capit *left, *right;

} capitale;


void capitale_stampa(const capitale *a, FILE *f) {
  fprintf(f,"%-16s %-16s %d\n", a->nome, a->nazione, a->kpop);
}

capitale *capitale_crea(char *s, int pop, char* naz)
{
    capitale *a  = malloc(sizeof(*a));
    a->nome = strdup(s); // creo una copia di s e l'assegno al nome
    a->kpop = pop;
    a->nazione = strdup(naz);
    a->left = a->right = NULL;
    return a;
}

void capitale_distruggi(capitale *a)
{
    free(a->nome);
    free(a->nazione);
    free(a);
}

// stampa tutti gli elementi dell'albero
// che ha come radice root
void abr_capitale_stampa(const capitale *root, FILE *f)
{
    if(root!=NULL) 
    {
        abr_capitale_stampa(root->left,f);
        capitale_stampa(root,f);
        abr_capitale_stampa(root->right,f);
    }
}

// stampa tutti gli elementi dell'albero
// che ha come radice root
// facendo una visita in preorder
// il parametro depth è la profondita di root
void abr_capitale_stampa_preorder(const capitale *root, FILE *f, int depth)
{
    // stampa depth spazi per mostrare la profondità del nodo
    for (int i = 0;i < depth; i++) 
    {
        fprintf(f,". ");
    }
    if(root!=NULL) 
    {
        capitale_stampa(root,f);
        abr_capitale_stampa_preorder(root->left,f,depth+1);
        abr_capitale_stampa_preorder(root->right,f,depth+1);
    }
    else 
    {
        fprintf(f,"X\n");
    }
}



// distrugge tutti gli elementi dell'ABR con radice root
void abr_capitale_distruggi(capitale *root)
{
    if(root!=NULL) 
    {
        abr_capitale_distruggi(root->left);
        abr_capitale_distruggi(root->right);
        capitale_distruggi(root);
    }
}



// inserisci il nuovo nodo "c" dentro l'albero 
// con radice "root", non inserisce se c
// è già presente, restituisce la root
// del nuovo albero contenente anche "c"
capitale *abr_inserisci(capitale *root, capitale *c)
{
    c->left = NULL;
    c->right = NULL;
   
    // caso base albero vuoto
    if(root==NULL) 
    {
        return c;
    }

    int ris = strcmp(c->nome,root->nome);
    if(ris == 0) 
    {   // i nomi sono uguali
        fprintf(stderr, "Nodo duplicato: ");
        capitale_stampa(c,stderr);
        capitale_distruggi(c);
    }
    else if(ris < 0) // c->nome < root->nome
    {
        root->left = abr_inserisci(root->left,c);
    }
    else 
    {
        // c->nome > root->nome
        root->right = abr_inserisci(root->right,c);
    }

    return root;
}    
    

// crea un abr con gli oggetti capitale letti da 
// *f inserendoli usando l'ordinamento dei nomi 
capitale *crea_abr(FILE *f)
{
    capitale *root = NULL;
    char *buffer = NULL;
    size_t n = 0;

    while (true) 
    {
        // leggi linea dal file
        ssize_t e = getline(&buffer, &n, f);
        if (e < 0) // assumiamo sia finito il file
        {
            free(buffer);
            break;
        }

        // estrae i campi nome, nazione e popolazione
        char *s = strtok(buffer, ";\n");
        char *pop = strtok(NULL, ";\n");
        char *naz = strtok(NULL, ";\n");
        capitale *b = capitale_crea(s, atoi(pop), naz);
        root = abr_inserisci(root,b);
    }  
    return root;
}

// cerca la città nome dentro l'abr con radice root
// restituisce il puntatore alla città se trovata
// altrimenti NULL
capitale *abr_ricerca(capitale *root, char *nome)
{
    if(root==NULL) 
    {
        return NULL;
    }

    int cfr = strcmp(nome,root->nome);

    if(cfr == 0)
    {
        return root;
    }
    else if (cfr < 0) // ricerco a sinistra 
    {
        return abr_ricerca(root->left,nome);
    }
    else // ricerco a destra
    {
        return abr_ricerca(root->right,nome);
    }
}

// dato un abr di radice root restiuisce
// la sua altezza = numero di livelli =
// profondità massima di una foglia
int abr_altezza(capitale *root)
{
    if(root == NULL)
    {
        return 0;
    }

    int hl = abr_altezza(root->left);
    int hr = abr_altezza(root->right);

    return (hl > hr) ? hl+1 : hr+1;
}

int abr_popolazione_max(capitale *root)
{
    if (root == NULL)
    {
        return 0;
    }

    int max_kpop = root->kpop;
    int max_kpop_left = abr_popolazione_max(root->left);
    int max_kpop_right = abr_popolazione_max(root->right);

    if (max_kpop < max_kpop_right)
    {
        max_kpop = max_kpop_right;
    }
    else if (max_kpop < max_kpop_left)
    {
        max_kpop = max_kpop_left;
    }

    return max_kpop;
}

void salva_abr_inorder(const capitale *root, FILE* f)
{
    // visita in-order
    if (root == NULL)
    {
        return;
    }

    salva_abr_inorder(root->left, f);
    fprintf(f, "%s; %s; %d\n", root->nome, root->nazione, root->kpop);
    salva_abr_inorder(root->right, f);
}

void salva_abr(const capitale *root)
{
    FILE* abr_file = fopen("salva_albero.txt", "w");
    if (abr_file == NULL)
    {
        fprintf(stderr, "Error opening file");
    }

    salva_abr_inorder(root, abr_file);
    if (fclose(abr_file) == EOF)
    {
        fprintf(stderr, "Error closing file");
    }
}


void stampa_città_nazione(const capitale *root, const char* input)
{
    if (root == NULL) 
    {
        return;
    }

    if (strcmp(root->nazione, input) == 0)
    {
        // stampa le città se la nazione di input è valida
        printf("Città: %s\n", root->nome);
    }

    stampa_città_nazione(root->left, input);
    stampa_città_nazione(root->right, input);
}


// =================================================================================
int main(int argc, char *argv[])
{
    if(argc < 2) 
    {
        printf("Uso: %s nomefile [nome1 nome2 ...]\n",argv[0]);
        exit(1);
    } 

    FILE *f = fopen(argv[1],"r");
    if(f == NULL) 
    {
        termina("Errore apertura file");
    }

    // costruzione albero leggendo capitali dal file
    capitale *root = crea_abr(f);

    if (fclose(f) == EOF)
    {
        termina("Errore chiusura");
    }
    puts("--- inizio lista ---");
    // stampa lista capitali appena creata
     abr_capitale_stampa(root,stdout);  
    // abr_capitale_stampa_preorder(root, stdout,0);  

    puts("--- fine lista ---");
    printf("Altezza albero: %d\n",abr_altezza(root));

    for (int i = 2; i < argc; i++) 
    {
        capitale *c = abr_ricerca(root, argv[i]);
        if(c == NULL) 
        {
            printf("%s non trovata\n", argv[i]);
        }
        else 
        {
            printf("Trovata: ");
            capitale_stampa(c,stdout);
        }
    }

    puts ("----- Popolazione massima -----\n");
    int max_pop = abr_popolazione_max(root);
    printf("Max pop: %d\n", max_pop);

    printf("Salvo albero su file...\n");
    salva_abr(root); 

    // stampa_città_nazione(root, argv[2]);
    
    abr_capitale_distruggi(root);


    return 0;
}



// stampa su stderr il  messaggio che gli passo
// se errno!=0 stampa anche il messaggio d'errore associato 
// a errno. dopo queste stampe termina il programma
void termina(const char *messaggio)
{
    if(errno == 0) 
    {
        fprintf(stderr,"%s\n",messaggio);
    }
    else
    {
        perror(messaggio);
    }
    exit(1);
}
