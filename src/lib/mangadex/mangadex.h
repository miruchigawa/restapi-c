#ifndef MANGADEX_H
#define MANGADEX_H

#include <stdbool.h>

#define BASE_URL "https://mangadex.org"
#define API_URL "https://api.mangadex.org"
#define MAX_TITLE_LENGTH 256
#define MAX_DESC_LENGTH 4096
#define MAX_RESULTS 100
#define MAX_ALT_TITLES 20
#define MAX_URL_LENGTH 512

typedef struct {
    char text[MAX_TITLE_LENGTH];
    char lang[8];
} AltTitle;

typedef struct {
    char id[64];
    char title[MAX_TITLE_LENGTH];
    AltTitle alt_titles[MAX_ALT_TITLES];
    int alt_titles_count;
    char description[MAX_DESC_LENGTH];
    char status[32];
    int release_date;
    char content_rating[32];
    char last_volume[32];
    char last_chapter[32];
    char image_url[MAX_URL_LENGTH];
} MangaInfo;

typedef struct {
    int current_page;
    MangaInfo* results;
    int result_count;
    int total_results;
    bool has_more;
} SearchResults;

SearchResults* mangadex_search(const char* query, int page, int limit);
char* fetch_cover_image(const char* id);
void free_search_results(SearchResults* results);
const char* get_error_message(void);

void init_rate_limiter(void);
void wait_for_rate_limit(void);

#endif