#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>

#define BUFFER_SIZE 1024

/* Signal handler for SIGINT (Ctrl+C) */
void sig_handler(int signo);

int main(void);

/**
 * sig_handler - Signal handler for SIGINT (Ctrl+C)
 * @signo: Signal number received
 *
 * This function is called when the SIGINT signal (Ctrl+C) is received.
 * It displays a prompt and flushes the standard output.
 */
void sig_handler(int signo)
{
    (void)signo;
    write(STDOUT_FILENO, "\n($) ", 5);
    fflush(stdout);
}

/**
 * main - Entry point of the shell program
 *
 * This function implements a simple shell that takes user commands
 * and executes them using the execve system call.
 *
 * Return: Always returns 0.
 */
int main(void)
{
    char *line = NULL;
    size_t len = 0;

    /* Set up signal handler for SIGINT */
    signal(SIGINT, sig_handler);

    while (1)
    {
        pid_t pid;
        pid = fork();

        /* Print shell prompt if running in terminal */
        if (isatty(STDIN_FILENO))
            write(STDOUT_FILENO, "($) ", 5);

        /* Read input line */
        if (getline(&line, &len, stdin) == -1)
        {
            /* Handle end of input */
            if (isatty(STDIN_FILENO))
                write(STDOUT_FILENO, "\n", 1);
            break;
        }

        /* Check for exit command */
        if (strcmp(line, "exit\n") == 0)
        {
            free(line);
            if (isatty(STDIN_FILENO))
                write(STDOUT_FILENO, "\n", 1);
            break;
        }

        /* Remove newline character from input */
        line[strlen(line) - 1] = '\0';

        if (pid == -1)
        {
            perror("fork");
            exit(EXIT_FAILURE);
        }

        if (pid == 0)
        {
            /* Child process: execute command */
            char *args[] = {NULL};
            char *env[] = {NULL};

            if (execve(line, args, env) == -1)
            {
                perror("execve");
                exit(EXIT_FAILURE);
            }
        }
        else
        {
            /* Parent process: wait for child */
            wait(NULL);
        }
    }

    /* Free allocated memory and exit */
    free(line);
    exit(EXIT_SUCCESS);
}

