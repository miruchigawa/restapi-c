/**
 * @file json_helper.h
 * @brief Header file for JSON manipulation utilities
 */
#ifndef JSON_HELPER_H
#define JSON_HELPER_H

#include <json-c/json.h>

/**
 * @brief Creates a JSON error response
 * @param status Error status code
 * @param message Error message
 * @return Dynamically allocated string containing JSON error
 */
char* create_json_error(const char* status, const char* message);

/**
 * @brief Creates a JSON object containing version information
 * @return Dynamically allocated string containing JSON version object
 */
char* create_json_version(char * git_commit_hash, char * version);

#endif 