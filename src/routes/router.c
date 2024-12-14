#include "router.h"
#include "../handlers/handlers.h"
#include <string.h>
#include <stdio.h>
/**
 * @brief Routes incoming HTTP requests to appropriate handlers
 * @param url The requested URL path
 * @param method The HTTP method (GET, POST, etc.)
 * @return Response struct containing the response data
 */
Response route_request(const char* url, const char* method, struct MHD_Connection *connection) {
    char path[100];

    snprintf(path, sizeof(path), "%s %s", method, url);

    // Version Route
    if (strcmp(path, "GET /api/version") == 0) {
        return handle_version_get();
    }


    return handle_not_found();
} 