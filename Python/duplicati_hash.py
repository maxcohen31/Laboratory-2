"""
Esempio di script per la ricerca di file duplicati
"""

import os, os.path
import sys
import subprocess




def cmpshell(a,b):
  """Confronta due file usando cmp della shell, restituisce il return value di cmp"""
  ris = subprocess.run(["cmp",a,b], stdout=subprocess.PIPE, stderr=subprocess.PIPE, text=True)
  return ris.returncode


def hash_file(filename):
    """Restituisce l'hash del file passato come argomento """
    r = subprocess.run(
        ["sha1sum", filename], stdout=subprocess.PIPE, stderr=subprocess.PIPE, text=True)
    if r.returncode != 0:
        raise RuntimeError(f"sha1sum failed on {filename}")
    return r.stdout.split()[0]

def visualizza_file_stesso_hash(file_list):
    result = {}
    for f in file_list:
        hash_v = hash_file(f)
        result.setdefault(hash_v, []).append(f)

    for h,f in result.items():
        print(f"file con hash {h}")
        for file in f:
            print(" - ", file)


def visualizza_duplicati(lista):
  """Dato un insieme di file visualizza quali di loro sono dei duplicati"""
  
  assert len(lista)>1, "Devo essere chiamato con lista con + di 1 elemento"
  for i in range(len(lista)):
    for j in range(i+1,len(lista)):
      # check hash of files
      if cmpshell(lista[i], lista[j]) == 0:
        # cmp restituisce True -> i file sono uguali 
        print(f"{lista[i]}\n  identico a:\n{lista[j]}\n")
        # passa al file successivo
        break


# funzione ricorsiva per cercare il file più grande
# nella directory corrente e in tutte le sue sottodirectory
def elenco_file(nome,dir_esplorate):
  """restituisce una coppia (nome,dimensione) per ogni 
     file dentro la directory nome e le sue
     sottodirectory"""

  assert os.path.isdir(nome), "Argomento deve essere una directory"
  print(f"Begin: {nome}",file=sys.stderr)
  # aggiungo nome ai già esplorati
  dir_esplorate.add(os.path.realpath(nome))
  # inizializzo la lista di file trovati in questa dir
  elenco = []
  # ottiene il contenuto della directory 
  listafile = os.listdir(nome)
  for f in listafile:
    nomecompleto = os.path.join(nome,f)
    # verifica se il file è accessibile
    if not os.access(nomecompleto,os.F_OK):
      print("!! Broken link:", nomecompleto, file=sys.stderr)
      continue
    # distinguo tra file normali e directory
    if not os.path.isdir(nomecompleto):
      nuovadim = os.path.getsize(nomecompleto)
      nuovonome = nomecompleto
      # aggiungo alla lista risultato
      # il file appena incontrato
      # il file è rappresentato dalla tupla
      #  (nome,dimensione)
      elenco.append((nuovonome,nuovadim))
    else:
      # nomecompleto è una directory: possibile chiamata ricorsiva
      # verifico che la directory sia esplorabile 
      if not os.access(nomecompleto, os.R_OK | os.X_OK):
        print(f"!! Directory {nomecompleto} non accessibile",file=sys.stderr)
        continue
      # verifica che la directory non sia già stata esplorata
      # va fatta con il realpath perchè la stessa dir può avere più nomi  
      nomereal = os.path.realpath(nomecompleto)
      if nomereal in dir_esplorate:
        print(f"!! Directory {nomecompleto} già esplorata",file=sys.stderr)
        print(f"!!  con nome {nomereal}",file=sys.stderr)
        continue
      # directory nuova e accessibile: esegui ricorsione
      lista_dir = elenco_file(nomecompleto,dir_esplorate)
      # concatena la lista di file della sottodirctory
      elenco += lista_dir
  # fine ciclo for sui file di questa directory     
  print(f"End: {nome}",file=sys.stderr)
  return elenco

def main(nomedir):
  """Lancia ricerca ricorsiva su nomedir
     poi cerca i duplicati tra tutti i file restituiti"""
  if not os.path.exists(nomedir):
    print("Il nome che mi hai passato non esiste")
    sys.exit(1)
  if not os.path.isdir(nomedir):
    print("Il nome che mi hai passato esiste, ma non è una directory")
    sys.exit(1)
  # invocazione funzione ricorsiva per creare l'elenco  
  # contenente le coppie (nomefile,dimensione)
  # nomeabs = os.path.abspath(nomedir)
  elenco = elenco_file(nomedir,set()) # cambiare nomedir->nomeabs per avere i path assoluti
  # creo dizionario dim -> [lista file di quella dimensione]
  diz = {}
  for nome,dim in elenco:
    if dim not in diz:
      diz[dim] = [nome]
    else:
      diz[dim].append(nome)
      
  # stampa gli elenchi che hanno più di un file
  for dim in diz:
    elenco = diz[dim]
    if len(elenco)>1:
      print(f"Devo confrontare i file di dimensione {dim}:")
      # ~ # stampo elenco
      # ~ for f in elenco:
        # ~ print("  " + f)
      # ~ print(50*"-")
      # da completare con i confronti ....
      #visualizza_duplicati(elenco)
      visualizza_file_stesso_hash(elenco)
  return

# verifico argomenti sulla linea di comando
# e faccio partire la ricerca
if __name__ == "__main__":
    if len(sys.argv) == 2:
        main(sys.argv[1])
    else:
        print("Uso:", sys.argv[0], "nome_directory")
