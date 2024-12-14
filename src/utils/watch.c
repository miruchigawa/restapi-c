#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/inotify.h>
#include <sys/wait.h>
#include <signal.h>
#include <string.h>
#include <time.h>

#define EVENT_SIZE (sizeof(struct inotify_event))
#define BUF_LEN (1024 * (EVENT_SIZE + 16))
#define MAX_PATHS 10
#define DEBOUNCE_TIME 1 

pid_t server_pid = -1;
time_t last_rebuild = 0;

void kill_server() {
    if (server_pid > 0) {
        kill(server_pid, SIGTERM);
        waitpid(server_pid, NULL, 0);
        server_pid = -1;
    }
}

void start_server() {
    server_pid = fork();
    if (server_pid == 0) {
        execl("./bin/server", "server", NULL);
        exit(1);
    }
}

void rebuild_and_restart() {
    time_t current_time = time(NULL);
    
    if (current_time - last_rebuild < DEBOUNCE_TIME) {
        return;
    }
    last_rebuild = current_time;

    printf("\n🔄 Changes detected, rebuilding...\n");

    kill_server();

    int make_status = system("make");
    if (make_status != 0) {
        printf("❌ Build failed!\n");
        return;
    }
    
    printf("✅ Build successful!\n");
    printf("🚀 Restarting server...\n\n");
    
    start_server();
}

void handle_signal(int sig) {
    if (sig == SIGINT) {
        printf("\n👋 Shutting down watcher...\n");
        kill_server();
        exit(0);
    }
}

int watch_directory(const char *path, int fd) {
    int wd = inotify_add_watch(fd, path, 
        IN_MODIFY | IN_CREATE | IN_DELETE | 
        IN_MOVED_FROM | IN_MOVED_TO);
    
    if (wd < 0) {
        printf("⚠️  Could not watch directory: %s\n", path);
        return -1;
    }
    return wd;
}

int main() {
    int fd = inotify_init();
    if (fd < 0) {
        perror("inotify_init");
        return 1;
    }

    const char *watch_paths[] = {
        "src",
        "src/handlers",
        "src/models",
        "src/routes",
        "src/utils",
        "src/lib",
        "src/lib/mangadex",
        "src/lib/nhentai"
    };
    
    int watch_count = sizeof(watch_paths) / sizeof(watch_paths[0]);
    for (int i = 0; i < watch_count; i++) {
        watch_directory(watch_paths[i], fd);
    }

    signal(SIGINT, handle_signal);
    
    printf("👀 Watching for changes in source files...\n");
    printf("Press Ctrl+C to stop\n\n");
    
    rebuild_and_restart();

    char buffer[BUF_LEN];
    while (1) {
        int length = read(fd, buffer, BUF_LEN);
        if (length < 0) {
            perror("read");
            return 1;
        }

        int i = 0;
        while (i < length) {
            struct inotify_event *event = (struct inotify_event *)&buffer[i];
            if (event->len) {
                if (!(event->mask & IN_ISDIR) && 
                    (strstr(event->name, ".c") || 
                     strstr(event->name, ".h") || 
                     strstr(event->name, "Makefile"))) {
                    
                    if (!strstr(event->name, ".swp") && 
                        !strstr(event->name, ".swx") && 
                        !strstr(event->name, "~")) {
                        rebuild_and_restart();
                        break;
                    }
                }
            }
            i += EVENT_SIZE + event->len;
        }
    }

    return 0;
}
