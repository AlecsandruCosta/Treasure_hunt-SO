#!/bin/bash

# filepath: /home/debian/treasure_hunt_project/build.sh

# Set the output binary name
OUTPUT="treasure_hub"

# Set the source files
SRC_FILES="src/treasure_hub.c src/treasure_manager.c src/log_operations.c"

# Set the include directory
INCLUDE_DIR="include/log_operations.h include/treasure_manager.h"

# Compile the project
echo "Compiling the project..."
gcc -Wall -o $OUTPUT $SRC_FILES $INCLUDE_DIR

# Check if compilation was successful
if [ $? -eq 0 ]; then
    echo "Compilation successful! Executable created: $OUTPUT"
else
    echo "Compilation failed!"
fi