#include "tube.h"

typedef struct {
    FILE  *process_PID;
} Process;

Tube *process (char *command);
#define recv win_recv
void *recv(int size);
#undef recv

#define send win_send
void send();
#undef send

void libs();
void libc();
void bin();