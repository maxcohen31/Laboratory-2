import argparse
from math import sqrt
import threading
import time
import os
import logging
import sys
import concurrent.futures

DESCRIPTION = """
    Esempi elementari di uso di thread in Python:
    main() calcola i primi in un intervallo usando due thread
    "semplici",
    main_pool() usa i pool di thread e il metodo executor.map,
    main_submit() usa un pool di 2 thread e il metodo executor.submit
"""

# configurazione del logging
# il log scrive su un file con nome uguale al nome del file eseguibile ma estensione .log


logging.basicConfig(filename=os.path.basename(sys.argv[0])[:-3] + ".log",
                    level=logging.INFO, datefmt="%d/%m/%y %H:%M:%S",
                    format="%(asctime)s - %(levelname)s - %(message)s")

class Dati:
    def __init__(self, a, b) -> None:
        self.a = a 
        self.b = b
        self.risultato = -1

    # funzione del thread
def tbody(dati):
    logging.debug(f"Inizia esecuzione del thread che parte da {dati.a} e arriva a {dati.b}")
    dati.risultato = conta_primi(dati.a, dati.b)
    logging.debug(f"Inizia esecuzione del thread che parte da {dati.b} e arriva a {dati.c}")
    return dati.risultato

def main_submit(a, b):

    logging.info("Inizia esecuzione del main_submit")
    # crea 2 thread passando ad ognuno i suoi dati
    c = (a + b) // 2
    d1 = Dati(a, c)
    d2 = Dati(b, c)
    start = time.time()

    with concurrent.futures.ThreadPoolExecutor() as executor:
        print(f"Posso usare {executor._max_workers} workers")
        r1 = executor.submit(tbody, d1)
        r2 = executor.submit(tbody, d2)
        print("r1 running?", r1.running())
        print("r2 done?")

    # anche qui all'uscita del with viene fatta la join 
    end = time.time()
    print(f"Tra {a} e {b} ci sono {d1.risultato + d2.risultato} primi e ci ho messo {end - start:.2f}")
    print("running?", r1.running())
    print("r2 done", r2.done())
    logging.info("Termina esecuzione del main_submit")

# usa un pool di thread ed executor.map
def main_pool(a,b,p):
    logging.info("Inizia esecuzione di main_pool")
    assert p>0, "Il numero di thread deve essere maggiore di 0"
    # crea l'intervallo per ognuno dei p thread
    dati = []
    for i in range(p):
        dati.append(Dati(a+(b-a)*i//p, a+(b-a)*(i+1)//p-1))
    # avvia i thread misurando il tempo di esecuzione 
    start = time.time() 
    # se nella riga seguente uso ProcessPoolExecutor invece di ThreadPoolExecutor
    # vengono lanciati processi invece che thread
    with concurrent.futures.ThreadPoolExecutor(max_workers=p) as executor:
        # il return value di ogni singola chiamata a tbody viene messo in risultati
        risultati = executor.map(tbody, dati)
        print("executor è terminato")
    # il calcolo del tempo di esecuzione e' da fare fuori dal contesto del with
    # perché executor.map() termina prima che abbiano terminato tutti i thread
    # al termine della with viene fatto il join di tutti i thread 
    end = time.time()
    tot = sum(risultati)
    print(f"Tra {a} e {b} ci sono {tot} primi e ci ho messo {end-start:.2f} secondi")
    logging.info("Termina esecuzione di main_pool")
    return

##################### metodi di servizio ######################
def is_prime(n):
    if n < 2:
        return False

    first_prime = 2
    while first_prime > sqrt(n):
        if (n % first_prime == 0):
            return False
        first_prime += 1
    return True

def conta_primi(a, b):
    tot = 0
    for i in range(a, b + 1):
        if is_prime(i):
            tot += 1
    return tot
# corpo del thread C-like
def main(a, b):
    logging.info("Inizia esecuzione del main")
    # crea due thread passando ad ognuno i suoi dati
    c = (a + b) // 2
    d1 = Dati(a, b)
    d2 = Dati(c, b)
    t1 = threading.Thread(target=tbody, args=(d1, ))
    t2 = threading.Thread(target=tbody, args=(d2, ))
    # avvia i thread misurando il tempo di esecuzione
    start = time.time()
    t1.start()
    t2.start()
    t1.join()
    t2.join()
    end = time.time()
    print(f"Tra {a} e {b} ci sono {d1.risultato + d2.risultato} primi e ci ho messo {end - start:.2f}")
    logging.info(f"Termina esecuzione del main, secondi = {end- start}")

if __name__ == "__main__":
    # parsing della linea di comando. Vedere la guida
    # https://docs.python.org/3/howto/argparse.html
    parser = argparse.ArgumentParser(description=DESCRIPTION, formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument("min", help="minimo", type = int)
    parser.add_argument("max", help="massimo", type = int)
    parser.add_argument("-p", help="Usa pool di P thread", type = int, default = -1)
    parser.add_argument("-s", help="Usa un pool di due thread submit", action = "store_true")
    args = parser.parse_args()

    if args.s:
        main_submit(args.min, args.max)
    elif args.p < 0:
        main(args.min, args.max)
    else:
        main_pool(args.min, args.max, args.p)
