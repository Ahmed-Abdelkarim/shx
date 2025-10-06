#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#define BUFFER_SIZE 1024
#define DELIM " \t\r\n\a"
char* read_line(void){
    int c;
    int size = BUFFER_SIZE;
    int pos = 0;
    char* buffer = malloc(sizeof(char) * size);
    if (buffer == NULL) {
        perror("Failed to allocate buffer");
        exit(EXIT_FAILURE);
    }
    while ((c = getchar()) != '\n' && c != EOF) {
        if (pos >= size) {
            size += BUFFER_SIZE;
            buffer = realloc(buffer, size * sizeof(char));
            if (buffer == NULL) {
                perror("Failed to reallocate buffer");
                exit(EXIT_FAILURE);
            }
        }
        buffer[pos++] = c;
    }
    buffer[pos] = '\0';

    return buffer;
}

char** parse_line(char* line) {
    int size = BUFFER_SIZE;
    int pos = 0;
    char** args = malloc(size * sizeof(char*));
    char* arg;
    if (!args) {
        perror("Failed to allocate args");
        exit(EXIT_FAILURE);
    }
    arg = strtok(line, DELIM);
    while (arg != NULL) {
        if (pos >= size) {
            size += BUFFER_SIZE;
            args = realloc(args, size * sizeof(char*));
            if (!args) {
                perror("Failed to reallocate args");
                exit(EXIT_FAILURE);
            }
        }
        args[pos++] = arg;
        arg = strtok(NULL, DELIM);
    }
    args[pos] = NULL;
    return args;
}

void vlog(const char *message) {
    printf("[LOG]: %s\n", message);
}

void initialize_shell() {
    vlog("Shell initialized.");
}

int execute_command(char **args) {
    printf("Executing command: %s\n", args[0]);
    
    pid_t pid, wpid;
    int status;
    pid = fork();
    if (pid == 0) {
        // Child process
        if (execvp(args[0], (char * const *)args) == -1) {
            perror("shx");
        }
        exit(EXIT_FAILURE);
    } else if (pid < 0) {
        // Error forking
        perror("shx");
    } else {
        // Parent process
        do {
            wpid = waitpid(pid, &status, WUNTRACED);
        } while (!WIFEXITED(status) && !WIFSIGNALED(status));
    }

    vlog("Command executed successfully.");
    return 1;
}

void loop() {
    char * line;
    char ** args;
    while (1) {
        printf("shx> ");
        line = read_line();
        args = parse_line(line);
        execute_command(args);
        free(line);
        free(args);
    }
}

int main(int argc, char *argv[]) {

    // Initialize the shell
    initialize_shell();

    // loop to process commands
    loop();

    // Cleanup and exit
    return EXIT_SUCCESS;
}