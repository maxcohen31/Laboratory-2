#! /usr/bin/env python3

from sys import argv, exit

if __name__ == "__main__":
    if len(argv) != 2:
        exit("ERRORE! Uso: nome_programma nome_file")
    with open(argv[1], "r") as f:
        with open(argv[1] + ".dec", "w") as new_file:
            print("".join(sorted(f.readlines(), key=lambda x: len(x))[::-1]).rstrip(), end="", file=new_file)
