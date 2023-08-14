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
 * main - Simple shell implementation with custom getline function.
 *
 * Return: Always 0.
 */
int main(void) {
    signal(SIGINT, sig_handler);

    while (1) {
        char input[BUFFER_SIZE];
        char *args[BUFFER_SIZE / 2];
        char *args_ptr = args[0];
        char *token = input;
        ssize_t read_size;
        int argc = 0;
        char *end;

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
                if (argc != 2) {
                    fprintf(stderr, "Usage: exit status\n");
                } else {
                    int exit_status = atoi(args[1]);
                    exit(exit_status);
                }
            } else if (strcmp(command, "env") == 0) {
                print_environment();
            } else if (strcmp(command, "setenv") == 0) {
                if (argc != 3) {
                    fprintf(stderr, "Usage: setenv VARIABLE VALUE\n");
                } else {
                    if (setenv(args[1], args[2], 1) != 0) {
                        perror("setenv");
                    }
                }
            } else if (strcmp(command, "unsetenv") == 0) {
                if (argc != 2) {
                    fprintf(stderr, "Usage: unsetenv VARIABLE\n");
                } else {
                    if (unsetenv(args[1]) != 0) {
                        perror("unsetenv");
                    }
                }
            } else if (strcmp(command, "cd") == 0) {
                if (argc > 2) {
                    fprintf(stderr, "Usage: cd [DIRECTORY]\n");
                } else {
                    char *dir = argc == 2 ? args[1] : getenv("HOME");
                    if (chdir(dir) != 0) {
                        perror("cd");
                    } else {
                        setenv("PWD", dir, 1);
                    }
                }
            } else {
                /* Handle other commands as before */
            }
        }
    }

    return 0;
}

