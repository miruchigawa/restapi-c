#include "handlers.h"
#include "../utils/json_helper.h"
#include "../utils/version.h"
#include <json-c/json_object.h>
#include <microhttpd.h>
#include <stdio.h>
Response handle_version_get(void) {
    json_object *data = json_object_new_object();
    char version[256];
    snprintf(version, sizeof(version), "Version: %s (Git: %s)", VERSION, GIT_COMMIT_HASH);
    json_object_object_add(data, "version", json_object_new_string(version));

    return (Response) {
        .status_code = MHD_HTTP_OK,
        .content_type = "application/json",
        .data = create_json_response("OK", data)
    };
}

Response handle_not_found(void) {
    return (Response) {
        .status_code = MHD_HTTP_NOT_FOUND,
        .content_type = "application/json",
        .data = create_json_response("NOT_FOUND", json_object_new_string("Route not found"))
    };
}
