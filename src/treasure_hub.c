#define _POSIX_C_SOURCE 200809L
#define _DEFAULT_SOURCE
#include "../include/treasure_manager.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <dirent.h>
#include <sys/select.h>
#include <fcntl.h>
#include <signal.h>
#include <stdatomic.h>

#define COLOR_RED "\x1b[31m"
#define COLOR_GREEN "\x1b[32m"
#define COLOR_YELLOW "\x1b[33m"
#define COLOR_BLUE "\x1b[34m"
#define COLOR_MAGENTA "\x1b[35m"
#define COLOR_CYAN "\x1b[36m"
#define COLOR_UNDERLINE "\x1b[4m"
#define COLOR_RESET "\x1b[0m"

#define MAX_INPUT_SIZE 256
#define PARAM_FILE "monitor_command.txt"

int monitor_pid = -1;
int pipefd[2] = {-1, -1}; // Initialize to invalid

volatile sig_atomic_t monitor_ready = 0;

void monitor_ready_handler(int sig)
{
    monitor_ready = 1;
}

void print_hub_banner()
{
    printf("\n");
    printf(COLOR_YELLOW);
    printf("████████ ██████  ███████  █████  ███████ ██    ██ ██████  ███████     ██   ██ ██    ██ ██████ \n");
    printf("   ██    ██   ██ ██      ██   ██ ██      ██    ██ ██   ██ ██          ██   ██ ██    ██ ██   ██ \n");
    printf("   ██    ██████  █████   ███████ ███████ ██    ██ ██████  █████       ███████ ██    ██ ██████  \n");
    printf("   ██    ██   ██ ██      ██   ██      ██ ██    ██ ██   ██ ██          ██   ██ ██    ██ ██   ██ \n");
    printf("   ██    ██   ██ ███████ ██   ██ ███████  ██████  ██   ██ ███████     ██   ██  ██████  ██████  \n");
    printf("\n");
    printf(COLOR_RESET);
}

// Signal handlers
void handle_list_hunts(int sig)
{
    printf("Listing hunts...\n");

    DIR *dir = opendir("hunts");

    if (!dir)
    {
        perror(COLOR_RED "Failed to open hunts directory" COLOR_RESET);
        return;
    }

    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL)
    {
        if (entry->d_type == DT_DIR &&
            strcmp(entry->d_name, ".") != 0 &&
            strcmp(entry->d_name, "..") != 0)
        {
            printf(" Found hunt: " COLOR_YELLOW "%s\n" COLOR_RESET, entry->d_name);
        }
    }
    closedir(dir);

    // Flush the output to ensure the prompt appears correctly
    fflush(stdout);
    kill(getppid(), SIGUSR1);
}

void handle_list_treasure(int sig)
{
    printf("Listing treasures...\n");

    FILE *file = fopen(PARAM_FILE, "r");
    if (!file)
    {
        perror(COLOR_RED "Failed to open parameter file" COLOR_RESET);
        return;
    }

    char hunt_id[256];
    if (fgets(hunt_id, sizeof(hunt_id), file))
    {
        hunt_id[strcspn(hunt_id, "\n")] = '\0'; // trim newline
        printf("Hunt ID: %s\n", hunt_id);

        // Call the function to read treasure
        read_treasure(hunt_id);
    }
    else
    {
        printf(COLOR_RED "No hunt_id found in file.\n" COLOR_RESET);
    }
    fclose(file);

    // Ensure all output is sent through the pipe before the parent reads
    fflush(stdout);
    kill(getppid(), SIGUSR1);
}

