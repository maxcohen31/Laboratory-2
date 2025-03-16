CC=gcc
INCDIRS=-I.
CFLAGS=-std=c11 -Wall -g -O -pthread $(INCDIRS) 
LDLIBS=-lm -pthread
CFILES=esercizio1.c esercizio2.c esercizio3.c
OBJECTS=esercizio1.o esercizio2.o esercizio3.o
EXEC=esercizio1 esercizio2 esercizio3

all: $(EXEC)

esercizio1: esercizio1.o
	$(CC) -o $@ $^ $(LDLIBS)

esercizio2: esercizio2.o
	$(CC) -o $@ $^ $(LDLIBS)

esercizio3: esercizio3.o
	$(CC) -o $@ $^ $(LDLIBS)

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -rf $(EXEC) $(OBJECTS)
