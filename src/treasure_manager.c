#include "../include/treasure_manager.h"
#include "../include/log_operations.h"
#include <stdio.h>
#include <fcntl.h> //for functions open, close
#include <unistd.h> //for functions read, write
#include <sys/stat.h> //for function fstat
#include <string.h>
#define FILE_PERMISSIONS 0644

void ensure_directory_exists(const char *hunt_id) {

    // Check if the "hunts/" directory exists
    struct stat st;
    if(stat("hunts", &st) == -1) {
        if (mkdir("hunts", 0777) == -1) {
            perror("Error creating hunts directory");
            return;
        }
    }

    // Create the specific hunt directory 
    char dirpath[256];
    snprintf(dirpath, sizeof(dirpath), "hunts/%s", hunt_id);

    if (stat(dirpath, &st) == -1) {
        if (mkdir(dirpath, 0777) == -1) {
            perror("Error creating directory");
        } else {
            printf("Directory %s created successfully.\n", dirpath);
        }
    } 
}


int open_treasure_file(const char *hunt_id, int flags) {
    // Ensure the directory exists
    ensure_directory_exists(hunt_id);

    // Construct the file path
    char filename[256];
    snprintf(filename, sizeof(filename), "hunts/%s/_treasure.dat", hunt_id);

    // Check if the file exists
    int fd = open(filename, flags, FILE_PERMISSIONS);  
    if (fd == -1) {
        perror("Error opening treasure file");
        return -1;
    }
    return fd;
}

int write_treasure(const char *hunt_id, Treasure *treasure) {
    // Open the treasure file for writing
    int fd = open_treasure_file(hunt_id, O_CREAT | O_WRONLY | O_APPEND);
    if (fd == -1) {
        return -1;
    }

    // Write the treasure data to the file  
    ssize_t written = write(fd, treasure, sizeof(Treasure)); //write(descriptor, buffer, size)
    close(fd);

    if(written != sizeof(Treasure)) {
        perror("Error writing to treasure file");
        return -1;
    }

    // Log the action
    log_action(hunt_id, "Added treasure ID %d | User: %s | Value: %d | Clue: %s", 
                treasure->id, treasure->username, treasure->value, treasure->clue);
    return 0;
}


void read_treasure(const char *hunt_id){
    // Open the treasure file for reading
    int fd = open_treasure_file(hunt_id, O_RDONLY);
    if (fd == -1) {
        return;
    }

    // Read the treasure data from the file
    Treasure treasure;
    printf("Treasures in hunt %s:\n", hunt_id);

    while (read(fd, &treasure, sizeof(Treasure)) == sizeof(Treasure)) {
        printf("ID: %d | User: %s | Location: (%.2f, %.2f) | Clue: %s | Value: %d\n",
               treasure.id, treasure.username, treasure.latitude, treasure.longitude,
               treasure.clue, treasure.value);
        }
    close(fd);
}

int view_treasure(const char *hunt_id, int id) {
    // Open the treasure file for reading
    int fd = open_treasure_file(hunt_id, O_RDONLY);
    if (fd == -1) {
        return -1;
    }

    // Read the treasure data from the file
    Treasure treasure;
    while (read(fd, &treasure, sizeof(Treasure)) == sizeof(Treasure)) {
        if (treasure.id == id) {
            printf("ID: %d | User: %s | Location: (%.2f, %.2f) | Clue: %s | Value: %d\n",
                   treasure.id, treasure.username, treasure.latitude, treasure.longitude,
                   treasure.clue, treasure.value);
            close(fd);
            return 0;
        }
    }
    printf("Treasure with ID %d not found in hunt %s.\n", id, hunt_id);
    close(fd);
    return -1;
} 

int remove_treasure(const char *hunt_id, int treassure_id){
    char filepath[256], temp_filepath[256];
    snprintf(filepath, sizeof(filepath), "hunts/%s/_treasure.dat", hunt_id);
    snprintf(temp_filepath, sizeof(temp_filepath), "hunts/%s/_temp_treasure.dat", hunt_id);

    int fd = open(filepath, O_RDONLY);
    if (fd == -1) {
        perror("Error opening treasure file");
        return -1;
    }

    int temp_fd = open(temp_filepath, O_CREAT | O_WRONLY | O_TRUNC, FILE_PERMISSIONS);
    if (temp_fd == -1) {
        close(fd);
        perror("Error creating temporary file");
        return -1;
    }

    Treasure treasure;
    int found = 0;

    while (read(fd, &treasure, sizeof(Treasure)) == sizeof(Treasure)) {
        if (treasure.id == treassure_id) {
            found = 1;
            continue; // Skip writing this treasure to the temp file
        }
        write(temp_fd, &treasure, sizeof(Treasure));
    }

    close(fd);
    close(temp_fd);

    if (!found) {
        printf("Treasure with ID %d not found in hunt %s.\n", treassure_id, hunt_id);
        remove(temp_filepath); // Remove the temporary file
        return -1;
    }
    // Replace the original file with the temporary file
    if (remove(filepath) == -1) {
        perror("Error removing original treasure file");
        return -1;
    }
    rename(temp_filepath, filepath); // Rename temp file to original file
    
    log_action(hunt_id, "Removed treasure ID %d", treassure_id);
    return 0;
}

int remove_hunt(const char *hunt_id){
    char filepath[256];
    snprintf(filepath, sizeof(filepath), "hunts/%s", hunt_id);
    if (rmdir(filepath) == -1) {
        perror("Error removing hunt directory");
        return -1;
    }
    log_action(hunt_id, "Removed hunt %s", hunt_id);
    return 0;
}
