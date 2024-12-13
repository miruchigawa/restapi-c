#include "handlers.h"
#include "../utils/json_helper.h"
#include "../utils/version.h"
#include <json-c/json_object.h>
#include <microhttpd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "../lib/mangadex/mangadex.h"

/**
 * @brief Handles GET requests for version endpoint
 * @return Response struct with version data
 */
Response handle_version_get(void) {
    json_object *data = json_object_new_object();
    char version[256];
    snprintf(version, sizeof(version), "Version: %s (Git: %s)", VERSION, GIT_COMMIT_HASH);
    json_object_object_add(data, "version", json_object_new_string(version));


    Response response;
    response.status_code = MHD_HTTP_OK;
    response.content_type = "application/json";
    response.data = create_json_response("OK", data);

    return response;
}

/**
 * @brief Handles 404 Not Found responses
 * @return Response struct with error message in JSON format
 */
Response handle_not_found(void) {
    Response response;
    response.status_code = MHD_HTTP_NOT_FOUND;
    response.content_type = "application/json";
    response.data = create_json_response("NOT_FOUND", json_object_new_string("Route not found"));
    
    return response;
} 

/** 
 * @todo: Implement this
 * @brief Handles GET requests for nhentai releases endpoint
 * @param connection The MHD_Connection object containing request details
 * @return Response struct with nhentai releases data
 */
Response handle_nhentai_releases_get(struct MHD_Connection *connection) {
    int page = 1;
    const char *page_str = MHD_lookup_connection_value(connection, MHD_GET_ARGUMENT_KIND, "page");
    if (page_str) {
        page = atoi(page_str);
    }

    json_object *data = json_object_new_object();
    json_object_object_add(data, "page", json_object_new_int(page));

    Response response;
    response.status_code = MHD_HTTP_OK;
    response.content_type = "application/json";
    response.data = create_json_response("OK", data);

    return response;
}
/**
 * @brief Handles GET requests for mangadex search endpoint
 * @param connection The MHD_Connection object containing request details
 * @return Response struct with mangadex search results
 */
Response handle_mangadex_search_get(struct MHD_Connection *connection) {
    const char *query = MHD_lookup_connection_value(connection, MHD_GET_ARGUMENT_KIND, "q");
    const char *page_str = MHD_lookup_connection_value(connection, MHD_GET_ARGUMENT_KIND, "page");
    const char *limit_str = MHD_lookup_connection_value(connection, MHD_GET_ARGUMENT_KIND, "limit");

    int page = page_str ? atoi(page_str) : 1;
    int limit = limit_str ? atoi(limit_str) : 20;

    if (!query || strlen(query) == 0) {
        Response response;
        response.status_code = MHD_HTTP_BAD_REQUEST;
        response.content_type = "application/json";
        response.data = create_json_response("INVALID_REQUEST", 
            json_object_new_string("Query parameter 'q' is required"));
        return response;
    }

    if (limit <= 0 || limit > MAX_RESULTS) {
        limit = MAX_RESULTS;
    }

    if (page <= 0) {
        page = 1;
    }

    SearchResults *results = mangadex_search(query, page, limit);
    
    Response response;
    if (!results) {
        response.status_code = MHD_HTTP_BAD_REQUEST;
        response.content_type = "application/json";
        response.data = create_json_response("FAILED_TO_MAKE_REQUEST", 
            json_object_new_string(get_error_message()));
        return response;
    }

    json_object *data = json_object_new_object();
    json_object *manga_array = json_object_new_array();

    for (int i = 0; i < results->result_count; i++) {
        MangaInfo *manga = &results->results[i];
        json_object *manga_obj = json_object_new_object();


        json_object_object_add(manga_obj, "id", 
            json_object_new_string(manga->id));
        json_object_object_add(manga_obj, "title", 
            json_object_new_string(manga->title));
        json_object_object_add(manga_obj, "status", 
            json_object_new_string(manga->status));
        json_object_object_add(manga_obj, "release_date", 
            json_object_new_int(manga->release_date));
        json_object_object_add(manga_obj, "content_rating", 
            json_object_new_string(manga->content_rating));
        json_object_object_add(manga_obj, "last_volume", 
            json_object_new_string(manga->last_volume));
        json_object_object_add(manga_obj, "last_chapter", 
            json_object_new_string(manga->last_chapter));
        json_object_object_add(manga_obj, "cover", 
            json_object_new_string(manga->image_url));

        json_object *alt_titles_array = json_object_new_array();
        for (int j = 0; j < manga->alt_titles_count; j++) {
            json_object *alt_title_obj = json_object_new_object();
            json_object_object_add(alt_title_obj, "lang", 
                json_object_new_string(manga->alt_titles[j].lang));
            json_object_object_add(alt_title_obj, "title", 
                json_object_new_string(manga->alt_titles[j].text));
            json_object_array_add(alt_titles_array, alt_title_obj);
        }
        json_object_object_add(manga_obj, "alt_titles", alt_titles_array);

        json_object_array_add(manga_array, manga_obj);
    }

    json_object_object_add(data, "current_page", json_object_new_int(results->current_page));
    json_object_object_add(data, "results", manga_array);
    json_object_object_add(data, "result_count", json_object_new_int(results->result_count));

    response.status_code = MHD_HTTP_OK;
    response.content_type = "application/json";
    response.data = create_json_response("OK", data);

    free_search_results(results);

    return response;
}