#ifndef JSON_HELPER_H
#define JSON_HELPER_H

#include <json-c/json.h>

/** 
 * @brief Creates a JSON response
 * @param status Status code
 * @param message Message
 * @return JSON response
 */
char *create_json_response(char *status, json_object *message);

#endif 