void handle_view_treasure(int sig)
{
    printf("Viewing treasures...\n");

    FILE *file = fopen(PARAM_FILE, "r");
    if (!file)
    {
        perror(COLOR_RED "Failed to open parameter file" COLOR_RESET);
        return;
    }

    char hunt_id[256], treasure_id[256];
    if (fgets(hunt_id, sizeof(hunt_id), file) &&
        fgets(treasure_id, sizeof(treasure_id), file))
    {
        hunt_id[strcspn(hunt_id, "\n")] = '\0';         // trim newline
        treasure_id[strcspn(treasure_id, "\n")] = '\0'; // trim newline

        printf("Hunt ID: %s\n", hunt_id);
        printf("Treasure ID: %s\n", treasure_id);

        // Call the function to view treasure
        int id = atoi(treasure_id); // Example treasure ID
        view_treasure(hunt_id, id);
    }
    else
    {
        printf(COLOR_RED "Invalid format in monitor_command.txt.\n" COLOR_RESET);
    }
    fclose(file);

    // Ensure all output is sent through the pipe before the parent reads
    fflush(stdout);
    kill(getppid(), SIGUSR1);
}

// Function to handle SIGUSR1 signal
void handle_sigterm(int sig)
{
    printf("Terminating...\n");
    sleep(5);
    printf("Terminated.\n");
    exit(0);
}

// Monitor process main loop
void start_monitor_loop()
{
    struct sigaction sa1, sa2, sa_view, sa_term;

    sa1.sa_handler = handle_list_hunts;
    sigemptyset(&sa1.sa_mask);
    sa1.sa_flags = 0;
    sigaction(SIGUSR1, &sa1, NULL);

    sa2.sa_handler = handle_list_treasure;
    sigemptyset(&sa2.sa_mask);
    sa2.sa_flags = 0;
    sigaction(SIGUSR2, &sa2, NULL);

    sa_view.sa_handler = handle_view_treasure;
    sigemptyset(&sa_view.sa_mask);
    sa_view.sa_flags = 0;
    sigaction(SIGALRM, &sa_view, NULL);

    sa_term.sa_handler = handle_sigterm;
    sigemptyset(&sa_term.sa_mask);
    sa_term.sa_flags = 0;
    sigaction(SIGTERM, &sa_term, NULL);

    // printf("[Monitor] Ready and waiting for signals (PID: %d)\n", getpid());

    while (1)
    {
        pause(); // Wait for signals
    }
}

void read_monitor_output()
{
    if (pipefd[0] == -1)
    {
        printf(COLOR_RED "Monitor pipe is not open.\n" COLOR_RESET);
        return;
    }
    char buffer[256];
    fd_set set;
    struct timeval timeout;

    FD_ZERO(&set);
    FD_SET(pipefd[0], &set);

    timeout.tv_sec = 5;
    timeout.tv_usec = 0;

    int rv = select(pipefd[0] + 1, &set, NULL, NULL, &timeout);
    if (rv > 0)
    {
        ssize_t nbytes;
        int got_output = 0;
        while ((nbytes = read(pipefd[0], buffer, sizeof(buffer) - 1)) > 0)
        {
            got_output = 1;
            buffer[nbytes] = '\0';
            printf(COLOR_GREEN "%s" COLOR_RESET, buffer);
        }
        if (!got_output)
            printf(COLOR_YELLOW "No output from monitor.\n" COLOR_RESET);
    }
    else
    {
        printf(COLOR_RED "No response from monitor (timeout).\n" COLOR_RESET);
    }
}

