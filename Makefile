CC=gcc
CFLAGS=-Iinclude -Wall -Wextra
BUILD_DIR=build
TARGET=$(BUILD_DIR)/expense_tracker

SRCS = src/main.c src/ui.c src/expense.c src/fileops.c src/encrypt.c

all: $(TARGET)

$(BUILD_DIR):
	@echo "Creating build directory..."
	@mkdir -p $(BUILD_DIR)

$(TARGET): $(SRCS) | $(BUILD_DIR)
	@echo "Building expense tracker..."
	$(CC) $(SRCS) -o $(TARGET) $(CFLAGS)
	@echo "Build complete! Run with: ./$(TARGET)"

clean:
	@echo "Cleaning build..."
	rm -f $(TARGET)

run: $(TARGET)
	@echo "Running program..."
	./$(TARGET)

.PHONY: all clean run