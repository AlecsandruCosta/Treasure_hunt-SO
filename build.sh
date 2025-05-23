#!/bin/bash

# Set the output binary names
HUB_OUTPUT="treasure_hub"
SCORE_OUTPUT="score_calculator"

# Set the source files
HUB_SRC="src/treasure_hub.c src/treasure_manager.c src/log_operations.c"
SCORE_SRC="src/score_calculator.c src/treasure_manager.c src/log_operations.c"

# Set the include directory
INCLUDE_DIR="-Iinclude"

# Compile treasure_hub
echo "Compiling treasure_hub..."
gcc -Wall -o $HUB_OUTPUT $HUB_SRC $INCLUDE_DIR
HUB_STATUS=$?

# Compile score_calculator
echo "Compiling score_calculator..."
gcc -Wall -o $SCORE_OUTPUT $SCORE_SRC $INCLUDE_DIR
SCORE_STATUS=$?

# Check if both compilations were successful
if [ $HUB_STATUS -eq 0 ] && [ $SCORE_STATUS -eq 0 ]; then
    echo "Compilation successful! Executables created: $HUB_OUTPUT, $SCORE_OUTPUT"
    echo "Running $HUB_OUTPUT..."
    ./$HUB_OUTPUT
else
    echo "Compilation failed!"
fi