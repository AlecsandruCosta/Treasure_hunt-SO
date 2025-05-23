#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <errno.h>
#include <sys/stat.h>

#define MAX_USERS 100

typedef struct {
    int id;
    char username[50];
    float latitude, longitude;
    char clue[250];
    int value;
}Treasure;

typedef struct
{
    char username[50];
    int total_value;
} UserScore;

UserScore scores[MAX_USERS];
int user_count = 0;

void add_score(const char *username, int value)
{
    for (int i = 0; i < user_count; i++)
    {
        if (strcmp(scores[i].username, username) == 0)
        {
            scores[i].total_value += value;
            return;
        }
    }
    // new user
    strncpy(scores[user_count].username, username, sizeof(scores[user_count].username));
    scores[user_count].total_value = value;
    user_count++;
}

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        fprintf(stderr, "Usage: %s <hunt_folder>\n", argv[0]);
        return 1;
    }

    const char *hunt_path = argv[1];
    DIR *dir = opendir(hunt_path);
    if (!dir)
    {
        perror("Failed to open hunt folder");
        return 1;
    }

    struct dirent *entry;
    char filepath[256];

    while ((entry = readdir(dir)) != NULL)
    {
        snprintf(filepath, sizeof(filepath), "%s/%s", hunt_path, entry->d_name);
        struct stat st;

        if (stat(filepath, &st) == 0 && S_ISREG(st.st_mode))
        {
            // It's a regular file
            FILE *file = fopen(filepath, "rb");
            if (!file)
            {
                perror("Failed to open file");
                continue;
            }

            Treasure treasure;
            while (fread(&treasure, sizeof(Treasure), 1, file) == 1)
            {
                add_score(treasure.username, treasure.value);
                printf("Read from %s: user=%s, value=%d\n", filepath, treasure.username, treasure.value);

            }
            fclose(file);
        }
    }
    closedir(dir);

    // output results
    for (int i = 0; i < user_count; i++)
    {
        printf("%s: %d\n", scores[i].username, scores[i].total_value);
    }

    return 0;
}