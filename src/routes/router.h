/**
 * @file router.h
 * @brief Header file for HTTP request routing functionality
 */
#ifndef ROUTER_H
#define ROUTER_H

#include "../models/response.h"

/**
 * @brief Routes incoming HTTP requests to appropriate handlers
 * @param url The requested URL path
 * @param method The HTTP method (GET, POST, etc.)
 * @return Response struct containing the response data
 */
Response route_request(const char* url, const char* method);

#endif 