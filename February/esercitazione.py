# Programma scritto da Luca Cosini

from os.path import exists
import sys 
import os
from string import ascii_lowercase
from pathlib import Path

if __name__ == "__main__":
    if len(sys.argv) != 3:
        print("Devi passare tre argomenti")
        #ricordati di controllare anche se le directory esistono
        sys.exit(1)

    else:

        src = sys.argv[1]
        dest = sys.argv[2]


        #si poteva evitare l'if con il comando che lo crea solo se non esiste già
        if (os.path.isdir(dest)):
            print("Directoty già esistente")
        else:
            os.mkdir(dest)
            p = os.path.abspath(dest)
            for letter in ascii_lowercase:
                os.makedirs(p+ "/" + letter, exist_ok=True)
        
        lista_file = []

        for path, subdirs, files in os.walk(src):
            for name in files:
                lista_file.append(os.path.join(path, name))

        sorted(lista_file)
        print(lista_file)
        
        last_name = ""
        counter = 0

        for file in lista_file:
            if (Path(file).is_symlink()):
                continue
            for letter in ascii_lowercase:
                if file.lower()[0] == letter and not(file in os.listdir(dest + f"/{letter}")):
                    os.symlink(src + "/" + file, dest + f"/{letter}/" + file)
                    print(f"{file} copiato!")
                elif(file.lower()[0] == letter):
                    if (file == last_name):
                        counter+=1
                    else:
                        counter = 2

                    nuovo_nome_file = file.split(".")
                    nfile = nuovo_nome_file[0] + str(counter) + nuovo_nome_file[1]
                    os.symlink(src + "/" + file, dest + f"/{letter}/" + nfile)
                    last_name = file
