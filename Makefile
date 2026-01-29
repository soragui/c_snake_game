# Snake Game Makefile

CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -Isrc
LDFLAGS = -lncurses
DEBUG_FLAGS = -g -DDEBUG
RELEASE_FLAGS = -O2 -DNDEBUG

# Directories
SRCDIR = src
OBJDIR = obj
BINDIR = .

# Source files
SOURCES = $(wildcard $(SRCDIR)/*.c)
OBJECTS = $(SOURCES:$(SRCDIR)/%.c=$(OBJDIR)/%.o)
TARGET = snake_game

# Default target
all: release

# Debug build
debug: CFLAGS += $(DEBUG_FLAGS)
debug: $(TARGET)

# Release build
release: CFLAGS += $(RELEASE_FLAGS)
release: $(TARGET)

# Create target executable
$(TARGET): $(OBJECTS) | $(BINDIR)
	$(CC) $(OBJECTS) -o $(BINDIR)/$(TARGET) $(LDFLAGS)

# Compile object files
$(OBJDIR)/%.o: $(SRCDIR)/%.c | $(OBJDIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Create directories
$(OBJDIR):
	mkdir -p $(OBJDIR)

$(BINDIR):
	mkdir -p $(BINDIR)

# Clean build artifacts
clean:
	rm -rf $(OBJDIR)
	rm -f $(BINDIR)/$(TARGET)

# Install (copy to /usr/local/bin)
install: release
	sudo cp $(BINDIR)/$(TARGET) /usr/local/bin/

# Uninstall
uninstall:
	sudo rm -f /usr/local/bin/$(TARGET)

# Run the game
run: $(TARGET)
	./$(TARGET)

# Check dependencies
deps:
	@echo "Checking for ncurses..."
	@pkg-config --exists ncurses || (echo "ncurses not found. Install with: sudo apt install libncurses5-dev" && exit 1)
	@echo "Dependencies OK!"

.PHONY: all debug release clean install uninstall run deps