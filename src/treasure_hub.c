#define _POSIX_C_SOURCE 200809L
#define _DEFAULT_SOURCE
#include "../include/treasure_manager.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <dirent.h>

#define COLOR_RED     "\x1b[31m"
#define COLOR_GREEN   "\x1b[32m"
#define COLOR_YELLOW  "\x1b[33m"
#define COLOR_BLUE    "\x1b[34m"
#define COLOR_MAGENTA "\x1b[35m"
#define COLOR_CYAN    "\x1b[36m"
#define COLOR_RESET   "\x1b[0m"

#define MAX_INPUT_SIZE 256
#define PARAM_FILE "monitor_command.txt"

int monitor_pid = -1;

void print_hub_banner() {
    printf("\n");
    printf(COLOR_YELLOW);
    printf("████████ ██████  ███████  █████  ███████ ██    ██ ██████  ███████     ██   ██ ██    ██ ██████ \n");
    printf("   ██    ██   ██ ██      ██   ██ ██      ██    ██ ██   ██ ██          ██   ██ ██    ██ ██   ██ \n");
    printf("   ██    ██████  █████   ███████ ███████ ██    ██ ██████  █████       ███████ ██    ██ ██████  \n");
    printf("   ██    ██   ██ ██      ██   ██      ██ ██    ██ ██   ██ ██          ██   ██ ██    ██ ██   ██ \n");
    printf("   ██    ██   ██ ███████ ██   ██ ███████  ██████  ██   ██ ███████     ██   ██  ██████  ██████  \n");
    printf("\n");
    printf(COLOR_RESET);
}                                                                                               

//Signal handlers
void handle_list_hunts(int sig) {
    printf("Listing hunts...\n");

    DIR *dir = opendir("hunts");

    if(!dir) {
        perror(COLOR_RED "Failed to open hunts directory" COLOR_RESET);
        return;
    }

    struct dirent *entry;
    while((entry = readdir(dir)) != NULL) {
        if(entry->d_type == DT_DIR &&
           strcmp(entry->d_name, ".") != 0 &&
           strcmp(entry->d_name, "..") != 0) {
            printf(" Found hunt: " COLOR_YELLOW "%s\n" COLOR_RESET, entry->d_name);
        }
    }
    closedir(dir);

    // Flush the output to ensure the prompt appears correctly
    fflush(stdout);
}

void handle_list_treasure(int sig) {
    printf("Listing treasures...\n");

    FILE *file = fopen(PARAM_FILE, "r");
    if(!file) {
        perror(COLOR_RED "Failed to open parameter file" COLOR_RESET);
        return;
    }

    char hunt_id[256];
    if(fgets(hunt_id, sizeof(hunt_id), file)){
        hunt_id[strcspn(hunt_id, "\n")] = '\0'; // trim newline
        printf("Hunt ID: %s\n", hunt_id);

        // Call the function to read treasure
        read_treasure(hunt_id);

    }else{
        printf(COLOR_RED "No hunt_id found in file.\n" COLOR_RESET);
    }
    fclose(file);
}

void handle_view_treasure(int sig) {
    printf("Viewing treasures...\n");

    FILE *file = fopen(PARAM_FILE, "r");
    if(!file) {
        perror(COLOR_RED "Failed to open parameter file" COLOR_RESET);
        return;
    }

    char hunt_id[256], treasure_id[256];
    if(fgets(hunt_id, sizeof(hunt_id), file) &&
       fgets(treasure_id, sizeof(treasure_id), file)){
        hunt_id[strcspn(hunt_id, "\n")] = '\0'; // trim newline
        treasure_id[strcspn(treasure_id, "\n")] = '\0'; // trim newline

        printf("Hunt ID: %s\n", hunt_id);
        printf("Treasure ID: %s\n", treasure_id);

        // Call the function to view treasure
        int id = atoi(treasure_id); // Example treasure ID
        view_treasure(hunt_id, id);

    }else{
        printf(COLOR_RED "Invalid format in monitor_command.txt.\n" COLOR_RESET);
    }
    fclose(file);
}

// Function to handle SIGUSR1 signal
void handle_sigterm(int sig) {
    printf("Terminating...\n");
    sleep(5);
    printf("Terminated.\n");
    exit(0);
}

