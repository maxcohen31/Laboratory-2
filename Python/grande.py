import os
from sys import argv, stderr

"""
Esempio di script per la navigazione del filesystem
Legge sulla riga di comando il nome di una directory
e restituisce il file più grande in essa contenuto
incluse tutte le sottodirectory

Funzioni di libreria per la gestione di file e directory

os.getcwd()       # restituisce directory corrente
os.chdir(path)    # cambia directory
os.listdir(path)  # elenca file (restituisce lista di stringhe)
os.access(path)   # verifica i permessi 
 
os.path.getsize(path) # dimensione file
os.path.exists(path)  # vero se il file/directory esiste  
os.path.isfile(path)  # vero se regular file
os.path.isdir(path)   # vero se directory
os.path.islink(path)  # vero se symbolic link
os.path.join(nome_dir,nome_file) # combina nome dir e file
os.path.abspath(path)  # restituisce path assoluto
os.path.realpath(path) # restituisce nome canonico eliminando link simbolici

Lista completa dei comandi su:
  https://docs.python.org/3/library/os.html
  https://docs.python.org/3/library/os.path.html

Per informazioni sui permessi dei file o come cambiarli
  man chmod

Per informazioni sui link simbolici e il loro uso:
   https://linuxize.com/post/how-to-create-symbolic-links-in-linux-using-the-ln-command/
"""

def trova_file(dir_name):
    """
    Restituisce la coppia (dim, nome) che identifica il file più grande
    tra quelli nella directory dir_name ed eventuali sottodirectory
    """
    assert os.path.isdir(dir_name) # dir_name deve essere una directory
    print(f"{dir_name} non è una directory")

    largest_file = None
    max_dimension = -1
    # contenuto della directory
    content = os.listdir(dir_name)
    for f in content:
        full_name = os.path.join(dir_name, f)
        # distinguo i file e le directory
        if not os.path.isdir(full_name):
            new_dim = os.path.getsize(full_name)
            new_file_found = full_name
        else:
            # Se full_name è una directory va esplorata.
            # Controlliamo i permessi 
            if not os.access(full_name, os.R_OK | os.X_OK):
                print(f"{full_name} non accessibile!", file=stderr)
                continue
            # la directory è accessibile e quindi richiamo trova_file
            new_dim, new_file_found = trova_file(full_name)
        # aggiorno il nome e la dimensione se ne ho trovato dei nuovi
        if new_dim > max_dimension:
            max_dimension = new_dim
            largest_file = new_file_found
    # fine ciclo
    print(f"Fine: {largest_file}", file=stderr)
    return (max_dimension, largest_file)
    
def main():
    if len(argv) != 2: 
        print("Uso: ", argv[0], "<nome directory>")
    else:
        if not os.path.exists(argv[1]):
            print("Il nome inserito non è valido")
            exit(1)
        if not os.path.isdir(argv[1]):
            print("Il nome inserito esiste ma non è una directory")
            exit(1)
        nome = os.path.abspath(argv[1])
        dim, nomeg = trova_file(nome)
        print(f"Il file più grande è {nomeg} e la sua dimensione è {dim} byte")
        return

if __name__ == "__main__":
    main()
