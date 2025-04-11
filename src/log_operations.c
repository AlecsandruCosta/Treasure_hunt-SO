#include "../include/log_operations.h"
#include <stdio.h>
#include <time.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdarg.h> // for va_list, va_start, va_end
#include <string.h>

#define LOG_FILE "logged_hunt"

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
    //vfprintf(fd, format, args); // write the formatted string to the log file
    vdprintf(fd, format, args); // write the formatted string to the log file
    va_end(args); // clean up the variable argument list

    // Write a newline character
    write(fd, "\n", 1);

    close(fd);
}