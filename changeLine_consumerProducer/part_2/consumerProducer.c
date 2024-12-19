// Write a single C program where simple items (e.g., int) are produced and consumed.

// The user uses the keyboard to control the production and consumption of items by entering:

// ‘p’:
// Produce an item if the buffer has space.
// Prints a <Buffer is full> message if the buffer is full.

// c’:
// Consume the oldest item in the buffer.
// Print a <Buffer is empty> message if the buffer is empty.

// - ‘0’: terminate the program.

// Notes:
// Use a bounded buffer: The producer writes to the bounded buffer and the consumer reads from it.

// Production should be limited to the buffer size after which no new items should be produced.

//  If the buffer is empty, then nothing can be consumed.
// Use 2 variables/pointers (in, out) to synchronize the use of the buffer:

// When in = out: buffer is empty of new items, and the reader should stop.
// When in + 1 = out: buffer is full, and the writer should stop.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <limits.h>

#define MAX_SIZE 8
#define MAX_LINE 10

typedef struct bufferStruct
{
    int in;
    int out;
    int content[MAX_SIZE]; // will hold items produced (single int)

} bufferStruct;

int main(int argc, char *argv[])
{
    char command;

    bufferStruct buffer;
    buffer.in = 0;
    buffer.out = 0;
    for (int j = 0; j < MAX_SIZE; j++)
    {
        buffer.content[j] = INT_MAX;
    }

    srand(time(NULL));
    int randomInt;
    int consumed;
    int shouldRun = 1;

    int i;

    while (shouldRun)
    {
        // read user input
        printf("\nEnter p to produce, c to consume, or 0 to exit: ");
        fflush(stdout);
        command = getchar();
        // ignore newline so 'p' gets read on current iteration
        while (getchar() != '\n')
            ;

        if (command == 'p')
        {
            if ((buffer.in + 1) % MAX_SIZE == buffer.out)
            {
                printf("\n<Buffer is full>\n");
            }
            else
            {
                randomInt = rand() % 51;
                buffer.content[buffer.in] = randomInt;
                // next thing to be input will be at next spot "in circle"
                buffer.in = (buffer.in + 1) % MAX_SIZE;
                printf("\nProduced: %d\n", randomInt);
                printf("Buffer content: ");
                for (i = 0; i < MAX_SIZE; i++)
                {
                    if (buffer.content[i] == INT_MAX)
                    {
                        continue;
                    }
                    else
                    {
                        printf("%d ", buffer.content[i]);
                    }
                }
            }
        }

        else if (command == 'c')
        {
            if ((buffer.in == buffer.out))
            {
                printf("\n<Buffer is empty>\n");
            }
            else
            {
                consumed = buffer.content[buffer.out];
                // buffer data consumed, set to -1
                buffer.content[buffer.out] = INT_MAX;
                printf("\nConsumed %d:\n", consumed);

                // next thing to be consumed moves around the "circle"
                buffer.out = (buffer.out + 1) % MAX_SIZE;
                printf("Buffer content: ");
                for (i = 0; i < MAX_SIZE; i++)
                {
                    if (buffer.content[i] == INT_MAX)
                    {
                        continue;
                    }
                    else
                    {
                        printf("%d ", buffer.content[i]);
                    }
                }
            }
        }

        else if (command == '0')
        {
            shouldRun = 0;
        }
    }

    return 0;
}