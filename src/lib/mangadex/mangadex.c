// mangadex.c
#include "mangadex.h"
#include <curl/curl.h>
#include <json-c/json.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#define RATE_LIMIT_REQUESTS 5
#define RATE_LIMIT_WINDOW 1 
#define USER_AGENT "Mozilla/5.0 (iPhone; CPU iPhone OS 12_0 like Mac OS X) AppleWebKit/605.1.15 (KHTML, like Gecko) Version/12.0 Mobile/15E148 Safari/604.1"

static char error_message[256] = {0};
static time_t last_request_times[RATE_LIMIT_REQUESTS] = {0};
static int request_index = 0;

struct MemoryStruct {
    char* memory;
    size_t size;
};

const char* get_error_message(void) {
    return error_message;
}

void init_rate_limiter(void) {
    for (int i = 0; i < RATE_LIMIT_REQUESTS; i++) {
        last_request_times[i] = 0;
    }
}

void wait_for_rate_limit(void) {
    time_t current_time = time(NULL);
    time_t oldest_request = last_request_times[request_index];
    
    if (oldest_request > 0) {
        time_t diff = current_time - oldest_request;
        if (diff < RATE_LIMIT_WINDOW) {
            sleep(RATE_LIMIT_WINDOW - diff);
        }
    }
    
    last_request_times[request_index] = time(NULL);
    request_index = (request_index + 1) % RATE_LIMIT_REQUESTS;
}

static size_t write_callback(void* contents, size_t size, size_t nmemb, void* userp) {
    size_t realsize = size * nmemb;
    struct MemoryStruct* mem = (struct MemoryStruct*)userp;

    char* ptr = realloc(mem->memory, mem->size + realsize + 1);
    if (!ptr) {
        snprintf(error_message, sizeof(error_message), "Memory allocation failed");
        return 0;
    }

    mem->memory = ptr;
    memcpy(&(mem->memory[mem->size]), contents, realsize);
    mem->size += realsize;
    mem->memory[mem->size] = 0;

    return realsize;
}

static void parse_alt_titles(struct json_object* alt_titles_array, MangaInfo* manga) {
    manga->alt_titles_count = 0;
    if (!json_object_is_type(alt_titles_array, json_type_array)) return;

    int array_len = json_object_array_length(alt_titles_array);
    for (int i = 0; i < array_len && i < MAX_ALT_TITLES; i++) {
        struct json_object* alt_title = json_object_array_get_idx(alt_titles_array, i);
        json_object_object_foreach(alt_title, lang, title) {
            strncpy(manga->alt_titles[i].lang, lang, sizeof(manga->alt_titles[i].lang) - 1);
            strncpy(manga->alt_titles[i].text, json_object_get_string(title), 
                   sizeof(manga->alt_titles[i].text) - 1);
            manga->alt_titles_count++;
        }
    }
}

char* fetch_cover_image(const char* id) {
    wait_for_rate_limit();

    CURL* curl;
    CURLcode res;
    struct MemoryStruct chunk = {0};
    char url[MAX_URL_LENGTH];
    
    snprintf(url, sizeof(url), "%s/cover/%s", API_URL, id);
    
    curl = curl_easy_init();
    if (!curl) {
        snprintf(error_message, sizeof(error_message), "Failed to initialize CURL");
        return NULL;
    }

    curl_easy_setopt(curl, CURLOPT_URL, url);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void*)&chunk);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 30L);
    curl_easy_setopt(curl, CURLOPT_USERAGENT, USER_AGENT);

    res = curl_easy_perform(curl);
    curl_easy_cleanup(curl);

    if (res != CURLE_OK) {
        snprintf(error_message, sizeof(error_message), "CURL request failed: %s", 
                curl_easy_strerror(res));
        free(chunk.memory);
        return NULL;
    }

    struct json_object* json = json_tokener_parse(chunk.memory);
    free(chunk.memory);

    if (!json) {
        snprintf(error_message, sizeof(error_message), "Failed to parse JSON response");
        return NULL;
    }

    struct json_object *data, *attributes, *filename;
    if (!json_object_object_get_ex(json, "data", &data) ||
        !json_object_object_get_ex(data, "attributes", &attributes) ||
        !json_object_object_get_ex(attributes, "fileName", &filename)) {
        json_object_put(json);
        snprintf(error_message, sizeof(error_message), "Invalid JSON structure");
        return NULL;
    }

    char* result = strdup(json_object_get_string(filename));
    json_object_put(json);
    
    return result;
}

