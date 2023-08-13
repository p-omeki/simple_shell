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
    int argc = 0;
    char input[BUFFER_SIZE];
    pid_t pid;
    pid = fork();
    signal(SIGINT, sig_handler);

    while (1) {
	char *token = strtok(input, " ");    
/*	char input[BUFFER_SIZE];*/
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

    exit(EXIT_SUCCESS);
}

