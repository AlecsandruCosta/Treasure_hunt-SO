#include "../include/treasure_manager.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void print_usage(){
    printf("Usage:\n");
    printf("./treasure_manager add <hunt_id>\n");
    printf("./treasure_manager list <hunt_id>\n");
    printf("./treasure_manager view <hunt_id> <treasure_id>\n");
    printf("./treasure_manager remove_treasure <hunt_id> <treasure_id>\n");
    printf("./treasure_manager remove_hunt <hunt_id>\n");
}

int main(int argc, char *argv[]) {
    if (argc < 3) {
        print_usage();
        return 1;
    }

    const char *action = argv[1];
    const char *hunt_id = argv[2];

    if (strcmp(action, "add") == 0) {
        Treasure t;
        printf("Enter Treasure ID: ");
        scanf("%d", &t.id);
        
        printf("Enter Username: ");
        scanf("%49s", t.username);

        printf("Enter Latitude: ");
        scanf("%f", &t.latitude);

        printf("Enter Longitude: ");
        scanf("%f", &t.longitude);

        printf("Enter Clue: ");
        scanf(" %[^\n]", t.clue); // Read full line after whitespace

        printf("Enter Treasure Value: ");
        scanf("%d", &t.value);


        // Add the treasure to the specified hunt
        if (write_treasure(hunt_id, &t) == 0) {
            printf("\nTreasure added successfully!\n");
        } else {
            printf("\nFailed to add treasure.\n"); 
        }
    } else if (strcmp(action, "list") == 0) {
        read_treasure(hunt_id);
    } else if (strcmp(action, "view") == 0 && argc == 4) {
        int treasure_id = atoi(argv[3]);
        view_treasure(hunt_id, treasure_id);
    } else if (strcmp(action, "remove_treasure") == 0 && argc == 4) {
        int treasure_id = atoi(argv[3]);
        if (remove_treasure(hunt_id, treasure_id) == 0) {
            printf("Treasure removed successfully!\n");
        } else {
            printf("Failed to remove treasure.\n");
        }
    } else if (strcmp(action, "remove_hunt") == 0) {
        if (remove_hunt(hunt_id) == 0) {
            printf("Hunt removed successfully!\n");
        } else {
            printf("Failed to remove hunt.\n");
        }
    } else {
        print_usage();
    }

    return 0;
}
