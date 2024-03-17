#ifndef __unix__
    #include <windows.h>
#endif

typedef struct {
#ifdef _WIN32
	PROCESS_INFORMATION process_handle;
	STARTUPINFO process_startup_info;
#elif __linux__
	int fd_in;
	int fd_out;
#else
#endif
    FILE  *process_PID;
} Process;

void process(char *cmd);
//char *recv(int size);
//void send();
void libs();
void libc();
void bin();