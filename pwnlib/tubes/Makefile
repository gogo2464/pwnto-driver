CC=clang
CFLAGS=-W -Wall -ansi -pedantic -std=c99 -g
INC=-I include/
 
all: swig-all

swig-all:
	swig -python process.i
	gcc -O2 -fPIC -c process.c
	gcc -O2 -fPIC -c process_wrap.c -I"$(PYENV)\include"
	gcc -shared process.o process_wrap.o -o _process.pyd -L "$(PYENV)\libs" -lpython3
	
clean:
	@rm process.o
	@rm process_wrap.o
	@rm process.py
	@rm _process.pyd