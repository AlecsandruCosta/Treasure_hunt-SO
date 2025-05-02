#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

#define MAX_INPUT 256

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
            if (monitor_running) {
                printf("Monitor is already running.\n");
            } else {
                printf("Starting monitor... [dummy response]\n");
                monitor_running = true;
            }
        }
        else if (strcmp(input, "list_hunts") == 0) {
            printf("List hunts... [dummy response]\n");
        }
        else if (strcmp(input, "list_treasures") == 0) {
            printf("List treasures... [dummy response]\n");
        }
        else if (strcmp(input, "view_treasure") == 0) {
            printf("View treasure... [dummy response]\n");
        }
        else if (strcmp(input, "stop_monitor") == 0) {
            if (!monitor_running) {
                printf("Monitor is not running.\n");
            } else {
                printf("Stopping monitor... [dummy response]\n");
                monitor_running = false;
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
        else {
            printf(" Unknown command: '%s'\n", input);
        }
    }

    return 0;
}
