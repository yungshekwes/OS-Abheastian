#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>


#define MAX_COMMAND_LENGTH 1000
#define MAX_NUM_ARGS 100

int main() {
    char command[MAX_COMMAND_LENGTH];
    char* args[MAX_NUM_ARGS];

    while (1) {
        printf("Enter a command: ");
        fgets(command, MAX_COMMAND_LENGTH, stdin);

        // Remove trailing newline from input
        command[strcspn(command, "\n")] = '\0';

        // Tokenize input command into arguments
        char* token = strtok(command, " ");
        int i = 0;
        while (token != NULL) {
            args[i++] = token;
            token = strtok(NULL, " ");
        }
        args[i] = NULL;

        // Exit program if user enters "exit"
        if (strcmp(args[0], "exit") == 0) {
            printf("Goodbye!\n");
            exit(0);
        }

        // Execute command using execve
        pid_t pid = fork();
        if (pid == 0) {
            // Child process
            int ret = execve(args[0], args, NULL);
            if (ret == -1) {
                perror("execve");
            }
            exit(1);
        } else if (pid < 0) {
            // Fork failed
            perror("fork");
        } else {
            // Parent process
            int status;
            waitpid(pid, &status, 0);
        }
    }

    return 0;
}