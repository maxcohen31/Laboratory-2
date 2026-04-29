#! /usr/bin/env python3

import threading
import time

# Possibile soluzione al problema dei 
# lettori/scrittori utilizzando una variabile di condizione

class RW:
    def __init__(self) -> None:
        self.readers = 0        # lettori attivi
        self.writers = 0        # scrittori in attesa
        self.writing = False    # True se c'è almeno uno scrittore attivo
        self.cond = threading.Condition()

    # 
    def read_lock(self) -> None:
        """
        Il lettore si mette in attesa finché c'è uno scrittore attivo
        o se ci sono scrittori in attesa. Se il lettore passa viene 
        incrementata la variabile dei lettori
        """
        tid = threading.get_native_id() % 100
        print(f"{tid:2d} read request")
        with self.cond:
            while self.writers > 0 or self.writing:
                self.cond.wait()
            self.readers += 1
    
    def read_unlock(self) -> None:
        """
        Rilascia la lock. Decrementa il contatore dei lettori.
        L'ultimo dei lettori risviglia un thread.
        """
        tid = threading.get_native_id() % 100
        print(f"{tid:2d} read completed")
        with self.cond:
            assert self.readers > 0 # There must be at least one reader
            assert not self.writing # There must not be any writer
            self.readers -= 1
            if self.readers == 0:
                self.cond.notify()

    def write_lock(self) -> None:
        """
        Acquisisce la lock in scrittura. Aumenta il contatore degli
        scrittori in attesa. Attende finché non ci sono né lettori
        né scrittori attivi. Una volta presa la lock la variabile 
        writing diventa True
        """
        tid = threading.get_native_id() % 100
        print(f"{tid:2d} write request")
        with self.cond:
            self.writers += 1
            while self.writing or self.readers > 0:
                self.cond.wait()
            self.writing = True
            self.writers -= 1
     
    def write_unlock(self) -> None:
        """
        Rilascia la lock in scrittura. Reimposta writing a True
        e notifica tutti i thread in attesa.
        """
        tid = threading.get_native_id() % 100
        print(f"{tid:2d} write done")
        with self.cond:
            assert self.writing
            self.writing = False
            self.cond.notify_all()


def reader(rw_obj) -> None:
    rw_obj.read_lock()
    time.sleep(0.5)
    rw_obj.read_unlock()

def writer(rw_obj) -> None:
    rw_obj.write_lock()
    time.sleep(1)
    rw_obj.write_unlock()

def main():
    rw_obj = RW()
    threads = []

    threads.append(threading.Thread(target=reader, args=(rw_obj, )))
    threads.append(threading.Thread(target=reader, args=(rw_obj, )))
    threads.append(threading.Thread(target=writer, args=(rw_obj, )))
    threads.append(threading.Thread(target=reader, args=(rw_obj, )))
    threads.append(threading.Thread(target=reader, args=(rw_obj, )))
    threads.append(threading.Thread(target=reader, args=(rw_obj, )))
    threads.append(threading.Thread(target=reader, args=(rw_obj, )))
    threads.append(threading.Thread(target=reader, args=(rw_obj, )))
    threads.append(threading.Thread(target=writer, args=(rw_obj, )))
    threads.append(threading.Thread(target=reader, args=(rw_obj, )))
    threads.append(threading.Thread(target=reader, args=(rw_obj, )))
    threads.append(threading.Thread(target=reader, args=(rw_obj, )))

    for thread in threads:
        thread.start()

    for thread in threads:
        thread.join()

    assert rw_obj.writers == 0
    assert rw_obj.readers == 0
    assert rw_obj.writing == False

if __name__ == "__main__":
    main()
