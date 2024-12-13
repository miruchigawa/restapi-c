/**
 * @file server.h
 * @brief Header file for HTTP server functionality
 */
#ifndef SERVER_H
#define SERVER_H

/**
 * @brief Starts the HTTP server
 * @param port Port number to listen on
 * @return 1 on success, 0 on failure
 */
int start_server(unsigned int port);

#endif 