#include "server.h"
#include <stdio.h>

#define PORT 8080

int main() {
    if (!start_server(PORT)) {
        fprintf(stderr, "Failed to start server\n");
        return 1;
    }
    
    printf("Server running on port %d\n", PORT);
    getchar();
    
    return 0;
}
