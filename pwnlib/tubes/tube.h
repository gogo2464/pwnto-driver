#include <sys/types.h>

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
    int fd_in;
    int fd_out;
    enum Tubetype type;
    pidNode *pid_node;
} Tube;

void init_all();