#include <sys/ipc.h>
#define MAX_SIZE 64

key_t key = 98765;

struct bufferStruct
{
    int in;
    int out;
    char content[MAX_SIZE];
    int atEndOfFile;
};

#define MSIZ sizeof(struct bufferStruct)