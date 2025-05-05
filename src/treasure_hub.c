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

#define MAX_INPUT_SIZE 256
#define PARAM_FILE "monitor_command.txt"

int monitor_pid = -1;

//Signal handlers
void handle_list_hunts(int sig) {
    printf("[Monitor] Handling list_hunts...\n");

    DIR *dir = opendir("hunts");

    if(!dir) {
        perror("Failed to open hunts directory");
        return;
    }

    struct dirent *entry;
    while((entry = readdir(dir)) != NULL) {
        if(entry->d_type == DT_DIR &&
           strcmp(entry->d_name, ".") != 0 &&
           strcmp(entry->d_name, "..") != 0) {
            printf(" Found hunt: %s\n", entry->d_name);
        }
    }
    closedir(dir);
}

void handle_list_treasure(int sig) {
    printf("[Monitor] Handling list_treasure...\n");

    FILE *file = fopen(PARAM_FILE, "r");
    if(!file) {
        perror("Failed to open parameter file");
        return;
    }

    char hunt_id[256];
    if(fgets(hunt_id, sizeof(hunt_id), file)){
        hunt_id[strcspn(hunt_id, "\n")] = '\0'; // trim newline
        printf("Hunt ID: %s\n", hunt_id);

        // Call the function to read treasure
        read_treasure(hunt_id);

    }else{
        printf("No hunt_id found in file.\n");
    }
    fclose(file);
}

void handle_view_treasure(int sig) {
    printf("[Monitor] Handling view_treasure...\n");

    FILE *file = fopen(PARAM_FILE, "r");
    if(!file) {
        perror("Failed to open parameter file");
        return;
    }

    char hunt_id[256];
    if(fgets(hunt_id, sizeof(hunt_id), file)){
        hunt_id[strcspn(hunt_id, "\n")] = '\0'; // trim newline
        printf("Hunt ID: %s\n", hunt_id);

        // Call the function to view treasure
        int id = 1; // Example treasure ID
        view_treasure(hunt_id, id);

    }else{
        printf("No hunt_id found in file.\n");
    }
    fclose(file);
}

// Function to handle SIGUSR1 signal
void handle_sigterm(int sig) {
    printf("[Monitor] Terminating...\n");
    sleep(5);
    printf("[Monitor] Terminated.\n");
    exit(0);
}

//Monitor process main loop
void start_monitor_loop() {
    struct sigaction sa1, sa2, sa_term;

    sa1.sa_handler = handle_list_hunts;
    sigemptyset(&sa1.sa_mask);
    sa1.sa_flags = 0;
    sigaction(SIGUSR1, &sa1, NULL);

    sa2.sa_handler = handle_list_treasure;
    sigemptyset(&sa2.sa_mask);
    sa2.sa_flags = 0;
    sigaction(SIGUSR2, &sa2, NULL);
    
    /*
    sa3.sa_handler = handle_view_treasure;
    sigemptyset(&sa3.sa_mask);
    sa3.sa_flags = 0;
    sigaction(SIGUSR3, &sa3, NULL);
    */

    sa_term.sa_handler = handle_sigterm;
    sigemptyset(&sa_term.sa_mask);
    sa_term.sa_flags = 0;
    sigaction(SIGTERM, &sa_term, NULL);

    printf("[Monitor] Ready and waiting for signals (PID: %d)\n", getpid());

    while (1) {
        pause(); // Wait for signals
    }
} 



int main() {
    char input[MAX_INPUT_SIZE];
    bool monitor_running = false;

    printf(" Welcome to Treasure Hub \n");
    printf(" Type 'help' for a list of commands.\n");

    while (1) {
        printf("treasure_hub> ");
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
                printf("\nMonitor is already running (PID %d).\n", monitor_pid);
            } else {
                pid_t pid = fork();
                if(pid < 0) {
                    perror("Fork failed");
                }else if(pid == 0) {
                    
                    start_monitor_loop(); // Start the monitor loop
                    /*
                    // Child process: monitor
                    printf("[Monitor process running with PID %d]\n", getpid());
                    while(1){
                        pause();
                    }
                    exit(0); 
                    */             
                }
                else {
                    // Parent (hub)
                    monitor_pid = pid;
                    printf("\nMonitor started with PID %d.\n", monitor_pid);
                }
            }
        }

        else if (strcmp(input, "list_hunts") == 0){
            if(monitor_pid > 0){
                kill(monitor_pid, SIGUSR1);
            }else{
                printf("Monitor is not running.\n");
            }
        }
        /*
        else if (strcmp(input, "list_treasures") == 0) {
            if(monitor_pid > 0){
                char hunt_id[256];
                printf("Enter hunt ID: ");
                fgets(hunt_id, sizeof(hunt_id), stdin);
                hunt_id[strcspn(hunt_id, "\n")] = '\0'; // trim newline

                FILE *file = fopen(PARAM_FILE, "w");
                if(file) {
                    fprintf(file, "%s\n", hunt_id);
                    fclose(file);
                }

                kill(monitor_pid, SIGUSR2);
            }else{
                printf("Monitor is not running.\n");
            }
        }
        */
        
        else if(strcmp(input, "list_treasures") == 0) {
            if(monitor_pid > 0){
                kill(monitor_pid, SIGUSR2);
            }else{
                printf("Monitor is not running.\n");
            }
        }

        else if (strcmp(input, "stop_monitor") == 0) {
            if(monitor_pid <= 0){
                printf("Monitor is not running.\n");
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
                printf(" Cannot exit: monitor is still running.\n");
            } else {
                printf("Exiting... Goodbye!\n");
                break;
            }
        }

        else if (strcmp(input, "help") == 0) {
            printf("Available commands:\n");
            printf(" start_monitor - Start the monitor process\n");
            printf(" stop_monitor - Stop the monitor process\n");
            printf(" list_hunts - List all hunts\n");
            printf(" list_treasures - List all treasures in a hunt\n");
            printf(" exit - Exit the program\n");
        }

        else {
            printf(" Unknown command: '%s'\n", input);
        }
    }

    return 0;
}
