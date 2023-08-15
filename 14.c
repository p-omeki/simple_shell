#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#define BUFFER_SIZE 1024

extern char **environ;

/**
 * get_variable_value - Gets the value of a specified variable.
 * @variable: The name of the variable.
 * Return: The value of the variable.
 */
const char *get_variable_value(const char *variable) {
    if (strcmp(variable, "$?") == 0) {
        return "0"; /* Replace with actual exit status value */
    } else if (strcmp(variable, "$$") == 0) {
        static char pid_str[16]; /* Max length of a pid */
        snprintf(pid_str, sizeof(pid_str), "%d", getpid());
        return pid_str;
    } else {
        return getenv(variable);
    }
}

/**
 * main - Simple shell implementation with variable replacement.
 *
 * Return: Always 0.
 */
int main(void) {
    char input[BUFFER_SIZE];
    char output[BUFFER_SIZE];

    while (1) {
        if (isatty(STDIN_FILENO))
            write(STDOUT_FILENO, "($) ", 5);

        ssize_t read_size = read(STDIN_FILENO, input, BUFFER_SIZE);
        if (read_size <= 0) {
            if (isatty(STDIN_FILENO))
                write(STDOUT_FILENO, "\n", 1);
            break;
        }
        input[read_size - 1] = '\0';

        char *output_ptr = output;
        const char *input_ptr = input;
        char variable[64]; /* Max variable length */

        while (*input_ptr != '\0') {
            if (*input_ptr == '$') {
                input_ptr++; /* Skip the '$' */
                int i = 0;
                while ((*input_ptr >= 'a' && *input_ptr <= 'z') ||
                       (*input_ptr >= 'A' && *input_ptr <= 'Z') ||
                       (*input_ptr >= '0' && *input_ptr <= '9') ||
                       (*input_ptr == '_')) {
                    variable[i] = *input_ptr;
                    input_ptr++;
                    i++;
                }
                variable[i] = '\0';
                const char *value = get_variable_value(variable);
                strcpy(output_ptr, value);
                output_ptr += strlen(value);
            } else {
                *output_ptr = *input_ptr;
                output_ptr++;
                input_ptr++;
            }
        }
        *output_ptr = '\0';

        /* Print the replaced input for demonstration */
        printf("Replaced: %s\n", output);
    }

    return 0;
}

