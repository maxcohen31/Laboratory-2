#! /usr/bin/env python3

import sys
import threading
import logging
import os
import time
import concurrent.futures

# Configurazione del logging
# il logger scrive su un file con nome uguale al nome del file eseguibile
logging.basicConfig(
        filename = os.path.basename(sys.argv[0])[0:3] + 'log',
        level = logging.INFO,
        datefmt = '%d/%m/%y %H:%M:%S',
        format = '%(asctime)s - %(levelname)s - %(message)s'
        )

def is_prime(n):
    assert n > 0, "n must be positive"
    if n < 2: return False
    if n == 2: return True
    if n % 2 == 0: return False
    for i in range(3, n//2, 2):
        if n % i == 0: return False
        if i * i > n: break
    return True

def elenco_primi(a, b):
    ris = []
    for i in range(a, b + 1):
        if is_prime(i):
            ris.append(i)
    return ris

# Classe per rappresentare la somma e il mutex
class Somma:
    def __init__(self) -> None:
        self.somma = 0
        self.lock = threading.Lock() # Analogo del mutex del C


# Calcola la somma dei primi in [a, b)
def tbody(a, b, somma):
    logging.debug(f"Thread in range {a, b} has started")
    lis = elenco_primi(a, b)
    for p in lis:
        # with somma.lock
        tmp = somma.somma + p
        time.sleep(0.001)
        somma.somma = tmp
    logging.debug(f"Thread in range {a, b} has finished")
    return

def main(a, b, p):
    logging.debug("Main has started")
    assert p > 0, "The number of threads must be positive"
    somma = Somma()
    with concurrent.futures.ThreadPoolExecutor(max_workers=p) as executor:
        for i in range(p):
            ai = a + (b - a) * i // p
            bi = a + (b - a) * (i + 1) // p - 1
            executor.submit(tbody, ai, bi, somma)
    print(f"Sum of the first primes in range [{a}, {b}) is {somma.somma}")
    logging.debug("Main finished")
    return


if __name__ == "__main__":
    if len(sys.argv) == 3:
        main(int(sys.argv[1]), int(sys.argv[2]), 1)
    if len(sys.argv) == 4:
        main(int(sys.argv[1]), int(sys.argv[2]), int(sys.argv[3]))
    else:
        print(f"Usage:\t%s <start> <end> <thread_num>" % sys.argv[0])

