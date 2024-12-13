/**
 * @file server.c
 * @brief Implementation of HTTP server using libmicrohttpd
 */
#include "server.h"
#include "routes/router.h"
#include <microhttpd.h>
#include <stdlib.h>
#include <string.h>

/**
 * @brief Callback handler for incoming HTTP requests
 * @param cls User-defined closure
 * @param connection Connection handle
 * @param url Requested URL
 * @param method HTTP method
 * @param version HTTP version
 * @param upload_data Upload data
 * @param upload_data_size Size of upload data
 * @param con_cls Connection closure
 * @return MHD_YES on success, MHD_NO on failure
 */
static enum MHD_Result request_handler(void *cls,
                         struct MHD_Connection *connection,
                         const char *url,
                         const char *method,
                         const char *version,
                         const char *upload_data,
                         size_t *upload_data_size,
                         void **con_cls) {
    Response response = route_request(url, method);
    struct MHD_Response *mhd_response;
    int ret;

    mhd_response = MHD_create_response_from_buffer(
        strlen(response.data),
        (void*)response.data,
        MHD_RESPMEM_MUST_COPY
    );
    
    MHD_add_response_header(mhd_response, "Content-Type", response.content_type);
    ret = MHD_queue_response(connection, response.status_code, mhd_response);
    
    MHD_destroy_response(mhd_response);
    free(response.data);
    
    return ret;
}

/**
 * @brief Starts the HTTP server on specified port
 * @param port Port number to listen on
 * @return 1 on success, 0 on failure
 */
int start_server(unsigned int port) {
    struct MHD_Daemon *daemon;
    
    daemon = MHD_start_daemon(
        MHD_USE_AUTO | MHD_USE_INTERNAL_POLLING_THREAD,
        port,
        NULL,
        NULL,
        &request_handler,
        NULL,
        MHD_OPTION_END
    );
    
    if (daemon == NULL) {
        return 0;
    }
    
    return 1;
} 