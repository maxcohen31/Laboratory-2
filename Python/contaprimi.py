import os
import re
import sys
import logging
import argparse
import time
import threading
import concurrent.futures

# Descrizione usata dal modulo argparse
Description = """
    Esempi elementari di uso di thread in python:
    main() calcola i primi in un intervallo usando due thread C-like
    main_pool() usa un pool di thread e il metodo executor.map
    main_submit() usa un pool di 2 thread e il metodo executor.submit """

# Configurazione dei logging
logging.basicConfig(
        filename=os.path.basename(sys.argv[0])[:-3] + ".log",
        level=logging.INFO,
        datefmt="%d/%m/%y %H:%M:%S",
        format="%(asctime)s - %(levelname)s - %(message)s"
        )

def is_prime(n: int) -> bool:
    assert n > 0, "n must be positive"
    if n < 2: return False
    if n == 2: return True
    if n % 2 == 0: return False
    for i in range(3, n//2, 2):
        if n % i == 0: return False
        if i * i > n: break
    return True

def conta_primi(a: int, b: int) -> int:
    tot = 0
    for i in range(a, b + 1):
        if is_prime(i):
            tot += 1
    return tot

class Dati:
    def __init__(self, a: int, b: int) -> None:
        self.a = a
        self.b = b
        self.result = -1

# Corpo del thread prende un oggetto Dati come argomnto
def tbody(dati: Dati) -> int:
    logging.debug(f"Thread from {dati.a} to {dati.b} has started")
    dati.result = conta_primi(dati.a, dati.b)
    logging.debug(f"Thread from {dati.a} to {dati.b} has finished")
    return dati.result

# Crea due thread in maniera C-like
def main(a, b):
    logging.info("main init")
    # Crea due thread passando a ciascuno i propri dati
    c = (a + b) // 2
    d1 = Dati(a, c)
    d2 = Dati(c, b)
    t1 = threading.Thread(target=tbody, args=(d1,))
    t2 = threading.Thread(target=tbody, args=(d2,))
    # Avvia i thread misurando il tempo di esecuzione
    start = time.time()
    t1.start()
    t2.start()
    t1.join()
    t2.join()
    end = time.time()
    print(f"Between {a} and {b} there are {d1.result + d2.result} primes and my conclusion time is {end - start}")
    logging.info(f"Main finished. Time (seconds): {end - start}")

# Usa un pool di thread ed executor.map
def main_pool(a, b, p):
    logging.info("main_pool init")
    assert p > 0, "Threads number must be positive"
    # Intervallo per ognuno dei p thread    
    dati = []
    for i in range(p):
        dati.append(Dati(a+(b-a)*i//p, a+(b-a)*(i+1)//p-1))
    start = time.time()
    # Se nella riga seguente uso ProcessPoolExecutor invece di ThreadPoolExecutor
    # vengono lanciati processi invece che thread
    with concurrent.futures.ThreadPoolExecutor(max_workers=3) as executor:
        # Il return di ogni singolo tbody viene messo in risultati
        risultati = executor.map(tbody, dati)
        print("executor.map has finished but the threads are still working")
    # il calcolo del tempo di esecuzione e' da fare fuori dal contesto del with
    # perché executor.map() termina prima che abbiano terminato tutti i thread
    # al termine della with viene fatto il join di tutti i thread
    end = time.time()
    tot = sum(risultati)
    print(f"Between {a} and {b} there are {tot} primes and my conclusion time is {end - start}")
    logging.info(f"Main_pool finished")
    return

# Usa un pool di thread e executor.submit
def main_submit(a, b):
    logging.info("main_submit init")
    c = (a + b) // 2
    d1 = Dati(a, c)
    d2 = Dati(c, b)
    start = time.time()
    with concurrent.futures.ProcessPoolExecutor() as executor:
        print(f"I can use {executor._max_workers} workers")
        r1 = executor.submit(tbody, d1)
        r2 = executor.submit(tbody, d2)
        print("r1 running?", r1.running())
        print("r2 done?", r2.done())
    end = time.time()
    print(f"Between {a} and {b} there are {r1.result() + r2.result()} primes and my conclusion time is {end - start}")
    print("r1 running?", r1.running())
    print("r2 done?", r2.done())
    logging.info("main_submit done")


# Questo codice viene eseguito solo se il file è eseguito direttamente
# e non importato come modulo con import da un altro file
if __name__ == "__main__":
    logging.info("Command line: " + "".join(sys.argv))
    # Parsing della linea di comando
    parser = argparse.ArgumentParser(description=Description, formatter_class=argparse.RawTextHelpFormatter)
    parser.add_argument("min", help="minimo", type=int)
    parser.add_argument("max", help="massimo", type=int)
    parser.add_argument("-p", help="Use a pool of p threads", type=int, default=-1)
    parser.add_argument("-s", help="Use a pool of two threads with submit", action="store_true")
    args = parser.parse_args()
    
    if args.s:
        main_submit(args.min, args.max)
    elif args.p < 0:
        main(args.min, args.max)
    else:
        main_pool(args.min, args.max, args.p)
