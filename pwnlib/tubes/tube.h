#include <sys/types.h>
#ifdef _WIN32
    #include <windows.h>
#endif

enum Tubetype
{
    REMOTE_TUBE,
    PROCESS_TUBE
};

typedef struct pidNode
{
    pid_t pid;
    struct pidNode *next;
    struct pidNode *prev;
} pidNode;

typedef struct Tube
{
    char *buffer;

	#ifdef _WIN32
	    PROCESS_INFORMATION process_handle;
	    STARTUPINFO process_startup_info;
    #elif __linux__
	    int fd_in;
        int fd_out;
        enum Tubetype type;
        pidNode *pid_node; 
    #else
        //printf("This code is compiled on an unknown operating system.\n");
    #endif
} Tube;

//void init_all();