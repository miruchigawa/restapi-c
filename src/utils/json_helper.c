#include <json-c/json_object.h>
#include <string.h>
#include "json_helper.h"

char *create_json_response(char *status, json_object *message) {
    json_object *json_obj = json_object_new_object();
    json_object_object_add(json_obj, "status", json_object_new_string(status));
    json_object_object_add(json_obj, "message", message);
    return strdup(json_object_to_json_string(json_obj));
}