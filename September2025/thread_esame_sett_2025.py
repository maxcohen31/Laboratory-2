"""
    Un sistema gestisce l’accesso concorrente a una risorsa condivisa da parte di due tipi distinti di
    thread: TA e TB. I thread TA accedono alla risorsa in mutua esclusione con tutti i thread. 
    I thread TB possono accedere alla risorsa in gruppi, con un massimo di k thread contemporaneamente, ma solo
    se non ci sono thread TA attivi o in attesa. 
    I thread TA hanno priorità maggiore: nessun thread TB può entrare nella sezione critica se c'è almeno un thread TA in attesa.

    Proporre un’implementazione per le funzioni enter_TA(), exit_TA(), enter_TB(), exit_TB() che faccia
    uso per la sincronizzazione esclusivamente di semafori utilizzando le primitive P e V. Indicare
    chiaramente il valore iniziale di ogni semaforo utilizzato.
"""

from concurrent.futures import ThreadPoolExecutor
import threading
import time
import random
import logging
import os
import sys

logging.basicConfig(
    filename=os.path.splitext(os.path.basename(sys.argv[0]))[0] + ".log",
    level=logging.INFO,
    datefmt="%d%m%y %H:%M:%S",
    format="%(asctime)s - %(levelname)s - %(message)s"
)

class Thread:
    def __init__(self, K=4) -> None:
        self.K = K
        self.mutex = threading.Semaphore(1) # binary semaphore
        self.sem_TA = threading.Semaphore(0) # sempahore for A threads
        self.sem_TB = threading.Semaphore(0) # sempahore for B threads
        self.waitTA = 0 # thread A counter
        self.waitTB = 0 # thread B counter
        self.activeTA = 0 # signals if thread A is active
        self.activeTB = 0 # signals if thread B is active
    
    def enter_TA(self):
        self.mutex.acquire() # entering the critical section
        self.waitTA += 1 # increment thread A counter
        while self.activeTA == 1 or self.activeTB > 0:
            self.mutex.release() # waiting for enter the critical section - V(mutex)
            self.sem_TA.acquire() # block until signaled
            self.mutex.acquire() 
        self.waitTA -= 1
        self.activeTA = 1 # thread A is active
        self.mutex.release()

    def exit_TA(self):
        self.mutex.acquire()
        activeTA = 0
        if self.waitTA > 0:
            self.sem_TA.release() # threads A have the priority - wake up a waiting A thread
        else:
            if self.waitTB > 0:
                n = min(self.waitTB, self.K) # allow up to K B threads to enter
                for _ in range(n):
                    self.waitTB -= 1
                    self.activeTB += 1
                    self.sem_TB.release() # maximum k thread enters 
        self.mutex.release() # exit critical section

    def enter_TB(self):
        self.mutex.acquire()
        self.waitTB += 1
        # if no thread A is active and capacity is < K, let thread B in
        if self.activeTA == 0 and self.waitTA == 0 and self.activeTB < self.K:
            self.waitTB -= 1
            self.activeTB += 1
            self.sem_TB.release()
        self.mutex.release()
        self.sem_TB.acquire() # wait until allowed
    
    def exit_TB(self):
        self.mutex.acquire()
        self.waitTB -= 1
        if self.waitTA > 0:
            if (self.activeTB == 1): # last thread B
                self.sem_TA.release() # wake up a thread A
            else:
                if (self.waitTB > 0): # let in a thread B
                    self.sem_TB.release()
        self.mutex.release()

t = Thread(4)

def thread_TA(idx):
    print(f"thread A[{idx}] about to enter")
    t.enter_TA()
    time.sleep(random.uniform(1, 3)) 
    print(f"thread A [{idx}] exits")
    t.exit_TA()

def thread_TB(idx):
    print(f"thread B[{idx}] about to enter")
    t.enter_TB()
    time.sleep(random.uniform(1, 3)) 
    print(f"thread B [{idx}] exits")
    t.exit_TB()


def main():
    logging.info("Main has started...\n")  
    with ThreadPoolExecutor(max_workers=10) as executor:
        for i in range(1, 3):
            executor.submit(thread_TA, i)
        for j in range(3, 11):
            executor.submit(thread_TB, j)

if __name__ == "__main__":
    main()







