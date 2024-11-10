# compilatore per ARMv7
CC=gcc
CFLAGS=-std=c11 -Wall -g

# compilatore e flag cross-compilazione
# CC=arm-linux-gnueabihf-gcc
# CFLAGS=-std=c11 -g -Wall -static

# stampa elenco dei primi <= N
# l'eseguibile primi dipende da primi_lab.c e primi.s
primi: primi_lab.c primi.s
	$(CC) $(CFLAGS) primi_lab.c primi.s -o primi
