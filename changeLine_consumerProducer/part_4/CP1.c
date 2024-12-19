// CP1 manages the source file
// CP2 manages the destination file

// Assume that the buffer used to read/write the files
// is too small to contain the entire file so multiple r/w calls
// are necessary to process the entire file

// CP1:
// creates a shared memory that will be used as a temporary buffer
// accepts source file as command line arg
// opens the source file; reads data from source file
// reads chunk of data from source file
// writes chunk of data to shared memory
// waits for CP2 to read shared memory
// then writes the next part of the source file to the shared memory
// Cycle continues until file is fully copied into shared memory

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/shm.h>
#include <fcntl.h>
#include <sys/types.h>
#include <unistd.h>
#include "buffer.h"

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        printf("Usage: CP1 <source file>\nPlease pass a source file as an argument.\n");
        exit(EXIT_FAILURE);
    }

    int id;
    struct bufferStruct *bufferPtr;
    int shouldRun = 1;
    int i;

    id = shmget(key, MSIZ, IPC_CREAT | 0666);
    if (id < 0)
    {
        perror("Error: creating shared memory\n");
        return EXIT_FAILURE;
    }

    // attach shared memory segment to buffer struct
    bufferPtr = (struct bufferStruct *)shmat(id, 0, 0);
    // if -1, failed to attach buffer to shared memory
    if (bufferPtr == (struct bufferStruct *)-1)
    {
        perror("Error: attaching shared memory\n");
        return EXIT_FAILURE;
    }

    // bufferPtr->in = 0;
    // bufferPtr->out = 0;
    bufferPtr->atEndOfFile = 0;

    FILE *fptr;
    fptr = fopen(argv[1], "r");

    if (fptr == NULL)
    {
        printf("Error opening file\n");
        return EXIT_FAILURE;
    }
    char current;

    while (shouldRun)
    {
        if ((bufferPtr->in + 1) % MAX_SIZE == bufferPtr->out)
        {
            printf("\n . . .Waiting for CP2 to consume buffer . . .\n");
            sleep(1);
            printf("Current buffer contents: ");
            for (i = 0; i < MAX_SIZE; i++)
            {
                printf("%c", bufferPtr->content[i]);
            }
        }
        else
        {
            if ((current = fgetc(fptr)) == EOF)
            {
                // signal to CP2 that CP1 is done writing to shared memory
                bufferPtr->atEndOfFile = 1;
                printf("\nAT END OF FILE\n");
                shouldRun = 0;
            }
            else
            {
                bufferPtr->content[bufferPtr->in] = current;
                bufferPtr->in = (bufferPtr->in + 1) % MAX_SIZE;
            }
        }
    }

    fclose(fptr);

    // disconnect buffer from shared memory
    if (shmdt(bufferPtr) == -1)
    {
        printf("error encountered detaching from shared memory.\n");
        return EXIT_FAILURE;
    }

    return 0;
}