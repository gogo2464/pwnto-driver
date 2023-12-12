CC=clang
CFLAGS=-W -Wall -ansi -pedantic -std=c99 -g 
INC=-I include/
 
all: swig-all

swig-all:
	swig -python process.i
	gcc -O2 -fPIC -c process.c
	gcc -O2 -fPIC -c process_wrap.c -I C:\Python38\include
	gcc -shared process.o process_wrap.o -o _process.pyd -L C:\Python38\libs -l python3
	
clean:
	del process.o
	del process_wrap.o
	del process.py
	del _process.pyd
	
	
#clang -shared -o process.dll -IC:\Python38\include process.c process_wrap.c -LC:\Python38\libs\python38.lib -lpython3#clang -shared -o process.dll -IC:\Python38\include process.c process_wrap.c -LC:\Python38\libs\python38.lib -lpython3