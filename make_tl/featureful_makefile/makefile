# put here for customization tasks
BINARY=bin
CODEDIRS=. ./lib
INCDIRS=. ./include/


CC=gcc
# generate files that encode make rules for the .h dependencies
DEPFLAGS=-MP -MD
# automatically add the -I onto each include directory
CFLAGS=-std=c11 -Wall -O -g $(foreach D, $(INCDIRS), -I$(D)) $(DEPFLAGS)

# for-style iteration (foreach) and regular expression completions (wildcards)
CFILES=$(foreach D, $(CODEDIRS), $(wildcard $(D)/*.c))
# regular expression replacement
OBJECTS=$(patsubst %.c, %.o, $(CFILES))
DEPFILES=$(patsubst %.c, %.d, $(CFILES))

all: $(BINARY)

$(BINARY): $(OBJECTS)
	$(CC) -o $@ $^

# only want the .c file dependency here, thus $< instead of $^
%.c: %.o
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -rf $(BINARY) $(OBJECTS) $(DEPFILES)

# run clean before distribute
distribute: clean
	tar zcvf dist.tgz *

# @ silences the printing of the command
# $(info...) prints out
diff:
	$(info The status of the repository, and the volume of per-file changes:)
	@git status
	@git diff --stat







