CC=clang
CFLAGS=-W -Wall -ansi -pedantic -std=c99 -g 
INC=-I include/
 
all: swig-all

swig-all:
	swig -o process_wrap.cpp -c++ -python process.i
	g++ -std=c++11 -O2 -fPIC -c process.cpp
	g++ -std=c++11 -O2 -fPIC -c process_wrap.cpp -I"$(PYENV)\include"
	g++ -std=c++11 -shared process.o process_wrap.o -o _process.pyd -L "$(PYENV)\libs" -l python3
	
clean:
	@del process.o
	@del process_wrap.o
	@del process_wrap.cpp
	@del process.py
	@del _process.pyd

# -l python38