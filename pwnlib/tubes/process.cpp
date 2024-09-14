#include <stdio.h>
#include <stdlib.h>
#include <iostream>

#include "process.hpp"


Process::Process(std::string fileToOpen) {
	/**
	* @brief open a file to debug it.
	*
	* @param fileToOpen , the file to execute.
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