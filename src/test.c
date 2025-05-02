#include <stdio.h>
#include <dirent.h>
#include <signal.h>

int main() {
    struct sigaction sa;
    DIR *d = opendir(".");

    if (d) {
        struct dirent *entry;
        while ((entry = readdir(d))) {
            if (entry->d_type == DT_DIR) {
                printf("Directory: %s\n", entry->d_name);
            }
        }
        closedir(d);
    }

    return 0;
}