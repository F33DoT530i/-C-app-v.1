CC     = gcc
CFLAGS = -Wall -Wextra -Iinclude

# ---- library objects (shared between app and tests) ----
LIB_SRC = src/credit.c src/card.c
LIB_OBJ = $(LIB_SRC:.c=.o)

# ---- main application ----
APP_SRC = src/main.c
APP_OBJ = $(APP_SRC:.c=.o)
TARGET  = build/app

# ---- test binaries ----
TEST_CREDIT = build/test_credit
TEST_CARD   = build/test_card

all: $(TARGET)

$(TARGET): $(LIB_OBJ) $(APP_OBJ)
	mkdir -p build
	$(CC) $(LIB_OBJ) $(APP_OBJ) -o $(TARGET)

$(TEST_CREDIT): $(LIB_OBJ) tests/test_credit.c
	mkdir -p build
	$(CC) $(CFLAGS) $(LIB_OBJ) tests/test_credit.c -o $(TEST_CREDIT)

$(TEST_CARD): $(LIB_OBJ) tests/test_card.c
	mkdir -p build
	$(CC) $(CFLAGS) $(LIB_OBJ) tests/test_card.c -o $(TEST_CARD)

test: $(TEST_CREDIT) $(TEST_CARD)
	./$(TEST_CREDIT)
	./$(TEST_CARD)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(LIB_OBJ) $(APP_OBJ) $(TARGET) $(TEST_CREDIT) $(TEST_CARD)

.PHONY: all test clean
