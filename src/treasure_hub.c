#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>

#define MAX_INPUT 256

int monitor_pid = -1;

int main() {
    char input[MAX_INPUT];
    bool monitor_running = false;

    printf(" Welcome to Treasure Hub \n");

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
                    
                    // Child process: monitor
                    printf("[Monitor process running with PID %d]\n", getpid());
                    while(1){
                        pause();
                    }
                    exit(0);              
                }
                else {
                    // Parent (hub)
                    monitor_pid = pid;
                    printf("\nMonitor started with PID %d.\n", monitor_pid);
                }
            }

        }else if (strcmp(input, "stop_monitor") == 0) {
            if(monitor_pid <= 0){
                printf("Monitor is not running.\n");
            }else{
                printf("\nStopping monitor with PID %d...\n", monitor_pid);
                kill(monitor_pid, SIGTERM);
                waitpid(monitor_pid, NULL, 0);
                monitor_pid = -1;
                printf("Monitor stopped.\n");
            }

        }else if (strcmp(input, "exit") == 0) {
            if (monitor_running) {
                printf(" Cannot exit: monitor is still running.\n");
            } else {
                printf("Exiting... Goodbye!\n");
                break;
            }
        }
        else {
            printf(" Unknown command: '%s'\n", input);
        }
    }

    return 0;
}
