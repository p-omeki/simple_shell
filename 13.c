#include <stdio.h>
#include <string.h>

#define MAX_ALIASES 100

typedef struct Alias {
    char name[50];
    char value[100];
} Alias;

Alias aliases[MAX_ALIASES];
int num_aliases = 0;

/**
 * add_alias - Adds or updates an alias in the aliases array.
 * @name: The name of the alias.
 * @value: The value of the alias.
 */
void add_alias(const char *name, const char *value) {
    int i;
    for (i = 0; i < num_aliases; i++) {
        if (strcmp(aliases[i].name, name) == 0) {
            strcpy(aliases[i].value, value);
            return;
        }
    }

    if (num_aliases < MAX_ALIASES) {
        strcpy(aliases[num_aliases].name, name);
        strcpy(aliases[num_aliases].value, value);
        num_aliases++;
    }
}

/**
 * print_aliases - Prints aliases based on the specified name.
 * @name: The name of the alias to print (NULL to print all aliases).
 */
void print_aliases(const char *name) {
    if (name == NULL) {
        int i;
        for (i = 0; i < num_aliases; i++) {
            printf("%s='%s'\n", aliases[i].name, aliases[i].value);
        }
    } else {
        int i;
        for (i = 0; i < num_aliases; i++) {
            if (strcmp(aliases[i].name, name) == 0) {
                printf("%s='%s'\n", aliases[i].name, aliases[i].value);
                return;
            }
        }
    }
}

int main() {
    /* Your shell initialization code */
    
    while (1) {
        /* Read user input */
        char input[100]; 
        fgets(input, sizeof(input), stdin);
        input[strcspn(input, "\n")] = '\0'; 
        
        if (strcmp(input, "alias") == 0) {
            print_aliases(NULL);
        } else if (strstr(input, "=") != NULL) {
            char *name = strtok(input, "=");
            char *value = strtok(NULL, "\0");
            add_alias(name, value);
        } else {
            char *name = strtok(input, " ");
            print_aliases(name);
        }
        
        /* Execute other commands and handle logic */
        
    }
    
    return 0;
}

