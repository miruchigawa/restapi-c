CC = gcc
CFLAGS = -Wall -I.
LIBS = -lmicrohttpd -ljson-c
GIT_HASH := $(shell git rev-parse --short HEAD 2>/dev/null || echo "unknown")
CFLAGS += -DGIT_COMMIT_HASH=\"$(GIT_HASH)\"

SRCS = src/main.c src/server.c src/routes/router.c src/handlers/handlers.c src/utils/json_helper.c
OBJS = $(SRCS:.c=.o)
TARGET = bin/server

WATCH_SRC = src/utils/watch.c
WATCH_TARGET = bin/watch

DEPS = src/models/response.h src/routes/router.h src/utils/version.h

GREEN = \033[32m
YELLOW = \033[33m
BLUE = \033[34m
RESET = \033[0m

$(TARGET): $(OBJS) $(DEPS)
	@mkdir -p bin
	@echo "$(BLUE)Linking$(RESET) $(TARGET)..."
	@$(CC) $(OBJS) -o $(TARGET) $(LIBS)
	@echo "$(GREEN)Build complete!$(RESET) Binary: $(TARGET)"

%.o: %.c $(DEPS)
	@echo "$(YELLOW)Compiling$(RESET) $<..."
	@$(CC) $(CFLAGS) -c $< -o $@

watch: $(WATCH_SRC)
	@mkdir -p bin
	@echo "$(BLUE)Building$(RESET) file watcher..."
	@$(CC) $(WATCH_SRC) -o $(WATCH_TARGET)
	@echo "$(GREEN)Watch utility built!$(RESET)"
	@./$(WATCH_TARGET)

clean:
	@echo "$(BLUE)Cleaning$(RESET) build files..."
	@rm -f $(OBJS) $(TARGET) $(WATCH_TARGET)
	@echo "$(GREEN)Clean complete!$(RESET)"

.PHONY: clean watch