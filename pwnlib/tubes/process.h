//#include "tube.h"

#ifdef _WIN32
    #include <windows.h>
#endif

typedef struct {
    char *buffer;
	void *(*pwnto_recv_ptr)(int);
	void (*pwnto_send)(char *input);
	void (*pwnto_close)();
	
#ifdef _WIN32
	PROCESS_INFORMATION process_handle;
	STARTUPINFO process_startup_info;
#elif __linux__
	int fd_in;
	int fd_out;
	enum Tubetype type;
	pidNode *pid_node;
#else
#endif
} Process;

Process *process (char *command);
void *pwnto_recv(int size);
void pwnto_send(char *input);

void libs();
void libc();
void bin();
void pwnto_close();