//Monitor process main loop
void start_monitor_loop() {
    struct sigaction sa1, sa2, sa_view, sa_term;

    sa1.sa_handler = handle_list_hunts;
    sigemptyset(&sa1.sa_mask);
    sa1.sa_flags = 0;
    sigaction(SIGUSR1, &sa1, NULL);

    sa2.sa_handler = handle_list_treasure;
    sigemptyset(&sa2.sa_mask);
    sa2.sa_flags = 0;
    sigaction(SIGUSR2, &sa2, NULL);
    
    sa_view.sa_handler = handle_view_treasure;
    sigemptyset(&sa_view.sa_mask);
    sa_view.sa_flags = 0;
    sigaction(SIGALRM, &sa_view, NULL);

    sa_term.sa_handler = handle_sigterm;
    sigemptyset(&sa_term.sa_mask);
    sa_term.sa_flags = 0;
    sigaction(SIGTERM, &sa_term, NULL);

    //printf("[Monitor] Ready and waiting for signals (PID: %d)\n", getpid());

    while (1) {
        pause(); // Wait for signals
    }
} 

// At the top of treasure_hub.c
int pipefd[2];


void read_monitor_output() {
    char buffer[256];
    ssize_t nbytes = read(pipefd[0], buffer, sizeof(buffer) - 1);
    if (nbytes > 0) {
        buffer[nbytes] = '\0'; // Null-terminate the string
        printf("Monitor Output:" COLOR_GREEN "%s" COLOR_RESET, buffer);
    } else {
        printf(COLOR_RED "No response from monitor.\n" COLOR_RESET);
    }
}


