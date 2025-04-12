#include "../include/log_operations.h"
#include <stdio.h>
#include <time.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdarg.h> // for va_list, va_start, va_end
#include <string.h>
#define PATH_MAX 4096

#define LOG_FILE "logged_hunt"


// Ensure symlink is always created in project root, not in /src
void create_symlink(const char *hunt_id) {
    char cwd[PATH_MAX];
    char root_path[PATH_MAX] = "~/treasure_hunt_project/"; // Default root path
    char target[PATH_MAX];
    char link_path[PATH_MAX];

    // Get current working directory
    if (getcwd(cwd, sizeof(cwd)) == NULL) {
        perror("getcwd failed");
        return;
    }

    // Go one level up if inside /src
    strcpy(root_path, cwd);
    if (strstr(cwd, "/src") != NULL) {
        char *src_position = strstr(root_path, "/src");
        if (src_position) *src_position = '\0'; // trim everything after "/src"
    }

    // Build the absolute target path and the symlink path
    #pragma GCC diagnostic push //
    #pragma GCC diagnostic ignored "-Wformat-truncation" // Ignore truncation warning, as root_path sould not realistically exceed PATH_MAX
    snprintf(target, sizeof(target), "%s/src/hunts/%s/logged_hunt", root_path, hunt_id);
    snprintf(link_path, sizeof(link_path), "%s/logged_hunt-%s", root_path, hunt_id);
    #pragma GCC diagnostic pop 

    printf("\n🔗 Attempting to create symlink:\n");
    printf("    Link:   %s\n", link_path);
    printf("    Target: %s\n", target);

    // Remove old symlink if it exists
    unlink(link_path);

    // Create the symlink
    if (symlink(target, link_path) == -1) {
        perror("Error creating symbolic link");
    }
}

// logs an action in the hunt's log file
void log_action(const char *hunt_id, const char *format, ...) {
    // Open the log file for appending
    char filepath[256];
    snprintf(filepath, sizeof(filepath), "hunts/%s/%s", hunt_id, LOG_FILE);

    
    
    int fd = open(filepath, O_CREAT | O_WRONLY | O_APPEND, 0644);
    if (fd == -1) {
        perror("Error opening log file");
        return;
    }

    // Get the current timestamp
    time_t now = time(NULL);
    struct tm *tm_info = localtime(&now);
    char time_str[26]; //this variable will hold the time string
    strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M:%S", tm_info); // format the time string

    // Write the timestamp to the log file
    dprintf(fd, "[%s] ", time_str);

    // Write log message using variable arguments
    va_list args;
    va_start(args, format);// initialize the variable argument list
    
    vdprintf(fd, format, args); // write the formatted string to the log file
    va_end(args); // clean up the variable argument list

    // Write a newline character
    //write(fd, "\n", 1);
    dprintf(fd, "\n");

    // Force flush to disk; this is to make sure that the log is written immediately so that the symlink can be created
    if (fsync(fd) == -1) {
        perror("fsync failed");
    }

    close(fd);

    create_symlink(hunt_id);
}