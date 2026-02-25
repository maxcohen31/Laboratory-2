CC=gcc
CFLAGS=-Wall -Wextra -g -std=c11 -O -pthread
LDLIBS=-lm -pthread

#CFILES=abr.c
OBJECT=contaprimi.o xerrori.o
BINARY=contaprimi.out tabella.out

all: $(BINARY)

$(BINARY): $(OBJECT)
	$(CC) $(CFLAGS) -o $@ $^ $(LDLIBS)

# regola per la creazione di file oggetto che dipendono da xerrori.h
%.o: %.c xerrori.h
	$(CC) $(CFLAGS) -c $<

clean:
	rm -rf $(BINARY) $(OBJECT)


