/**
 * @file main.c
 * @brief Main entry point for the HTTP server application
 */
#include "server.h"
#include <stdio.h>

#define PORT 8080

/**
 * @brief Main function that starts the server
 * @return 0 on success, 1 on failure
 */
int main() {
    if (!start_server(PORT)) {
        fprintf(stderr, "Failed to start server\n");
        return 1;
    }
    
    printf("Server running on port %d\n", PORT);
    getchar();
    
    return 0;
}
