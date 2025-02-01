#! /usr/bin/env python3

from sys import argv, exit


if __name__ == "__main__":
    if len(argv) != 2:
        exit("ERRORE. Uso: programma nomefile") 
    with open(argv[1], "r") as f:
        with open(argv[1] + ".rev", "w") as new_file:
            print("".join(f.readlines()[::-1]), end="", file=new_file)

