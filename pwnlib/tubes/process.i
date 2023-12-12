%module process
%{
#ifdef _MSC_VER
#define SWIG_PYTHON_INTERPRETER_NO_DEBUG
#endif

#include "process.h"
%}

void send();