#! usr/bin/env python3

import sys
import queue
import math 
import threading

def is_prime(n: int) -> bool:
    if n < 2: return False
    if n % 2 == 0: return n == 2
    for i in range(3, int(math.sqrt(n))+1, 2):
        if n % i == 0: return False
    return True

def cbody(buffer, buffer_lock, sem_free_slots, sem_data_items, results, thread_id):
    """
    Function performed by the consumer thread. 
    Reads one element at the time from the buffer and 
    keeps track of prime numbers.
    If a number n is prime the variable 'somma' gets increased by n.
    The variable 'quanti' counts every prime number found.
    """
    quanti = 0
    somma = 0
    print(f"[C] Consumer {threading.get_native_id()} has started")
    while True:
        sem_data_items.acquire()
        with buffer_lock:
            n = buffer.get()
        sem_free_slots.release()
        
        if n == -1: break # Termination signal
        if is_prime(n):
            quanti += 1
            somma += n
    results[thread_id] = (quanti, somma)
    print(f"[C] Consumer {thread_id} has finished")

def pbody(nomefile, buffer_lock, buffer, sem_free_slots, sem_data_items):
    """
    function performed by the producer thread.
    It populates the buffer with numbers read from a given file.
    """
    print(f"[P] Producer {threading.get_native_id()} with file {nomefile} has started")
    try:
        with open(nomefile, "r") as f:
            for line in f:
                n = int(line.strip())
                assert n > 0, "n must be positive"
                sem_free_slots.acquire()
                with buffer_lock:
                    buffer.put(n)
                sem_data_items.release()
    except FileNotFoundError:
        print(f"Error: file {nomefile} not found!")
    print(f"[P] Producer {threading.get_native_id()} has finished")


def main():
    if len(sys.argv) < 2:
        print(f"Usage:\t {sys.argv[0]} file1 [file2 ... fileK] ")
        sys.exit(1)

    # Consumer thread
    cons_thread = 3
    assert cons_thread >= 1
    # Producer thread
    prod_thread = len(sys.argv) - 1
    assert prod_thread >= 1

    tot_primes = 0
    sum = 0

    # Buffer init
    buffer = queue.Queue(maxsize=10)
    buffer_lock = threading.Lock()

    # Semaphores
    sem_free_slots = threading.Semaphore(10)
    sem_data_items = threading.Semaphore(0)

    results = [None] * cons_thread
    c_threads = []
    for i in range(cons_thread):
        t = threading.Thread(target=cbody, args=(buffer, buffer_lock, sem_free_slots, sem_data_items, results, i))
        c_threads.append(t)
        t.start()
    print("Consumers started")

    p_threads = []
    for i in range(prod_thread):
        t = threading.Thread(target=pbody, args=(sys.argv[i+1], buffer_lock, buffer, sem_free_slots, sem_data_items))
        p_threads.append(t)
        t.start()
    
    # Join (producers)
    for t in p_threads:
        t.join()

    # Writing termination signal
    for _ in c_threads:
        sem_free_slots.acquire()
        with buffer_lock:
            buffer.put(-1)
        sem_data_items.release()
    print("Termination signals written correctly")

    # Join (consumers)
    for t in c_threads:
        t.join()

    
    for qnt, s in results:
        tot_primes += qnt
        sum += s

    print(f"Found {tot_primes} primes with sum {sum}")


if __name__ == "__main__":
    main()
