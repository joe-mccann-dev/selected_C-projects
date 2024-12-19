// CP1 manages the source file
// CP2 manages the destination file

// Assume that the buffer used to read/write the files
// is too small to contain the entire file so multiple r/w calls
// are necessary to process the entire file

// CP2:
// opens an already-created shared memory that will be used as a temporary buffer.
// writes to the destination file that's passed as a command line arg
// reads a chunk of data from the shared memory and writes it to the destination file
// Waits for CP1 to write the next part of the source file
// before it reads it from the shared memory and writes to the destination file.
// Cycle continues until the file is fully copied and EOF is reached
// Detaches and deletes shared memory after writing is complete

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
        printf("Usage: CP2 <destination file>\nPlease pass a destination file as an argument.\n");
        exit(EXIT_FAILURE);
    }

    int id;
    struct bufferStruct *bufferPtr;
    int shouldRun = 1;
    int i;

    if ((id = shmget(key, MSIZ, 0)) < 0)
    {
        perror("Error: getting ID of a shared memory\n");
        return EXIT_FAILURE;
    }

    bufferPtr = (struct bufferStruct *)shmat(id, 0, 0);
    // if -1, memory failed to attach to memory segment with shmat
    if (bufferPtr == (struct bufferStruct *)-1)
    {
        perror("Error: attaching shared memory\n");
        return EXIT_FAILURE;
    }

    FILE *fptr;
    fptr = fopen(argv[1], "w");

    if (fptr == NULL)
    {
        printf("Error opening file\n");
        return EXIT_FAILURE;
    }

    char current;
    while (shouldRun || bufferPtr->in != bufferPtr->out)
    {
        if ((bufferPtr->in == bufferPtr->out))
        {
            printf("\n . . .Waiting for CP1 to write to buffer . . .\n");
            sleep(1);
        }
        else
        {
            // read from buffer
            char consumed = bufferPtr->content[bufferPtr->out];
            // write consumed item to file
            if (fprintf(fptr, "%c", consumed) < 0)
            {
                printf("Error writing to file\n");
                break;
            }
            bufferPtr->out = (bufferPtr->out + 1) % MAX_SIZE;
        }
        if (bufferPtr->atEndOfFile)
        {
            shouldRun = 0;
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