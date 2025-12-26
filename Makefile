CC = gcc
CFLAGS = -Wall -Wextra -Iinclude
SRC = src/main.c
OBJ = $(SRC:.c=.o)
TARGET = build/app

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(OBJ) -o $(TARGET)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(OBJ) $(TARGET)

.PHONY: all clean
