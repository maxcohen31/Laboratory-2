CC=gcc
CFLAGS=-Wall -Wextra -g -std=c11 -O -pthread
LDLIBS=-lm -pthread

#CFILES=abr.c
OBJECT=mult_files_sem.o  xerrori.o
BINARY=mult_files_sem

all: $(BINARY)

#  $@ viene sostituito con il nome del target
#  $< viene sostituito con il primo prerequisito
#  $^ viene sostituito con tutti i prerequisiti

# versioni di prod/con con semafori oppure condvar
pctest: $(PC_EXEC)

# regola per la creazione degli eseguibili
$(BINARY): $(OBJECT)
	$(CC) $(CFLAGS) -o $@ $^ $(LDLIBS)

# regola per la creazione di file oggetto che dipendono da xerrori.h
%.o: %.c xerrori.h
	$(CC) $(CFLAGS) -c $<

# Condition variables
pctest.o: pctest.c xerrori.h
	$(CC) $(CFLAGS) -O3 -c $< -o $@

# come sopra ma definisce la costante USACV quindi vengono
# usate le CV invece dei semafori nel paradigma prod-cons
pctestCV.o: pctest.c xerrori.h
	$(CC) $(CFLAGS) -O3 -c $<  -o $@ -DUSACV

# versione C11 dei precedenti
pctestC11.out: pctestC11.c semC11.h
	$(CC) $(CFLAGS) -O3  $< -o $@
pctestC11CV.out: pctestC11.c semC11.h
	$(CC) $(CFLAGS) -O3  $< -o $@ -DUSACV


clean:
	rm -f $(BINARY) $(OBJECT)


