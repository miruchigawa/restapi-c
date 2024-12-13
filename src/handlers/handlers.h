/**
 * @file handlers.h
 * @brief Header file containing declarations for HTTP request handlers
 */
#ifndef HANDLERS_H
#define HANDLERS_H

#include "../models/response.h"
#include <microhttpd.h>
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

/**
 * @brief Handles GET requests for nhentai releases endpoint
 * @return Response struct with nhentai releases data
 */
Response handle_nhentai_releases_get(struct MHD_Connection *connection);

/**
 * @brief Handles GET requests for mangadex search endpoint
 * @return Response struct with mangadex search data
 */
Response handle_mangadex_search_get(struct MHD_Connection *connection);

#endif 