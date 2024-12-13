
#include <json-c/json_object.h>
#include <json-c/printbuf.h>
#include <string.h>
#include <stdio.h>
char* create_json_error(const char* status, const char* message) {
    struct json_object *json_obj = json_object_new_object();
    json_object_object_add(json_obj, "status", json_object_new_string(status));
    json_object_object_add(json_obj, "message", json_object_new_string(message));
    
    char* result = strdup(json_object_to_json_string(json_obj));
    json_object_put(json_obj);
    return result;
}

char* create_json_version(char * git_commit_hash, char * version) {
    struct json_object *json_obj = json_object_new_object();
    struct json_object *data = json_object_new_object();
    json_object_object_add(json_obj, "status", json_object_new_string("OK"));
    json_object_object_add(json_obj, "data", data);

    char version_str[256];
    snprintf(version_str, sizeof(version_str), "Version: %s (Git: %s)", version, git_commit_hash);
    json_object_object_add(data, "version", json_object_new_string(version_str));

    char* result = strdup(json_object_to_json_string(json_obj));
    json_object_put(json_obj);
    return result;
}