int main()
{
    char input[MAX_INPUT_SIZE];

    // printf(" Welcome to Treasure Hub \n");
    print_hub_banner();
    printf(COLOR_BLUE" Type 'help' for a list of commands.\n" COLOR_RESET);

    struct sigaction sa_ready;
    sa_ready.sa_handler = monitor_ready_handler;
    sigemptyset(&sa_ready.sa_mask);
    sa_ready.sa_flags = 0;
    sigaction(SIGUSR1, &sa_ready, NULL);

    while (1)
    {
        printf(COLOR_MAGENTA "treasure_hub> " COLOR_RESET);
        fflush(stdout);

        // Read user input
        if (fgets(input, sizeof(input), stdin) == NULL)
        {
            printf("Error reading input.\n");
            continue;
        }

        // Remove newline character
        input[strcspn(input, "\n")] = '\0';

        // Skip empty input
        if (input[0] == '\0') {
            continue;
        }

        // Handle commands
        if (strcmp(input, "start_monitor") == 0)
        {
            if (monitor_pid > 0)
            {
                printf(COLOR_BLUE "Monitor is already running (PID %d).\n" COLOR_RESET, monitor_pid);
            }
            else
            {
                if (pipe(pipefd) == -1)
                {
                    perror(COLOR_RED "Pipe creation failed" COLOR_RESET);
                    continue;
                }
                pid_t pid = fork();
                if (pid < 0)
                {
                    perror("Fork failed");
                    close(pipefd[0]);
                    close(pipefd[1]);
                    continue;
                }
                else if (pid == 0)
                {
                    close(pipefd[0]);
                    dup2(pipefd[1], STDOUT_FILENO);
                    close(pipefd[1]);
                    start_monitor_loop();
                    exit(0);
                }
                else
                {
                    close(pipefd[1]);
                    int flags = fcntl(pipefd[0], F_GETFL, 0);
                    fcntl(pipefd[0], F_SETFL, flags | O_NONBLOCK);
                    monitor_pid = pid;
                    printf(COLOR_BLUE "Monitor started with PID %d.\nReady and waiting for signals\n" COLOR_RESET, monitor_pid );
                }
            }
        }

        else if (strcmp(input, "list_hunts") == 0)
        {
            if (monitor_pid > 0)
            {
                monitor_ready = 0;
                kill(monitor_pid, SIGUSR1);
                while (!monitor_ready)
                {
                    usleep(1000); // Sleep 1ms to avoid busy-waiting
                }
                read_monitor_output();
            }
            else
            {
                printf(COLOR_BLUE "Monitor is not running.\n" COLOR_RESET);
            }
        }

        else if (strcmp(input, "list_treasures") == 0)
        {
            if (monitor_pid > 0)
            {
                char hunt_id[256];
                printf(COLOR_BLUE "Enter hunt ID: " COLOR_RESET);
                fgets(hunt_id, sizeof(hunt_id), stdin);
                hunt_id[strcspn(hunt_id, "\n")] = '\0'; // trim newline

                FILE *file = fopen(PARAM_FILE, "w");
                if (file)
                {
                    fprintf(file, "%s\n", hunt_id);
                    fclose(file);
                }

                monitor_ready = 0;
                kill(monitor_pid, SIGUSR2);
                while (!monitor_ready)
                {
                    usleep(1000); // Sleep 1ms to avoid busy-waiting
                }
                read_monitor_output();
            }
            else
            {
                printf(COLOR_BLUE "Monitor is not running.\n" COLOR_RESET);
            }
        }

        else if (strcmp(input, "view_treasure") == 0)
        {
            if (monitor_pid > 0)
            {
                char hunt_id[256], treasure_id[256];

                printf(COLOR_BLUE "Enter hunt ID: " COLOR_RESET);
                fgets(hunt_id, sizeof(hunt_id), stdin);
                hunt_id[strcspn(hunt_id, "\n")] = '\0'; // trim newline

                printf(COLOR_BLUE "Enter treasure ID: " COLOR_RESET);
                fgets(treasure_id, sizeof(treasure_id), stdin);
                treasure_id[strcspn(treasure_id, "\n")] = '\0'; // trim newline

                FILE *file = fopen(PARAM_FILE, "w");
                if (file)
                {
                    fprintf(file, "%s\n%s\n", hunt_id, treasure_id);
                    fclose(file);
                }
                else
                {
                    printf(COLOR_RED "Failed to open parameter file.\n" COLOR_RESET);
                    continue;
                }

                monitor_ready = 0;
                kill(monitor_pid, SIGALRM);
                while (!monitor_ready)
                {
                    usleep(1000); // Sleep 1ms to avoid busy-waiting
                }
                read_monitor_output();
            }
            else
            {
                printf(COLOR_BLUE "Monitor is not running.\n" COLOR_RESET);
            }
        }

        else if (strcmp(input, "calculate_score") == 0)
        {
            if (monitor_pid > 0)
            {
                DIR *dir = opendir("hunts");
                if (!dir)
                {
                    perror(COLOR_RED "Failed to open hunts directory" COLOR_RESET);
                    continue;
                }

                struct dirent *entry;
                while ((entry = readdir(dir)) != NULL)
                {
                    if (entry->d_type == DT_DIR &&
                        strcmp(entry->d_name, ".") != 0 && // skip current directory
                        strcmp(entry->d_name, "..") != 0)
                    { // skip parent directory

                        char hunt_path[256];
                        snprintf(hunt_path, sizeof(hunt_path), "hunts/%.249s", entry->d_name);

                        // check if the directory contains any files (treasures)
                        DIR *hunt_dir = opendir(hunt_path);
                        int has_treasure = 0;
                        if (hunt_dir)
                        {
                            struct dirent *e;
                            while ((e = readdir(hunt_dir)) != NULL)
                            {
                                if (e->d_type == DT_REG)
                                {
                                    has_treasure = 1;
                                    break;
                                }
                            }
                            closedir(hunt_dir);
                        }
                        if (!has_treasure)
                        {
                            // Skip empty hunt directories
                            continue;
                        }

                        int score_pipe[2];
                        if (pipe(score_pipe) == -1)
                        {
                            perror(COLOR_RED "Pipe creation failed" COLOR_RESET);
                            continue;
                        }

                        pid_t pid = fork();
                        if (pid < 0)
                        {
                            perror(COLOR_RED "Fork failed" COLOR_RESET);
                            continue;
                        }
                        else if (pid == 0)
                        {
                            // Child process
                            close(score_pipe[0]);
                            dup2(score_pipe[1], STDOUT_FILENO);
                            close(score_pipe[1]);
                            execlp("./score_calculator", "score_calculator", hunt_path, NULL);
                            perror(COLOR_RED "execlp failed" COLOR_RESET);
                            exit(1);
                        }
                        else
                        {
                            // Parent process
                            close(score_pipe[1]);
                            char buffer[256];
                            ssize_t nbytes = read(score_pipe[0], buffer, sizeof(buffer) - 1);
                            if (nbytes > 0)
                            {
                                buffer[nbytes] = '\0';
                                printf(COLOR_YELLOW "\nScores for hunt: %s\n" COLOR_RESET, entry->d_name);
                                printf("%s", buffer);
                            }
                            close(score_pipe[0]);
                            wait(NULL);
                        }
                    }
                }
                closedir(dir);
            }
            else
            {
                printf(COLOR_BLUE "Monitor is not running.\n" COLOR_RESET);
            }
        }

        else if (strcmp(input, "stop_monitor") == 0)
        {
            if (monitor_pid <= 0)
            {
                printf(COLOR_BLUE "Monitor is not running.\n" COLOR_RESET);
            }
            else
            {
                printf(COLOR_BLUE "\nStopping monitor with PID %d...\n" COLOR_RESET, monitor_pid);
                kill(monitor_pid, SIGTERM);
                waitpid(monitor_pid, NULL, 0);
                monitor_pid = -1;
                if (pipefd[0] != -1)
                {
                    close(pipefd[0]);
                    pipefd[0] = -1;
                }
                printf(COLOR_BLUE "Monitor stopped.\n" COLOR_RESET);
            }
        }

        else if (strcmp(input, "exit") == 0)
        {
            if (monitor_pid > 0)
            {
                printf(COLOR_RED " Cannot exit: monitor is still running.\n" COLOR_RESET);
            }
            else
            {
                printf(COLOR_BLUE "Exiting... Goodbye!\n" COLOR_RESET);
                break;
            }
        }

        else if (strcmp(input, "help") == 0)
        {
            printf("Available commands:\n");
            printf(COLOR_GREEN);
            printf("    start_monitor - Start the monitor process\n");
            printf("    stop_monitor - Stop the monitor process\n");
            printf("    list_hunts - List all hunts\n");
            printf("    list_treasures - List all treasures in a hunt\n");
            printf("    view_treasure - View a specific treasure\n");
            printf("    calculate_score - Calculate the score for each user in each hunt\n");
            printf("    exit - Exit the program\n");
            printf(COLOR_RESET);
        }

        else
        {
            printf(COLOR_RED " Unknown command: '%s'\n" COLOR_RESET, input);
        }
    }

    return 0;
}