int main() {
    char input[MAX_INPUT_SIZE];
    bool monitor_running = false;

    //printf(" Welcome to Treasure Hub \n");
    print_hub_banner();
    printf(" Type 'help' for a list of commands.\n");

    while (1) {
        printf(COLOR_MAGENTA "treasure_hub> " COLOR_RESET);
        fflush(stdout);

        // Read user input
        if (fgets(input, sizeof(input), stdin) == NULL) {
            printf("Error reading input.\n");
            continue;
        }

        // Remove newline character
        input[strcspn(input, "\n")] = '\0';

        // Handle commands
        if (strcmp(input, "start_monitor") == 0) {
            if (monitor_pid > 0) {
                printf(COLOR_BLUE "Monitor is already running (PID %d).\n" COLOR_RESET, monitor_pid);
                //printf("\n");
            } else {
                //pid_t pid = fork();

                //int pipefd[2];
                if (pipe(pipefd) == -1) {
                    perror(COLOR_RED "Pipe creation failed" COLOR_RESET);
                    continue;
                }

                pid_t pid = fork();
                if(pid < 0) {
                    perror("Fork failed");
                }else if(pid == 0) {
                    close(pipefd[0]); // Close read end of the pipe in the child process
                    dup2(pipefd[1], STDOUT_FILENO); // Redirect stdout to the pipe
                    close(pipefd[1]); // Close write end of the pipe in the child process

                    start_monitor_loop(); // Start the monitor loop
                    exit(0); 
                                 
                }
                else {
                    // Parent (hub)
                    close(pipefd[1]); // Close write end of the pipe in the parent process
                    monitor_pid = pid;
                    printf("Monitor started with PID %d.\nReady and waiting for signals\n", monitor_pid);
                }
            }
        }

        else if (strcmp(input, "list_hunts") == 0){
            if(monitor_pid > 0){
                kill(monitor_pid, SIGUSR1);
                read_monitor_output();
            }else{
                printf(COLOR_BLUE "Monitor is not running.\n" COLOR_RESET);
            }
        }
        
        else if (strcmp(input, "list_treasures") == 0) {
            if(monitor_pid > 0){
                char hunt_id[256];
                printf(COLOR_BLUE "Enter hunt ID: " COLOR_RESET);
                fgets(hunt_id, sizeof(hunt_id), stdin);
                hunt_id[strcspn(hunt_id, "\n")] = '\0'; // trim newline

                FILE *file = fopen(PARAM_FILE, "w");
                if(file) {
                    fprintf(file, "%s\n", hunt_id);
                    fclose(file);
                }

                kill(monitor_pid, SIGUSR2);
                read_monitor_output();
            }else{
                printf(COLOR_BLUE "Monitor is not running.\n" COLOR_RESET);
            }
        }

        else if(strcmp(input, "view_treasure") == 0) {
            if(monitor_pid > 0){
                char hunt_id[256], treasure_id[256];

                printf(COLOR_BLUE "Enter hunt ID: " COLOR_RESET);
                fgets(hunt_id, sizeof(hunt_id), stdin);
                hunt_id[strcspn(hunt_id, "\n")] = '\0'; // trim newline

                printf(COLOR_BLUE "Enter treasure ID: " COLOR_RESET);
                fgets(treasure_id, sizeof(treasure_id), stdin);
                treasure_id[strcspn(treasure_id, "\n")] = '\0'; // trim newline

                FILE *file = fopen(PARAM_FILE, "w");
                if(file) {
                    fprintf(file, "%s\n%s\n", hunt_id, treasure_id);
                    fclose(file);
                }
                else {
                    printf(COLOR_RED "Failed to open parameter file.\n" COLOR_RESET);
                    continue;
                }

                kill(monitor_pid, SIGALRM); // Send signal to view treasure
                read_monitor_output();
            }else{
                printf(COLOR_BLUE "Monitor is not running.\n" COLOR_RESET);
            }
        }

        else if(strcmp(input, "calculate_score") == 0) {
            DIR *dir = opendir("hunts");
            if(!dir) {
                perror(COLOR_RED "Failed to open hunts directory" COLOR_RESET);
                continue;
            }

            struct dirent *entry;
            while((entry = readdir(dir)) != NULL) {
                if(entry->d_type == DT_DIR &&
                   strcmp(entry->d_name, ".") != 0 &&
                   strcmp(entry->d_name, "..") != 0) {
                    
                    char hunt_path[256];
                    snprintf(hunt_path, sizeof(hunt_path), "hunts/%s", entry->d_name);

                    int score_pipe[2];
                    if (pipe(score_pipe) == -1) {
                        perror(COLOR_RED "Pipe creation failed" COLOR_RESET);
                        continue;
                    }

                    pid_t pid = fork();
                    if(pid < 0) {
                        perror(COLOR_RED "Fork failed" COLOR_RESET);
                        continue;
                    }else if(pid == 0) {
                        // Child process
                        close(score_pipe[0]); // Close read end of the pipe in the child process
                        dup2(score_pipe[1], STDOUT_FILENO); // Redirect stdout to the pipe
                        close(score_pipe[1]); // Close write end of the pipe in the child process

                        execlp("./score_calculator", "score_calculator", hunt_path, NULL);
                        perror(COLOR_RED "execlp failed" COLOR_RESET);
                        exit(1);
                    }else {
                        // Parent process
                        close(score_pipe[1]); // Close write end of the pipe in the parent process

                        char buffer[256];
                        ssize_t nbytes = read(score_pipe[0], buffer, sizeof(buffer) - 1);
                        if (nbytes > 0) {
                            buffer[nbytes] = '\0'; // Null-terminate the string
                            printf(COLOR_YELLOW "Scores for hunt: %s\n" COLOR_RESET, entry->d_name);
                            printf("%s", buffer);
                        } else {
                            printf(COLOR_RED "No response from score calculator.\n" COLOR_RESET);
                        }
                        close(score_pipe[0]); // Close read end of the pipe in the parent process
                        wair(NULL); // Wait for child process to finish
                    }

                }
            }
        }
        
        /*
        
        else if(strcmp(input, "list_treasures") == 0) {
            if(monitor_pid > 0){
                kill(monitor_pid, SIGUSR2);
            }else{
                printf("Monitor is not running.\n");
            }
        }*/

        else if (strcmp(input, "stop_monitor") == 0) {
            if(monitor_pid <= 0){
                printf(COLOR_BLUE "Monitor is not running.\n" COLOR_RESET);
            }else{
                printf("\nStopping monitor with PID %d...\n", monitor_pid);
                kill(monitor_pid, SIGTERM);
                waitpid(monitor_pid, NULL, 0);
                monitor_pid = -1;
                printf("Monitor stopped.\n");
            }
        }

        else if (strcmp(input, "exit") == 0) {
            if (monitor_running) {
                printf(COLOR_RED" Cannot exit: monitor is still running.\n" COLOR_RESET);
            } else {
                printf("Exiting... Goodbye!\n");
                break;
            }
        }

        else if (strcmp(input, "help") == 0) {
            printf("Available commands:\n");
            printf(COLOR_GREEN);
            printf("    start_monitor - Start the monitor process\n" );
            printf("    stop_monitor - Stop the monitor process\n" );
            printf("    list_hunts - List all hunts\n" );
            printf("    list_treasures - List all treasures in a hunt\n" );
            printf("    view_treasure - View a specific treasure\n" );
            printf("    exit - Exit the program\n" );
            printf(COLOR_RESET);
        }

        else {
            printf(COLOR_RED" Unknown command: '%s'\n" COLOR_RESET, input);
        }
    }

    return 0;
}
