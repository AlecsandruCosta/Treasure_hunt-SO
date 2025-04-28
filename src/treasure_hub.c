#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define MAX_INPUT 256

int main(){
    char input[MAX_INPUT];
    bool monitor_running = false;

    printf("Welcome to the Treasure Hub!\n");

    while(1){
        printf("treasure_hub> ");
        fflush(stdout);

        //Read user input
        if(fgets(input, sizeof(input), stdin) == NULL){
            printf("Error reading input.\n");
            continue;
        }

        //Remove newline character
        input[strcspn(input, "\n")] = 0;

        //Handle commands
        if(strcmp(input, "start_monitor") == 0){
            if(monitor_running){
                printf("Monitor is already running.\n");
            } else {
                printf("Starting monitor...\n");
                // [start the monitor process]
                monitor_running = true;
            }

            int pid = fork();
            if(pid < 0){
                perror("Fork failed");
                exit(EXIT_FAILURE);
            } else if(pid == 0){
                while(1){
                    if(strcmp(input, "list_hunts") == 0){
                        printf("Listing all hunts...\n"); //ADD THE REST OF THE FUNCTIONS
                }
            }

        }else if(strcmp(input, "list_hunts") == 0){
            printf("Listing all hunts...\n");
            // [list all the hunts]
        }else if(strcmp(input, "list_treasures") == 0){
            printf("Listing all treasures...\n");
            // [list all the treasures]
        }else if(strcmp(input, "view_treasure") == 0){
            printf("Viewing treasure...\n");
            // [view a specific treasure]
        }else if(strcmp(input, "stop_monitor") == 0){
            if(monitor_running){
                printf("Stopping monitor...\n");
                // [stop the monitor process]
                monitor_running = false;
            } else {
                printf("Monitor is not running.\n");
            }
        }else if(strcmp(input, "exit") == 0){
            if(monitor_running){
                printf("Monitor is running. Please stop it before exiting.\n");
            } else {
                printf("Exiting Treasure Hub... Goodbye!\n");
                break;
            }
        }else{
            printf("Unknown command: %s\n", input);
            printf("Available commands: start_monitor, list_hunts, list_treasures, view_treasure, stop_monitor, exit\n");
        }
    }
    return 0;
}