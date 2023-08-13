#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <sys/wait.h>

#define BUFFER_SIZE 1024

extern char **environ;

/* Signal handler for SIGINT (Ctrl+C) */
void sig_handler(int signo) {
    (void)signo;
    write(STDOUT_FILENO, "\n($) ", 5);
    fflush(stdout);
}

int main(void) {
    pid_t pid;
    pid = fork();

    signal(SIGINT, sig_handler);

    while (1) {
        char command[BUFFER_SIZE];

        if (isatty(STDIN_FILENO))
            write(STDOUT_FILENO, "($) ", 5);

        if (fgets(command, sizeof(command), stdin) == NULL) {
            if (isatty(STDIN_FILENO))
                write(STDOUT_FILENO, "\n", 1);
            break;
        }

        command[strlen(command) - 1] = '\0';

        if (pid == -1) {
            perror("fork");
            exit(EXIT_FAILURE);
        }

        if (pid == 0) {
	     char *args[] = {NULL};	

             if (execve(command, args, environ) == -1) {
                perror("execve");
                exit(EXIT_FAILURE);
            }
        } else {
            wait(NULL);
        }
    }

    exit(EXIT_SUCCESS);
}

