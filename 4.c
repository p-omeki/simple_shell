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
    signal(SIGINT, sig_handler);

    while (1) {
        char input[BUFFER_SIZE];
        char *args[BUFFER_SIZE / 2];
        char *token;
        int argc = 0;
        pid_t pid;
        char *path = getenv("PATH");
        char *path_token;

        if (isatty(STDIN_FILENO))
            write(STDOUT_FILENO, "($) ", 5);

        if (fgets(input, sizeof(input), stdin) == NULL) {
            if (isatty(STDIN_FILENO))
                write(STDOUT_FILENO, "\n", 1);
            break;
        }

        input[strlen(input) - 1] = '\0';

        token = strtok(input, " ");

        while (token != NULL) {
            args[argc++] = token;
            token = strtok(NULL, " ");
        }
        args[argc] = NULL;

        if (argc > 0) {
            char *command = args[0];

            if (strcmp(command, "exit") == 0) {
                exit(EXIT_SUCCESS);
            }

            path_token = strtok(path, ":");

            while (path_token != NULL) {
                char executable_path[BUFFER_SIZE];
                snprintf(executable_path, sizeof(executable_path), "%s/%s", path_token, command);
                if (access(executable_path, X_OK) == 0) {
                    break;
                }
                path_token = strtok(NULL, ":");
            }

            pid = fork();

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
        }
    }

    return 0;
}

