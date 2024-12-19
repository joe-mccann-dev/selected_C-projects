/**
 * After reading user input, the steps are:
 * (1) fork a child process using fork()
 * (2) the child process will invoke execvp()
 * (3) parent will invoke wait() unless command includes &
 */

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>

#define MAX_LINE 80

int main(void) {
    char* args[MAX_LINE / 2 + 1];
    char command[MAX_LINE];
    int shouldRun = 1;
    pid_t child;

    while (shouldRun) {
        printf("osh> ");
        fflush(stdout);

        // read user input
        fgets(command, sizeof(command), stdin);
        int inputLength = strlen(command);

        // remove newline character at end of input
        if ((inputLength > 0) && (command[inputLength - 1] == '\n')) {
            command[strlen(command) - 1] = '\0';
        }

        // referenced https://www.youtube.com/watch?v=34DnZ2ewyZo 
        // to figure out how to split strings in C
        int argIndex = 0;
        char* token = strtok(command, " ");
        while (token != NULL) {
            // printf(" %s\n", token);
            args[argIndex] = token;
            token = strtok(NULL, " ");
            argIndex++;
        }

        // argIndex points to one after last argument
        args[argIndex] = NULL;

        if (strcmp("exit", command) == 0) {
            shouldRun = 0;
            continue;
        }

        if (args[0] == NULL) {
            continue;
        }

        // * (1) fork a child process using fork()
        child = fork();
        if (child < 0) {
            printf("Process failed to fork.\n");
            return 1;
        }
        // (2) the child process will invoke execvp()
        if (child == 0) {
            // printf("Inside child process %d\n", getpid());
            if (strcmp(args[argIndex - 1], "&") == 0) {
                args[argIndex - 1] = NULL;
            }

            execvp(args[0], args);
            fprintf(stderr, "unknown command: %s\n", args[0]);
            exit(EXIT_FAILURE);
        }
        // (3) parent will invoke wait() unless command includes &
        else {
            if (strcmp(args[argIndex - 1], "&") != 0) {
                waitpid(child, NULL, 0);
                // printf("parent process %d waited for child to complete\n", getpid());
            }
            else {
                printf(" . . . Background job processing . . .\n");
                fflush(stdout);
                sleep(1);
            }
        }
    }

    return 0;
}
