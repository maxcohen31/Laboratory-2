import threading
import sys
import time

def countdown(a, pausa):
    for i in range(a, -1, -1):
        print(f"{threading.current_thread().name}", "-->", i)
        time.sleep(pausa)

def main(lista):
    th = []
    lista = []
    # passo in target la funzione da eseguire 
    # e in args gli argomenti della funzione (in una tupla)
    for s in lista:
        x = threading.Thread(target=countdown, args=(int(s), 0.5))
        x.start()
        th.append(x)
    for y in th:
        y.join()
        print(f"Thread {y.name} terminato")


if len(sys.argv)>1:
    main(sys.argv[1:])
else:
        print("Uso:\n\t %s i1 [i2 i3 ...]" % sys.argv[0])
# invoca il main con i parametri passati sulla linea di comando 
