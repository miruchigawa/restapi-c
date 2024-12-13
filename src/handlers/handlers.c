/**
 * @file handlers.c
 * @brief Implementation of HTTP request handlers
 */
#include "handlers.h"
#include "../utils/json_helper.h"
#include "../utils/version.h"
#include <microhttpd.h>

/**
 * @brief Handles GET requests for version endpoint
 * @return Response struct with version data
 */
Response handle_version_get(void) {
    Response response;
    response.status_code = MHD_HTTP_OK;
    response.content_type = "application/json";
    response.data = create_json_version(GIT_COMMIT_HASH, VERSION);

    return response;
}

/**
 * @brief Handles 404 Not Found responses
 * @return Response struct with error message in JSON format
 */
Response handle_not_found(void) {
    Response response;
    response.status_code = MHD_HTTP_NOT_FOUND;
    response.content_type = "application/json";
    response.data = create_json_error("NOT_FOUND", "Route not found");
    
    return response;
} 