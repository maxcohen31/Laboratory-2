import threading
import time
import sys
import logging
import concurrent.futures
import os
from math import sqrt

# configuarazione del logging
logging.basicConfig(filename=os.path.basename(sys.argv[0])[:-3] + ".log",
                    level=logging.INFO, 
                    datefmt="%d/%m/%y %H:%M:%S",
                    format="%(asctime)s - %(message)s"
                    )

# classe usata per rappresentare la somma e il suo mutex
class Somma:
    def __init__(self) -> None:
        self.somma = 0
        self.lock = threading.Lock() # analogo di un mutex del C


# calcola somma dei primi in [a, b)
def tbody(a, b, somma):
    logging.debug(f"Inizia esecuzione del thread che parte da {a} e arriva in {b}")
    lis = elenco_primi(a, b)

    for p in lis:
        with somma.lock: # equivalente a mutex_lock
            tmp = somma.somma + p
            time.sleep(0.001) # questa condizione può causare una race condition
            somma.somma = tmp
    logging.debug(f"Termina esecuzione del thread che parte da {a} e finisce in {b}")
    return

# funzione per calcolare la somma dei primi in [a, b] 
def main(a, b, p):
    logging.debug("Inizia esecuzione del main")
    assert p > 0, "Il numero deve essere maggiore di 0"

    # crea l'intervallo per ognuno dei p thread
    somma = Somma()
    with concurrent.futures.ThreadPoolExecutor(max_workers=p) as executor:
        for i in range(p):
            ai = a + ((b - a)*i) // p
            bi = a + ((b - a) * (i + 1)) // (p - 1)
            # esempio d'uso di submit(): crea un singolo thread 
            # che esegue la funzione tbody con parametri ai, bi, somma
            executor.submit(tbody, ai, bi, somma)
    print(f"La somma dei primi in [{a}, {b}) è {somma.somma}")
    logging.debug("Termina esecuzione del main")
    return

# restituisce lista primi dei in [a, b]
def elenco_primi(a, b):
    ris = []
    for p in range(a, b + 1):
        if primo(p):
            ris.append(p)
    return ris

def primo(n):
    if n <= 1:
        return False
    start = 2
    while start <= sqrt(n):
        if (n % start == 0):
            return False
        start += 1
    return True

if __name__ == "__main__":
    if len(sys.argv) == 3:
        main(int(sys.argv[1]), int(sys.argv[2]), 1)
    elif len(sys.argv) == 4:
        main(int(sys.argv[1]), int(sys.argv[2]), int(sys.argv[3]))
    else:
        print("Uso:\n\t %s inizio fine [numthread]" % sys.argv[0])
