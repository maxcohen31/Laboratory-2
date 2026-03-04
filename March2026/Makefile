CC=gcc
CFLAGS=-Wall -Wextra -g -std=c11 -O -pthread
LDLIBS=-lm -pthread

#CFILES=abr.c
OBJECT=semaphore_prod_cons.o xerrori.o
BINARY=semaphore_prod_cons

all: $(BINARY)

# regola per la creazione degli eseguibili
$(BINARY): $(OBJECT)
	$(CC) $(CFLAGS) -o $@ $^ $(LDLIBS)

# regola per la creazione di file oggetto che dipendono da xerrori.h
%.o: %.c xerrori.h
	$(CC) $(CFLAGS) -c $<

clean:
	rm -f $(BINARY) $(OBJECT)


