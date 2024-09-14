CC=clang
CFLAGS=-W -Wall -ansi -pedantic -std=c99 -g 
INC=-I include/
 
all: swig-all

swig-all:
	swig -c++ -python process.i
	clang++ -O2 -shared -c process.cpp --target=x86_64-pc-windows-msvc
	clang++ -O2 -shared -c process_wrap.cxx -I"$(PYENV)\include" --target=x86_64-pc-windows-msvc
	clang++ -shared process.o process_wrap.o -o _process.pyd -L "$(PYENV)\libs" -l python3 --target=x86_64-pc-windows-msvc
	
clean:
	@rm process.o
	@rm process_wrap.o
	@rm process.py
	@rm _process.pyd