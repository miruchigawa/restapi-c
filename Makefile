CC = gcc
CFLAGS = -Wall -I.
LIBS = -lmicrohttpd -ljson-c

GIT_HASH := $(shell ./scripts/get_git_hash.sh)
CFLAGS += -DGIT_COMMIT_HASH=\"$(GIT_HASH)\"

SRCS = src/main.c src/server.c src/routes/router.c src/handlers/handlers.c src/utils/json_helper.c
OBJS = $(SRCS:.c=.o)
TARGET = server

DEPS = src/models/response.h src/models/user.h src/routes/router.h src/utils/version.h

$(TARGET): $(OBJS) $(DEPS)
	$(CC) $(OBJS) -o $(TARGET) $(LIBS)

%.o: %.c $(DEPS)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)