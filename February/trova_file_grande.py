#! /usr/bin/env python3

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

"""

from os import path
from os import listdir
from os import F_OK, X_OK, R_OK
from os import access
from sys import argv, exit, stderr


def cerca_grande(nome, directory_esplorate):
    assert path.isdir(nome), "Argomento deve essere una directory"

    lista_file = listdir(nome)
    dim = -1
    nome_file_grande = None

    for file in lista_file:
        # prendo percorso di ogni elemento in lista_file
        nome_completo = path.join(nome, file)
        # verifica se il file è accessibile
        if not access(nome_completo, F_OK):
            print("BROKEN LINK!", nome_completo, file=stderr)
            continue
        # distinguo tra file accessibili e directory
        if not path.isdir(nome_completo):
            nuova_dim = path.getsize(nome_completo)
            nuovo_nome = nome_completo
        else:
            # nome_completo è una directory
            # verifico che la directory sia esplorabile
            if not access(nome_completo, X_OK | R_OK):
                print(f"Directory {nome_completo} non esplorabile")
                continue
            nome_dir = path.realpath(nome_completo)
            # verifico che la directory sia già stata esplorata
            if nome_dir in directory_esplorate:
                print(f"Directory {nome_dir} già esplorata!", file=stderr)
                continue
            directory_esplorate.add(nome_completo)
            # directory nuova e accessibile
            nuova_dim, nuovo_nome = cerca_grande(nome_completo, directory_esplorate)
        if nuova_dim > dim:
            dim = nuova_dim
            nome_file_grande = nuovo_nome

    return (dim, nome_file_grande)

if __name__ == "__main__":
    nomedir = argv[1]
    
    # la directory esiste
    if not path.exists(nomedir):
        print("Questa directory non esiste")
        exit(1)
    # il nome passato non è una directory
    if not path.isdir(nomedir):
        print("Il nome passato non è una directory")
        exit(1)

    # path assoluto
    abspath = path.abspath(nomedir)
    # dimensione
    dim, risultato = cerca_grande(abspath, set())

    print(f"Il file più grande è {risultato} di dimensione {dim}")
    