SearchResults* mangadex_search(const char* query, int page, int limit) {
    if (page <= 0 || limit > MAX_RESULTS || limit * (page - 1) >= 10000) {
        snprintf(error_message, sizeof(error_message), "Invalid search parameters");
        return NULL;
    }

    wait_for_rate_limit();

    CURL* curl;
    CURLcode res;
    struct MemoryStruct chunk = {0};
    char* encoded_query = curl_easy_escape(NULL, query, 0);
    char url[MAX_URL_LENGTH];
    
    snprintf(url, sizeof(url), 
             "%s/manga?limit=%d&title=%s&offset=%d&order[relevance]=desc",
             API_URL, limit, encoded_query, limit * (page - 1));
    
    curl_free(encoded_query);

    curl = curl_easy_init();
    if (!curl) {
        snprintf(error_message, sizeof(error_message), "Failed to initialize CURL");
        return NULL;
    }

    curl_easy_setopt(curl, CURLOPT_URL, url);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void*)&chunk);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 30L);
    curl_easy_setopt(curl, CURLOPT_USERAGENT, USER_AGENT);

    res = curl_easy_perform(curl);
    curl_easy_cleanup(curl);

    if (res != CURLE_OK) {
        snprintf(error_message, sizeof(error_message), "CURL request failed: %s", 
                curl_easy_strerror(res));
        free(chunk.memory);
        return NULL;
    }

    SearchResults* results = calloc(1, sizeof(SearchResults));
    if (!results) {
        snprintf(error_message, sizeof(error_message), "Memory allocation failed");
        free(chunk.memory);
        return NULL;
    }

    results->current_page = page;
    results->results = calloc(limit, sizeof(MangaInfo));
    if (!results->results) {
        snprintf(error_message, sizeof(error_message), "Memory allocation failed");
        free(chunk.memory);
        free(results);
        return NULL;
    }

    struct json_object* json = json_tokener_parse(chunk.memory);
    free(chunk.memory);

    if (!json) {
        snprintf(error_message, sizeof(error_message), "Failed to parse JSON response");
        free_search_results(results);
        return NULL;
    }

    struct json_object* data;
    if (!json_object_object_get_ex(json, "data", &data)) {
        snprintf(error_message, sizeof(error_message), "Invalid JSON structure");
        json_object_put(json);
        free_search_results(results);
        return NULL;
    }

    int array_len = json_object_array_length(data);
    for (int i = 0; i < array_len; i++) {
        struct json_object* manga_obj = json_object_array_get_idx(data, i);
        struct json_object *id, *attributes, *relationships;
        
        json_object_object_get_ex(manga_obj, "id", &id);
        json_object_object_get_ex(manga_obj, "attributes", &attributes);
        json_object_object_get_ex(manga_obj, "relationships", &relationships);

        MangaInfo* manga = &results->results[i];
        strncpy(manga->id, json_object_get_string(id), sizeof(manga->id) - 1);

        // Parse title
        struct json_object* titles;
        if (json_object_object_get_ex(attributes, "title", &titles)) {
            struct json_object* en_title;
            if (json_object_object_get_ex(titles, "en", &en_title)) {
                strncpy(manga->title, json_object_get_string(en_title), 
                        sizeof(manga->title) - 1);
            }
        }

        struct json_object* alt_titles;
        if (json_object_object_get_ex(attributes, "altTitles", &alt_titles)) {
            parse_alt_titles(alt_titles, manga);
        }

        struct json_object* tmp;
        if (json_object_object_get_ex(attributes, "status", &tmp))
            strncpy(manga->status, json_object_get_string(tmp), sizeof(manga->status) - 1);
        
        if (json_object_object_get_ex(attributes, "year", &tmp))
            manga->release_date = json_object_get_int(tmp);
        
        if (json_object_object_get_ex(attributes, "contentRating", &tmp))
            strncpy(manga->content_rating, json_object_get_string(tmp), 
                    sizeof(manga->content_rating) - 1);
        
        if (json_object_object_get_ex(attributes, "lastVolume", &tmp))
            strncpy(manga->last_volume, json_object_get_string(tmp), 
                    sizeof(manga->last_volume) - 1);
        
        if (json_object_object_get_ex(attributes, "lastChapter", &tmp))
            strncpy(manga->last_chapter, json_object_get_string(tmp), 
                    sizeof(manga->last_chapter) - 1);


        int rel_len = json_object_array_length(relationships);
        for (int j = 0; j < rel_len; j++) {
            struct json_object* rel = json_object_array_get_idx(relationships, j);
            struct json_object *type, *rel_id;
            
            json_object_object_get_ex(rel, "type", &type);
            if (strcmp(json_object_get_string(type), "cover_art") == 0) {
                json_object_object_get_ex(rel, "id", &rel_id);
                char* cover_filename = fetch_cover_image(json_object_get_string(rel_id));
                if (cover_filename) {
                    snprintf(manga->image_url, sizeof(manga->image_url),
                            "%s/covers/%s/%s", BASE_URL, manga->id, cover_filename);
                    free(cover_filename);
                }
                break;
            }
        }

        results->result_count++;
    }

    json_object_put(json);
    return results;
}

void free_search_results(SearchResults* results) {
    if (results) {
        free(results->results);
        free(results);
    }
}