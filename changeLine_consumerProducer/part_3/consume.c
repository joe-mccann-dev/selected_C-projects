#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/shm.h>
#include "buffer.h"
#include <limits.h>

int main(void)
{
    int id;
    struct bufferStruct* bufferPtr;
    int shouldRun = 1;
    char command;
    int i;

    // find id of shared memory segment produced in produce.c
    if ((id = shmget(key, MSIZ, 0)) < 0)
    {
        perror("Error: getting ID of a shared memory\n");
        exit(-1);
    }

    // attach shared memory segment to buffer struct
    bufferPtr = (struct bufferStruct*)shmat(id, 0, 0);
    // if -1, memory failed to attach to memory segment with shmat
    if (bufferPtr == (struct bufferStruct*)-1) {
        perror("Error: attaching shared memory\n");
        exit(-1);
    }

    bufferPtr->in = 0;
    bufferPtr->out = 0;
    // set to INT_MAX to ignore printing empty buffer slots
    for (int j = 0; j < MAX_SIZE; j++)
    {
        bufferPtr->content[j] = INT_MAX;
    }

    while (shouldRun)
    {
        printf("\nPress enter to consume or 0 to exit: ");
        fflush(stdout);
        command = getchar();

        if (command == '\n')
        {
            if ((bufferPtr->in == bufferPtr->out))
            {
                printf("\n<Buffer is empty>\n");
            }
            else
            {
                int consumed = bufferPtr->content[bufferPtr->out];
                // buffer data consumed, set to -1
                bufferPtr->content[bufferPtr->out] = INT_MAX;
                printf("\nConsumed %d:\n", consumed);

                // next thing to be consumed moves around the "circle"
                bufferPtr->out = (bufferPtr->out + 1) % MAX_SIZE;
                printf("Buffer content: ");
                // since it can only use buffer_size - 1 elements, don't bother printing last -1
                for (i = 0; i < MAX_SIZE; i++)
                {
                    if (bufferPtr->content[i] == INT_MAX)
                    {
                        continue;
                    }
                    else
                    {
                        printf("%d ", bufferPtr->content[i]);
                    }
                }
            }
        }
        else if (command == '0')
        {
            shouldRun = 0;
        }
    }

    // disconnect buffer from shared memory
    if (shmdt(bufferPtr) == -1)
    {
        printf("error encountered detaching from shared memory.\n");
        return EXIT_FAILURE;
    }
}