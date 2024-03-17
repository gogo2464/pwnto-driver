%module process
%{
#include "process.h"
%}

%include "process.h"

%extend Process {
  Process(char *command) {
    Process *proc;
    proc = (Process *) malloc(sizeof(Process));

    printf("process created!");
    return proc;
  }
  ~Process() {
    free($self);
  }
  char *recv(int size) {
	    char *buff;
	    fgets(buff, sizeof(size), $self->process_PID);
      return buff;
  }
};