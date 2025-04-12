#include "../include/treasure_manager.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void print_usage(){
    printf("Usage:\n");
    printf("./treasure_hunt add <hunt_id>\n");
    printf("./treasure_hunt list <hunt_id>\n");
    printf("./treasure_hunt view <hunt_id> <treasure_id>\n");
    printf("./treasure_hunt remove_treasure <hunt_id> <treasure_id>\n");
    printf("./treasure_hunt remove_hunt <hunt_id>\n");
}
/*
int main() {
    char hunt_id[50];
    Treasure t;

    printf("Enter hunt name (ID): ");
    scanf("%49s", hunt_id);

    printf("\n--- Adding a Treasure ---\n");
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

    // List all treasures in this hunt
    printf("\n--- Treasures in %s ---\n", hunt_id);
    read_treasure(hunt_id);
    
    printf("\nViewing a Treasure in Hunt %s\n", hunt_id);
    printf("Enter Treasure ID to view: ");
    int treasure_id;
    scanf("%d", &treasure_id);
    view_treasure(hunt_id, treasure_id);

    printf("\n--- Removing a Treasure ---\n");
    printf("Enter Treasure ID to remove: ");
    scanf("%d", &treasure_id);
    if (remove_treasure(hunt_id, treasure_id) == 0) {
        printf("Treasure removed successfully!\n");
    } else {
        printf("Failed to remove treasure.\n");
    }
    //check if the treasure was removed
    printf("\n--- Treasures in %s after removal ---\n", hunt_id);
    read_treasure(hunt_id);

    return 0;
}
    */

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
