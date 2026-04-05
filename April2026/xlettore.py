import sys
import struct
import os

def main(nome):
    fd = os.open(nome, os.O_RDONLY)
    print(f"=={os.getpid()} == {nome} aperto in lettura", file=sys.stderr)
    tot = 0
    sum = 0
    while True:
        # legge fino a 4 byte mettendoli in un bytearray
        bs = os.read(fd, 4)
        if len(bs) == 0: break
        tot += 1
        # converte i 4 byte letti in un intero e lo stampa
        valore = struct.unpack("<i", bs)[0]
        sum += valore
        print(f"=={os.getpid()} == {valore}")
    print(f"=={os.getpid()} == letti {tot} interi", file=sys.stderr)
    print(f"Sum: {sum}")

if __name__ == "__main__":
    if len(sys.argv) != 2:
        print("Uso:\n\t %s nomepipe" % sys.argv[0])
    else:
        main(sys.argv[1])
