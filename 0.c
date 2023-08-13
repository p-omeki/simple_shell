#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>

#define BUFFER_SIZE 1024

void sig_handler(int signo) {
    (void)signo;
    write(STDOUT_FILENO, "\n($) ", 5);
    fflush(stdout);
}

int main(void) {
    char *line = NULL;
    size_t len = 0;

    signal(SIGINT, sig_handler);

    while (1) {

	pid_t pid;
        pid = fork();

        if (isatty(STDIN_FILENO))
            write(STDOUT_FILENO, "($) ", 5);

        if (getline(&line, &len, stdin) == -1) {
            if (isatty(STDIN_FILENO))
                write(STDOUT_FILENO, "\n", 1);
            break;
        }

        if (strcmp(line, "exit\n") == 0) {
            free(line);
            if (isatty(STDIN_FILENO))
                write(STDOUT_FILENO, "\n", 1);
            break;
        }

        line[strlen(line) - 1] = '\0';

        if (pid == -1) {
            perror("fork");
            exit(EXIT_FAILURE);
        }

        if (pid == 0) {
            char *args[] = {NULL};
            char *env[] = {NULL}; 
      
            if (execve(line, args, env) == -1) {
                perror("execve");
                exit(EXIT_FAILURE);
            }
        } else {
            wait(NULL);
        }
    }

    free(line);
    exit(EXIT_SUCCESS);
}

