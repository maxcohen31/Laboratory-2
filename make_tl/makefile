CC=gcc
INCDIRS=-I.
OPT=-O0
CFLAGS=-std=c11 -Wall -g $(INCDIRS) $(OPT)

CFILES=x.c y.c
OBJECTS=x.o y.o

BINARY=bin

all: $(BINARY)

# binary depends on objects
$(BINARY): $(OBJECTS)
	$(CC) -o $@ $^

# all *.o files depends on all *.c files
%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $^

clean:
	rm -rf $(BINARY) $(OBJECTS)
