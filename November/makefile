CC=gcc
CFLAGS=-std=c11 -Wall -O -g
LDLIBS=-lm

EXECS=parole frequenze legginomi cowboy

all: $(EXECS)

# Regola per creare il file oggetto lista_cowboy.o
cowboy: lista_cowboy.c lista_cowboy.h
	$(CC) $(CFLAGS) $< -o $@ $(LDLIBS)


parole: parole.o strint.o 
	$(CC) parole.o strint.o $(LDLIBS) -o parole

parole.o: parole.c strint.h
	$(CC) parole.c $(CFLAGS) -c -o parole.o

strint.o: strint.c strint.h
	$(CC) $(CFLAGS) -c $< -o $@
	
# $@: target - file che la regola sta producendo
frequenze: frequenze.o strint.o
	$(CC) $^ $(LDLIBS) -o $@

# $<: prima dipendenza - utile quando abbiamo una sola dipendenza
frequenze.o: frequenze.c strint.h
	$(CC) $< $(CFLAGS) -c -o $@

clean:
	rm -f $(EXECS) *.o

