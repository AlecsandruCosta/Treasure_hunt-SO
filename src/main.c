#include <stdio.h>
#include <string.h>
#include "../include/treasure_manager.h"
#include "../include/log_operations.h"


int main(){

    Treasure treasure = {1, "user123", 37.7749, -122.4194, "Find the hidden treasure!", 1000};

    if(write_treasure("hunt1", &treasure) == 0) {
        printf("Treasure added successfully.\n");
    } else {
        printf("Failed to add treasure.\n");
    }
   

    
    return 0;
}