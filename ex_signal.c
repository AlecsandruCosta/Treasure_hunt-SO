#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <signal.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <string.h>

pid_t child_pid = 0;
int n = 0;

//a simple example of the fork() function
int main(int argc, char *args[]) {
    int status;
    //char *args[3];
    char action = args[1];

    child_pid = fork();
    if (child_pid == 0) {
        // Child process
        execvp(args[0], action);
        perror("execvp failed");
        exit(1);
    } else if (child_pid > 0) {
        // Parent process
        waitpid(child_pid, &status, 0);
        printf("Child process finished with status %d\n", WEXITSTATUS(status));
    } else {
        // Fork failed
        perror("fork failed");
        exit(1);
    }

    return 0;
}