CC = gcc
CFLAGS = -Wall
SRC = src/main.c src/treasure_manager.c src/log_operations.c
OUT = treasure_manager

all:
	$(CC) $(CFLAGS) $(SRC) -o $(OUT)

clean:
	rm -f $(OUT)
