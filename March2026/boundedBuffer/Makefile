CC		=  gcc
CFLAGS	        += -Wall
INCLUDES        += -I .
OPTFLAGS	= -O3 
LIBS            = -lpthread -lrt

SOURCES           = $(wildcard *.c)
TARGETS          += $(SOURCES:.c=)

.PHONY: all clean cleanall
.SUFFIXES: .c .h

%: %.c
	$(CC) $(CFLAGS) $(INCLUDES) $(OPTFLAGS) -o $@ $< $(LDFLAGS) $(LIBS)

all		: $(TARGETS)

boundedBuffer_fifo: boundedBuffer_fifo.c  ./queue/queue.c
	$(CC) $(INCLUDES) -I ./queue  $(CFLAGS) $(OPTFLAGS) -o $@ $^ $(LDFLAGS) $(LIBS)

clean		: 
	rm -f $(TARGETS)


cleanall	: clean
	rm -f *.o *~



