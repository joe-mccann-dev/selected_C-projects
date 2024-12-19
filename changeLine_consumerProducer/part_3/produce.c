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
    struct bufferStruct *bufferPtr;
    int shouldRun = 1;
    char command;
    int randomInt;
    int i;

    // generate random seed for produced randomInt
    srand(time(NULL));

    id = shmget(key, MSIZ, IPC_CREAT | 0666);
    if (id < 0)
    {
        perror("Error: creating shared memory\n");
        exit(-1);
    }

    // attach shared memory segment to buffer struct
    bufferPtr = (struct bufferStruct *)shmat(id, 0, 0);
    // if -1, failed to attach buffer to shared memory
    if (bufferPtr == (struct bufferStruct *)-1)
    {
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
        printf("\nPress enter to produce or 0 to exit: ");
        fflush(stdout);
        command = getchar();

        if (command == '\n')
        {
            if ((bufferPtr->in + 1) % MAX_SIZE == bufferPtr->out)
            {
                printf("\n<Buffer is full>\n");
            }
            else
            {
                // produce an int between 0 and 50
                randomInt = rand() % 51;
                // place produced int in buffer
                bufferPtr->content[bufferPtr->in] = randomInt;
                // next thing to be input will be at next spot "in circle"
                bufferPtr->in = (bufferPtr->in + 1) % MAX_SIZE;
                printf("\nProduced: %d\n", randomInt);
                printf("Buffer content: ");
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
