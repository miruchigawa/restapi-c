/**
 * @file handlers.c
 * @brief Implementation of HTTP request handlers
 */
#include "handlers.h"
#include "../utils/json_helper.h"
#include "../utils/version.h"
#include <json-c/json_object.h>
#include <microhttpd.h>
#include <stdio.h>
/**
 * @brief Handles GET requests for version endpoint
 * @return Response struct with version data
 */
Response handle_version_get(void) {
    json_object *data = json_object_new_object();
    char version[256];
    snprintf(version, sizeof(version), "Version: %s (Git: %s)", VERSION, GIT_COMMIT_HASH);
    json_object_object_add(data, "version", json_object_new_string(version));


    Response response;
    response.status_code = MHD_HTTP_OK;
    response.content_type = "application/json";
    response.data = create_json_response("OK", data);

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
    response.data = create_json_response("NOT_FOUND", json_object_new_string("Route not found"));
    
    return response;
} 

/**
 * @brief Handles GET requests for nhentai releases endpoint
 * @return Response struct with nhentai releases data
 */
Response handle_nhentai_releases_get(void) {
    Response response;
    response.status_code = MHD_HTTP_OK;
    response.content_type = "application/json";
    response.data = create_json_response("OK", json_object_new_string("Nhentai releases"));

    return response;
}