#include <stdio.h>
#include <string.h>
#include "../include/treasure_manager.h"

int main(){

    Treasure t1;
    t1.id = 1;
    strcpy(t1.username, "treasure_hunter");
    t1.latitude = 34.0522;
    t1.longitude = -118.2437;
    t1.value = 1000;

    printf("Treasure ID: %d\n", t1.id);
    printf("Treasure Username: %s\n", t1.username);
    printf("Treasure Latitude: %.4f\n", t1.latitude);
    printf("Treasure Longitude: %.4f\n", t1.longitude);
    printf("Treasure Value: %d\n", t1.value);


    return 0;
}