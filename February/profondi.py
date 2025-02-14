import re
import os
import sys
from time import localtime, asctime


# classe per memorizzare le informazioni di un file
class Miofile:
    def __init__(self, path, depth=0):
        self.path = path
        self.mtime = os.path.getmtime(path)
        self.size = os.path.getsize(path)
        # contatore della profondità del file
        self.depth = depth
    
    def precedente_a(self,limite):
        """Restituisce true se il tempo di modifica
        è precedente a limite espresso in secondi da Epoch"""
        return self.mtime < limite
        
    def __lt__(self,other):
        "confronta dimensioni e a parità di dimensione il nome"
        if self.size < other.size:
            return True
        if self.size > other.size:
            return False 

    def __eq__(self,other):
        return self.path == other.path and self.size == other.size and self.mtime == other.mtime 

    def __str__(self):
        t = asctime(localtime(self.mtime))
        return f"{self.path}\n size:{self.size}  modificato:{t}"

    def __repr__(self):
        return f"Miofile({self.path})"


def cerca_profondi(nome_dir, dir_esplorate, profondità=0):
    assert os.path.isdir(nome_dir), "Devi passare una directory!"
    
    lista = []
    lista_file = os.listdir(nome_dir)

    for file in lista_file:
        nome_completo = os.path.join(nome_dir, file)
        # verifica se il file è accessibile
        if not os.access(nome_completo, os.F_OK):
            print("Broken link", nome_completo, sys.stderr)
            continue
        # file o directory? 
        if not os.path.isdir(nome_completo):
            dimensione_file = os.path.getsize(nome_completo) 
            # crea un oggetto Miofile con il file trovato
            # aggiungendolo alla lista
            nuovo_nome = Miofile(nome_completo, profondità)
            nuovo_nome.size = dimensione_file
            lista.append(nuovo_nome)
        else:
            # nome_completo è una directory
            if not os.access(nome_completo, os.R_OK | os.X_OK):
                print(f"{nome_completo} non è accessibile", file=sys.stderr)
                continue
            # la stessa directory può avere più nomi
            nome_reale = os.path.realpath(nome_completo)
            if nome_reale in dir_esplorate:
                print(f"{nome_reale} è già stata esplorata")    
            dir_esplorate.add(nome_reale)
            # nuova directory accessibile
            lista_directory = cerca_profondi(nome_completo, dir_esplorate, profondità + 1)
            lista += lista_directory
    return lista


def ordina_file(lista_file):
    # salvo la profondità massima
    max_depth = max(file.depth for file in lista_file)
    # ordina la lista in base alla dimensione degli oggetti Miofile e alla profondità
    deepest_files = sorted([file for file in lista_file if file.depth == max_depth], key=lambda x: (x.size, x.depth))
    return deepest_files    

if __name__ == "__main__":
    if len(sys.argv) != 2:
        print(f"Uso {sys.argv[0]} <nome_directory>")
        sys.exit(1)
    else:
        p = cerca_profondi(sys.argv[1], set(), 0)
        #for f in p:
        #    print(f"Il file {f} è a profondità {f.depth}\n")
        
        # ordinamento
        print("File ordinati per dimensione crescente")
        ordinati = ordina_file(p)
        
        for f in ordinati:
            print(os.path.basename(f.path))

