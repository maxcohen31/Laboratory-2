#! /usr/bin/env python3

"""
    Soluzione compitino del 20/02/2025 
    Corso: Laboratorio 2
"""

import os
from pathlib import Path
import sys

class File:
    def __init__(self, nome, linee = None):
        self.nome = nome
        if linee == None:
            self.linee = []
        else:
            self.linee = linee 

    def __lt__(self, other):
        """
        Ordinamento 
         prima in base alla riga che contiene pattern 
         in caso di parità, in base al nome del file
        """
        def prima_linea_con_pattern(file):
            for line in file.linee:
                if pattern in line:
                    return line
            return None

        prima_linea_self = prima_linea_con_pattern(self)
        prima_linea_other = prima_linea_con_pattern(other)

        if prima_linea_self is None and prima_linea_other is None:
            return self.nome < other.nome
        if prima_linea_self is None:
            return False
        if prima_linea_other is None:
            return True
        if prima_linea_self == prima_linea_other:
            return self.nome < other.nome
        return prima_linea_self < prima_linea_other         

    def __str__(self):
        return f"### {self.nome}\n" + "".join(self.linee)

######################################################

def salva_contenuto_file(file):
    """
    Salva il contenuto del file letto
    """
    with open(file, "r") as f:
        return f.readlines()

def salva_num_linee(file, n):
    """
    Salva n linee 
    """
    with open(file, "r") as f:
        return f.readlines()[:n]

if __name__ == "__main__":
    if len((sys.argv)) != 4:
        print("Uso: directory num_linee pattern")
        sys.exit(1)

    dir = sys.argv[1]
    num = int(sys.argv[2])
    pattern = sys.argv[3]
    lista_file = os.listdir(dir)
    tutti = []

    for f in lista_file:
        nome_completo = os.path.join(dir, f)
        # controllo che f sia un file
        if os.path.isfile(nome_completo):
            # salvo il file nell'oggetto File
            file_trovato = File(os.path.abspath(nome_completo), salva_num_linee(nome_completo, num))
            tutti.append(file_trovato)
        # ho trovato una directory
        elif os.path.isdir(nome_completo):
            # verifico che la directory sia accessibile
            dir_path = Path(nome_completo)
            if not os.access(nome_completo, os.R_OK | os.X_OK):
                print(f"Directory {nome_completo} non accessibile")
                continue
            # cerca ricorsivamente tutti i file nelle sotto directory 
            tutti.extend([File(file.name, salva_contenuto_file(file)) for file in dir_path.rglob("*") if file.is_file()])
    
    # filtro la lista tutti affinchè contenga i file con il numero 
    # delle linee minore o uguale al parametro num
    tutti = [f for f in tutti if len(f.linee) <= num]

    pochi = [f for f in tutti if any(pattern in line for line in f.linee)]
    pochi.sort()
   

    for i in pochi:
        print(f"'pochi' contiene {len(pochi)} file")
        print(i, end="")
    

