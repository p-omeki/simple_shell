#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <sys/wait.h>

#define BUFFER_SIZE 1024

extern char **environ;

void sig_handler(int signo) {
    (void)signo;
    write(STDOUT_FILENO, "\n($) ", 5);
    fflush(stdout);
}

int main(void) {
    int command_found = 0;
/*    char *command = args[0];*/
    char *path = getenv("PATH");
    char *path_token = strtok(path, ":");
    int argc = 0;
    char input[BUFFER_SIZE];
    pid_t pid;
    pid = fork();
    signal(SIGINT, sig_handler);

    while (1) {
        char *token = strtok(input, " ");
   /*     char input[BUFFER_SIZE];*/
        char *args[BUFFER_SIZE / 2];

        if (isatty(STDIN_FILENO))
            write(STDOUT_FILENO, "($) ", 5);

        if (fgets(input, sizeof(input), stdin) == NULL) {
            if (isatty(STDIN_FILENO))
                write(STDOUT_FILENO, "\n", 1);
            break;
        }

        input[strlen(input) - 1] = '\0';


        while (token != NULL) {
            args[argc++] = token;
            token = strtok(NULL, " ");
        }
        args[argc] = NULL;

        if (argc > 0) {
            char *command = args[0];
   
	while (path_token != NULL) {
            char executable_path[BUFFER_SIZE];
            snprintf(executable_path, sizeof(executable_path), "%s/%s", path_token, command);
            if (access(executable_path, X_OK) == 0) {
                command_found = 1;
                break;
            }
            path_token = strtok(NULL, ":");
        }

        if (command_found) {
   
            if (pid == -1) {
                perror("fork");
                exit(EXIT_FAILURE);
            }

            if (pid == 0) {
                if (execve(args[0], args, environ) == -1) {
                    perror("execve");
                    exit(EXIT_FAILURE);
                }
            } else {
                wait(NULL);
            }
        } else {
            fprintf(stderr, "Command not found: %s\n", command);
        }
    }

    exit(EXIT_SUCCESS);
    return 0;
}

