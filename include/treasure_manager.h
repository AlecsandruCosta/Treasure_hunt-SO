
typedef struct {
    int id;
    char username[50];
    float latitude, longitude;
    char clue[250];
    int value;
}Treasure;

void ensure_directory_exists(const char *hunt_id);
int open_treasure_file(const char *hunt_id, int flags);
int write_treasure(const char *hunt_id, Treasure *treasure);
void read_treasure(const char *hunt_id);
int view_treasure(const char *hunt_id, int id);
int remove_treasure(const char *hunt_id, int treasure_id);