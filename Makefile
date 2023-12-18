CC=clang
CFLAGS=-W -Wall -ansi -pedantic -std=c99 -g
PYENV?=C:\Python38

test:
	$(MAKE) -C testing test

all:
	$(MAKE) -C pwnlib
 
clean:
	$(MAKE) -C pwnlib clean