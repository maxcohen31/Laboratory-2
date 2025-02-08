#! /usr/bin/env python3


import sys
import os

def elenco_file(nome, directory_visitate):
    assert os.path.isdir(nome), "Il nome passato deve essere una directory"

    risultato = []
    # tutti i file in nome
    lista_file = os.listdir(nome)

    for file in lista_file:
        nome_completo = os.path.join(nome, file)
        # verifica che il file sia accesibile
        if not os.access(nome_completo, os.F_OK):
            print("BROKEN LINK!", file=sys.stderr)
            continue
        # distinguo tra file normali e directory
        if not os.path.isdir(nome_completo):
            nuova_dim = os.path.getsize(nome_completo)
            nuovo_nome = nome_completo
            # aggiungo il file alla lista in una tupla
            risultato.append((nuova_dim, nuovo_nome))
        else:
            # directory trovata
            # verifico che sia accessibile
            if os.access(nome_completo, os.R_OK | os.X_OK):
                print(f"Directory {nome_completo} non accessibile", file=sys.stderr)

            nome_directory = os.path.realpath(nome_completo)
            if nome_directory in directory_visitate:
                print("Directory già visitata")
                continue
            directory_visitate.add(nome_directory)
            # directory nuova e accessibile: ricorsione
            lista_dir = elenco_file(nome_completo, directory_visitate)
            risultato += lista_dir
    return risultato

if __name__ == "__main__":
    if not os.path.exists(sys.argv[1]):
        print("La directory non esiste")
        sys.exit(1)
    if not os.path.isdir(sys.argv[1]):
        print("L'argomento passato non è una directory")
        sys.exit(1)
    
    nomeabs = os.path.abspath(sys.argv[1])
    elenco = elenco_file(nomeabs, set())

    elenco.sort(key=lambda x: (x[1], x[0]), reverse=True)

    for i in elenco:
        print(f"{i[1]} -> {i[0]}")


