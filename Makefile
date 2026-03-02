CC = gcc
CFLAGS = -Wall -Wextra -Iinclude

LIB_SRC = src/encryption.c src/payment_account.c src/credit_card.c
APP_SRC  = src/main.c $(LIB_SRC)
APP_OBJ  = $(APP_SRC:.c=.o)
TARGET   = build/app

TEST_SRC = tests/test_payment.c $(LIB_SRC)
TEST_OBJ = $(TEST_SRC:.c=.o)
TEST_BIN = build/test_payment

all: $(TARGET)

$(TARGET): $(APP_OBJ)
	mkdir -p build
	$(CC) $(APP_OBJ) -o $(TARGET)

$(TEST_BIN): $(TEST_OBJ)
	mkdir -p build
	$(CC) $(TEST_OBJ) -o $(TEST_BIN)

test: $(TEST_BIN)
	./$(TEST_BIN)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(APP_OBJ) $(TEST_OBJ) $(TARGET) $(TEST_BIN)

.PHONY: all test clean
