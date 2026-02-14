CC=gcc
CFLAGS=-Wall -Wextra -g -std=c11 -W -O 

CFILES=abr.c
OBJECT=abr.o
BINARY=abr

all: $(BINARY)

$(BINARY): $(OBJECT)
	$(CC) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $^

clean:
	rm -rf $(BINARY) $(OBJECT)


