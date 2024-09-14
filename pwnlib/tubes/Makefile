CC=clang
CFLAGS=-W -Wall -ansi -pedantic -std=c99 -g 
INC=-I include/
 
all: swig-all

swig-all:
	swig -c++ -python process.i
	clang++ -O2 -shared -c process.cpp
	clang++ -O2 -shared -c process_wrap.cxx -I"$(PYENV)\include"
	clang++ -shared process.o process_wrap.o -o _process.pyd -L "$(PYENV)\libs" -l python3
	
clean:
	@rm process.o
	@rm process_wrap.o
	@rm process.py
	@rm _process.pyd