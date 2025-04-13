## COSTA ALECSANDRU
This is the first phase of the Treasure Hunter project

## HOW TO RUN
-run this command in the rerminal:
./treasure_hunt
You will be shown the usage of the implemented functions.

## COMMAND SUMMARY
    ./treasure_manager add <hunt_id>
-creates a hunt named <hunt_id> and adds a treasure to it; after completion, creates creates a "hunts" folder (if it didn't exist already") and creates a symlink in the root folder that points to the log of the hunt

    ./treasure_manager list <hunt_id>
-lists all the treasures in <hunt_id>

    ./treasure_manager view <hunt_id> <treasure_id>
-views all the details of the treasure with the name <treasure_id> in the hunt named <treasure_id>

    ./treasure_manager remove_treasure <hunt_id> <treasure_id>
-removes the specified treasure from the specified hunt

    ./treasure_manager remove_hunt <hunt_id>
-removes the specified hunt from the "hunts" folder

## PROJECT STRUCTURE
treasure_hunt_project/
├── src/
│   ├── main.c
│   ├── treasure_manager.c
│   ├── log_operations.c
├── include/
│   ├── log_operations.h
│   ├── treasure_manager.h
├── hunts/
│   └── (dynamically created folders like testHunt)
├── logged_hunt-huntX  
├── treasure_manager   
├── README.md          