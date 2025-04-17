import queue
import threading
import sys
import math

def is_prime(n):
    if n < 1:
        return False
    start = 2
    while start < math.sqrt(n):
        if n % start == 0:
            return False
        start += 1
    return True

# funzione eseguita dai thread consumatori
def cbody(buffer, buffer_lock, sem_free_slots, sem_data_items, results, thread_id):
    quanti = 0
    somma = 0
    print(f"[C] Consumatore {thread_id} partito")
    while True:
        sem_data_items.acquire()
        with buffer_lock:
            n = buffer.get()
        sem_free_slots.release()
        if n == -1: # segnale di terminazione
            break
        if is_prime(n):
            quanti += 1
            somma += n
    results[thread_id] = (quanti, somma)
    print(f"[C] Consumatore {thread_id} terminato")

# funzione eseguita dai thread produttori
def pbody(nomefile, buffer, buffer_lock, sem_free_slots, sem_data_items):
    print(f"[P] Produttore con file {nomefile} partito")
    try:
        with open(nomefile, "r") as f:
            for line in f:
                n = int(line.strip())
                assert n > 0
                sem_free_slots.acquire()
                with buffer_lock:
                    buffer.put(n)
                sem_data_items.release()
    except FileNotFoundError:
        print(f"Errore: file {nomefile} non trovato")
    print(f"[P] Produttore con file {nomefile} terminato")


def main():
    if len(sys.argv) < 2:
        print(f"Uso: {sys.argv[0]} file [file1 file2 ... filek]")
        sys.exit(1)

    cons = 3 # numero thread consumatori
    assert cons > 0
    prod = len(sys.argv) - 1 # produttori
    assert prod > 0

    tot_primi = 0
    tot_somma = 0

    # buffer condiviso
    buffer = queue.Queue(maxsize=10)
    buffer_lock = threading.Lock()

    # semafori
    sem_free_slots = threading.Semaphore(10)
    sem_data_items = threading.Semaphore(0)

    # creazione e avvio dei thread consumatori
    results = [None] * cons
    consumer_threads = []
    for i in range(cons):
        t = threading.Thread(target=cbody, args=(buffer, buffer_lock, sem_free_slots, sem_data_items, results, i))
        consumer_threads.append(t)
        t.start()
    print("Thread consumatori creati")

    # creazione dei thread produttori
    producer_threads = []
    for i in range(prod):
        t = threading.Thread(target=pbody, args=(sys.argv[i + 1], buffer, buffer_lock, sem_free_slots, sem_data_items))
        producer_threads.append(t)
        t.start()

    print("Thread produttori creati")

    # attesa della terminazione per i consumatori
    for t in producer_threads:
        t.join()

    # scrittura dei segnali di terminazione per i consumatori
    for _ in range(cons):
        sem_free_slots.acquire()
        with buffer_lock:
            buffer.put(-1)
        sem_data_items.release()
    print("Valori di terminazione scritti")

    # attesa della terminazione dei thread consumatori e calcolo del risultato
    for t in consumer_threads:
        t.join()

    for quanti, somma in results:
        tot_primi += quanti
        tot_somma += somma
    
    print(f"Trovati {tot_primi} primi con somma {tot_somma}")


if __name__ == "__main__":
    main()


