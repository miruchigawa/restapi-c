/**
 * @file handlers.h
 * @brief Header file containing declarations for HTTP request handlers
 */
#ifndef HANDLERS_H
#define HANDLERS_H

#include "../models/response.h"

/**
 * @brief Handles 404 Not Found responses
 * @return Response struct with error message
 */
Response handle_not_found(void);

/**
 * @brief Handles GET requests for version endpoint
 * @return Response struct with version data
 */
Response handle_version_get(void);

#endif 