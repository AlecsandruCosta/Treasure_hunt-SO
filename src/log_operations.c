#include "../include/log_operations.h"
#include <stdio.h>
#include <time.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdarg.h> // for va_list, va_start, va_end
#include <string.h>
#include <limits.h> // for PATH_MAX

#define LOG_FILE "logged_hunt"

void create_symlink(const char *hunt_id) {
    // Create a symbolic link to the hunt directory
    char target[512], linkname[512];
    const char *project_root = "/home/debian/treasure_hunt_project"; // Explicitly set the root directory

    // Path to the actual log file (inside the hunt directory)
    int target_len = snprintf(target, sizeof(target), "%s/hunts/%s/logged_hunt", project_root, hunt_id);
    if (target_len < 0 || target_len >= sizeof(target)) {
        fprintf(stderr, "Error: target path is too long\n");
        return;
    }

    // Path to the symbolic link (in the root folder)
    int linkname_len = snprintf(linkname, sizeof(linkname), "%s/logged_hunt-/%s", project_root, hunt_id);
    if (linkname_len < 0 || linkname_len >= sizeof(linkname)) {
        fprintf(stderr, "Error: linkname path is too long\n");
        return;
    }

    unlink(linkname); // Remove existing symlink if it exists

    // Create the symbolic link
    if (symlink(target, linkname) == -1) {
        perror("Error creating symbolic link");
    } else {
        printf("Symbolic link created: %s -> %s\n", linkname, target);
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

    close(fd);
}