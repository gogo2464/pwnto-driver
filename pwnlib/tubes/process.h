typedef struct {
    FILE  *process_PID;
} Process;

Process proc;

void process (char *command);
void recv(int size);
void send();
void libs();
void libc();
void bin();