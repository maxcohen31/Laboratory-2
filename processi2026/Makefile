CC=gcc 
CFLAGS=-std=c11 -Wall -g -O -pthread
LDLIBS=-lm -pthread -lrt

EXECS:moria.out

all: $(EXECS)

%.out: %.o xerrori.o
	$(CC) $(LDFLAGS) -o $@ $^ $(LDLIBS)

%.o: %.c xerrori.h


clean:
	rm *.o $(EXECS)
