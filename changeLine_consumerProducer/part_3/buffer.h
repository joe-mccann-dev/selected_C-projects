#define MAX_SIZE 8

key_t key = 777;

struct bufferStruct
{
    int in;
    int out;
    int content[MAX_SIZE]; // will hold items produced (single int)
};

#define MSIZ sizeof(struct bufferStruct)