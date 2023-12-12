CC=clang
CFLAGS=-W -Wall -ansi -pedantic -std=c99 -g 
INC=-I include/
SRC=src/
 
all:
	$(MAKE) -C pwnlib
 
clean:
    $(MAKE) -C  pwnlib clean