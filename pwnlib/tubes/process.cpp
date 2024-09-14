#include <stdio.h>
#include <stdlib.h>
#include <iostream>

#include "process.hpp"


Process::Process() {
	/**
	* @brief open a file.
	*
	* @param file , the file to execute.
	* @return Nothing.
	*
	* @test
	* File * fdesc = process("python");
	* //CHECK(p.recv(5) == "Hello");
	*/
	
	printf("executing...\n");
}

Process::~Process() {
    std::cout << " destroying process." << std::endl;
}

void Process::recv(int size) {

}

void Process::send() {

}

void Process::libs() {

}

void Process::libc() {

}

void Process::bin() {

}

/*
void recv(int size) {
	char *buff;
	fgets(buff, sizeof(size), proc.process_PID);
}

*/