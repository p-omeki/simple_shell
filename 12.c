#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <sys/wait.h>

#define BUFFER_SIZE 1024

extern char **environ;

/**
 * sig_handler - Handles the SIGINT signal by printing a prompt.
 * @signo: The signal number.
 */
void sig_handler(int signo) {
    (void)signo;
    write(STDOUT_FILENO, "\n($) ", 5);
    fflush(stdout);
}

/**
 * print_environment - Prints the current environment.
 */
void print_environment(void) {
    char **env = environ;
    while (*env) {
        printf("%s\n", *env);
        env++;
    }
}

/**
 * execute_command - Executes a command and handles logical operators.
 * @args: Array of arguments for the command.
 * @logical_operator: Type of logical operator (0: None, 1: &&, 2: ||).
 * @status: Pointer to the status of the previous command.
 */
void execute_command(char **args, int logical_operator, int *status) {
    pid_t pid = fork();
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
        wait(status);
        if (logical_operator == 1 && *status != 0) {
            *status = -1;
        } else if (logical_operator == 2 && *status == 0) {
            *status = -1;
        }
    }
}

/**
 * main - Simple shell implementation with logical operator handling.
 *
 * Return: Always 0.
 */
int main(void) {
    ssize_t read_size;
    char input[BUFFER_SIZE];
    char *args[BUFFER_SIZE / 2];
    char *args_ptr;
    char *token = input;
    int argc = 0;
    int logical_operator = 0;
    int status = 0;
    char *end;
    int i;

    signal(SIGINT, sig_handler);

    while (1) {
        if (isatty(STDIN_FILENO))
            write(STDOUT_FILENO, "($) ", 5);

        read_size = read(STDIN_FILENO, input, BUFFER_SIZE);
        if (read_size <= 0) {
            if (isatty(STDIN_FILENO))
                write(STDOUT_FILENO, "\n", 1);
            break;
        }
        input[read_size - 1] = '\0';
        end = input + read_size - 1;

        args_ptr = args[0];  /* Initialize args_ptr */

        while (*token == ' ')
            token++;

        while (token < end) {
            char *token_end = strchr(token, ' ');
            if (token_end == NULL)
                token_end = end;

            *token_end = '\0';
            strcpy(args_ptr, token);
            args_ptr += strlen(token) + 1;
            argc++;
            token = token_end + 1;

            while (*token == ' ')
                token++;
        }
        args[argc] = NULL;

        if (argc > 0) {
            char *command = args[0];

            if (strcmp(command, "exit") == 0) {
                exit(EXIT_SUCCESS);
            }

            if (strcmp(command, "env") == 0) {
                print_environment();
                continue;
            }

            for (i = 0; args[i] != NULL; i++) {
                if (strcmp(args[i], "&&") == 0) {
                    logical_operator = 1;
                    args[i] = NULL;
                    break;
                } else if (strcmp(args[i], "||") == 0) {
                    logical_operator = 2;
                    args[i] = NULL;
                    break;
                }
            }

            if (logical_operator) {
                if (status == 0 && logical_operator == 2) {
                    continue;
                } else if (status != 0 && logical_operator == 1) {
                    continue;
                }
            }

            execute_command(args, logical_operator, &status);
        }
    }

    return 0;
}

