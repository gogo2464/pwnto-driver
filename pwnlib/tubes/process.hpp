#include <stdio.h>
#include <stdlib.h>
#include <iostream>

class Process {
    public:
        Process();
        ~Process();
        void recv(int size);
        void send();
        void libs();
        void libc();
        void bin();
    private:
        int processPid;
};