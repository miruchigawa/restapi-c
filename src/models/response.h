/**
 * @file response.h
 * @brief Definition of HTTP response structure
 */
#ifndef RESPONSE_H
#define RESPONSE_H

/**
 * @struct Response
 * @brief Structure representing an HTTP response
 * @var status_code HTTP status code
 * @var content_type MIME type of the response
 * @var data Response body content
 */
typedef struct {
    int status_code;
    const char* content_type;
    char* data;
} Response;

#endif 