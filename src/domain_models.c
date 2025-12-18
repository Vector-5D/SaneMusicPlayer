#include "domain_models.h"
#include "logger.h"
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

// =============================================================================
// TRACK IMPLEMENTATION
// =============================================================================

track_t* track_create(const char* path) {
    if (!path) {
        LOG_ERROR("Couldn't create track; path is NULL.");
        return NULL;
    }
    
    track_t* track = calloc(1, sizeof(track_t));
    if (!track) {
        LOG_ERROR("Memory allocation failed; couldn't create track.");
        return NULL;
    }
    
    track->path = strdup(path);
    track->title = strdup("Unknown");
    track->artist = strdup("Unknown Artist");
    track->album = strdup("Unknown Album");
    track->genre = strdup("Unknown");
    track->duration = 0;
    track->year = 0;
    track->track_number = 0;
    
    LOG_INFO("Track created: %s", path);
    return track;
}

void track_free(track_t* track) {
    if (!track) {
        LOG_ERROR("Couldn't free track; track is NULL.");
        return;
    }
    
    free(track->path);
    free(track->title);
    free(track->artist);
    free(track->album);
    free(track->genre);
    free(track);
    
    LOG_INFO("Track freed successfully.");
}

track_t* track_copy(const track_t* track) {
    if (!track) {
        LOG_ERROR("Couldn't copy track; track is NULL.");
        return NULL;
    }
    
    track_t* copy = calloc(1, sizeof(track_t));
    if (!copy) {
        LOG_ERROR("Memory allocation failed; couldn't copy track.");
        return NULL;
    }
    
    copy->path = strdup(track->path);
    copy->title = strdup(track->title);
    copy->artist = strdup(track->artist);
    copy->album = strdup(track->album);
    copy->genre = strdup(track->genre);
    copy->duration = track->duration;
    copy->year = track->year;
    copy->track_number = track->track_number;
    
    LOG_INFO("Track copied: %s", track->path);
    return copy;
}

// track list implementation

track_list_t* track_list_create() {
    track_list_t* list = calloc(1, sizeof(track_list_t));
    if (!list) {
        LOG_ERROR("Memory allocation failed; couldn't create track list.");
        return NULL;
    }
    
    LOG_INFO("Track list created successfully.");
    return list;
}

void track_list_free(track_list_t* list) {
    if (!list) {
        LOG_ERROR("Couldn't free track list; list is NULL.");
        return;
    }
    
    track_list_clear(list);
    free(list->items);
    free(list);
    
    LOG_INFO("Track list freed successfully.");
}

bool track_list_append(track_list_t* list, const track_t* track) {
    if (!list || !track) {
        LOG_ERROR("Couldn't append track to list; list or track is NULL.");
        return false;
    }
    
    if (list->count >= list->capacity) {
        size_t new_capacity = list->capacity == 0 ? 16 : list->capacity * 2;
        track_t* tmp = realloc(list->items, new_capacity * sizeof(track_t));
        if (!tmp) {
            LOG_ERROR("Memory allocation failed; couldn't append track to list.");
            return false;
        }
        list->items = tmp;
        list->capacity = new_capacity;
    }
    
    // copy the track data
    list->items[list->count] = *track;
    list->count++;
    
    LOG_INFO("Track appended to list: %s", track->path);
    return true;
}

bool track_list_remove(track_list_t* list, size_t index) {
    if (!list || index >= list->count) {
        LOG_ERROR("Couldn't remove track from list; list is NULL or index out of bounds.");
        return false;
    }
    
    // free the track's strings
    track_t* t = &list->items[index];
    free(t->path);
    free(t->title);
    free(t->artist);
    free(t->album);
    free(t->genre);
    
    // shift remaining items
    for (size_t i = index; i < list->count - 1; i++) {
        list->items[i] = list->items[i + 1];
    }
    list->count--;
    
    LOG_INFO("Track removed from list at index %zu.", index);
    return true;
}

bool track_list_remove_by_path(track_list_t* list, const char* path) {
    if (!list || !path) {
        LOG_ERROR("Couldn't remove track by path; list or path is NULL.");
        return false;
    }
    
    for (size_t i = 0; i < list->count; i++) {
        if (strcmp(list->items[i].path, path) == 0) {
            LOG_INFO("Track found and removing: %s", path);
            return track_list_remove(list, i);
        }
    }
    
    LOG_WARN("Track not found in list: %s", path);
    return false;
}

bool track_list_clear(track_list_t* list) {
    if (!list) {
        LOG_ERROR("Couldn't clear track list; list is NULL.");
        return false;
    }
    
    for (size_t i = 0; i < list->count; i++) {
        track_t* t = &list->items[i];
        free(t->path);
        free(t->title);
        free(t->artist);
        free(t->album);
        free(t->genre);
    }
    
    list->count = 0;
    LOG_INFO("Track list cleared successfully.");
    return true;
}

track_t* track_list_get(track_list_t* list, size_t index) {
    if (!list || index >= list->count) {
        LOG_ERROR("Couldn't get track from list; list is NULL or index out of bounds.");
        return NULL;
    }
    return &list->items[index];
}

track_t* track_list_find_by_path(track_list_t* list, const char* path) {
    if (!list || !path) {
        LOG_ERROR("Couldn't find track by path; list or path is NULL.");
        return NULL;
    }
    
    for (size_t i = 0; i < list->count; i++) {
        if (strcmp(list->items[i].path, path) == 0) {
            LOG_INFO("Track found in list: %s", path);
            return &list->items[i];
        }
    }
    
    LOG_WARN("Track not found in list: %s", path);
    return NULL;
}

bool track_list_contains(track_list_t* list, const track_t* track) {
    if (!list || !track) {
        LOG_ERROR("Couldn't check if list contains track; list or track is NULL.");
        return false;
    }
    
    return track_list_find_by_path(list, track->path) != NULL;
}

size_t track_list_index_of(track_list_t* list, const track_t* track) {
    if (!list || !track) {
        LOG_ERROR("Couldn't get index of track; list or track is NULL.");
        return SIZE_MAX;
    }
    
    for (size_t i = 0; i < list->count; i++) {
        if (strcmp(list->items[i].path, track->path) == 0) {
            LOG_INFO("Track found at index %zu: %s", i, track->path);
            return i;
        }
    }
    
    LOG_WARN("Track not found in list: %s", track->path);
    return SIZE_MAX;
}

// =============================================================================
// ALBUM IMPLEMENTATION
// =============================================================================

album_t* album_create(const char* title) {
    if (!title) {
        LOG_ERROR("Couldn't create album; title is NULL.");
        return NULL;
    }
    
    album_t* album = calloc(1, sizeof(album_t));
    if (!album) {
        LOG_ERROR("Memory allocation failed; couldn't create album.");
        return NULL;
    }
    
    album->title = strdup(title);
    album->tracks = NULL;
    album->track_count = 0;
    album->track_capacity = 0;
    
    LOG_INFO("Album created: %s", title);
    return album;
}

void album_free(album_t* album) {
    if (!album) {
        LOG_ERROR("Couldn't free album; album is NULL.");
        return;
    }
    
    free(album->title);
    free(album->tracks); // ONLY free the pointer, not the tracks themselves
    free(album);
    
    LOG_INFO("Album freed successfully.");
}

bool album_add_track(album_t* album, const track_t* track) {
    if (!album || !track) {
        LOG_ERROR("Couldn't add track to album; album or track is NULL.");
        return false;
    }

    // first check if track already exists
    for (size_t i = 0; i < album->track_count; i++) {
        if (album->tracks[i] == track) {
            LOG_WARN("Track already exists in album: %s", track->path);
            return false; // track already exists
        }
    }
    
    if (album->track_count >= album->track_capacity) {
        size_t new_capacity = album->track_capacity == 0 ? 8 : album->track_capacity * 2;
        track_t** tmp = realloc(album->tracks, new_capacity * sizeof(track_t*));
        if (!tmp) {
            LOG_ERROR("Memory allocation failed; couldn't add track to album.");
            return false;
        }
        album->tracks = tmp;
        album->track_capacity = new_capacity;
    }
    
    album->tracks[album->track_count++] = track;
    LOG_INFO("Track added to album '%s': %s", album->title, track->path);
    return true;
}

bool album_remove_track(album_t* album, track_t* track) {
    if (!album || !track) {
        LOG_ERROR("Couldn't remove track from album; album or track is NULL.");
        return false;
    }
    
    for (size_t i = 0; i < album->track_count; i++) {
        if (album->tracks[i] == track) {
            // shift remaining pointers down by one
            for (size_t j = i; j < album->track_count - 1; j++) {
                album->tracks[j] = album->tracks[j + 1];
            }
            album->track_count--;
            LOG_INFO("Track removed from album '%s': %s", album->title, track->path);
            return true;
        }
    }
    
    LOG_WARN("Track not found in album '%s': %s", album->title, track->path);
    return false;
}

bool album_has_track(album_t* album, const track_t* track) {
    if (!album || !track) {
        LOG_ERROR("Couldn't check if album has track; album or track is NULL.");
        return false;
    }
    
    for (size_t i = 0; i < album->track_count; i++) {
        if (album->tracks[i] == track) {
            return true;
        }
    }
    
    return false;
}

size_t album_get_track_count(const album_t* album) {
    if (!album) {
        LOG_ERROR("Couldn't get track count; album is NULL.");
        return 0;
    }
    return album->track_count;
}

int album_get_total_duration(const album_t* album) {
    if (!album) {
        LOG_ERROR("Couldn't get total duration; album is NULL.");
        return 0;
    }
    
    int total = 0;
    for (size_t i = 0; i < album->track_count; i++) {
        total += album->tracks[i]->duration;
    }
    
    LOG_INFO("Album '%s' total duration: %d seconds", album->title, total);
    return total;
}

track_t* album_get_track(const album_t* album, size_t index) {
    if (!album || index >= album->track_count) {
        LOG_ERROR("Couldn't get track from album; album is NULL or index out of bounds.");
        return NULL;
    }
    return album->tracks[index];
}

// album list implementation

album_list_t* album_list_create() {
    album_list_t* list = calloc(1, sizeof(album_list_t));
    if (!list) {
        LOG_ERROR("Memory allocation failed; couldn't create album list.");
        return NULL;
    }
    
    LOG_INFO("Album list created successfully.");
    return list;
}

void album_list_free(album_list_t* list) {
    if (!list) {
        LOG_ERROR("Couldn't free album list; list is NULL.");
        return;
    }
    
    album_list_clear(list);
    free(list->items);
    free(list);
    
    LOG_INFO("Album list freed successfully.");
}

bool album_list_append(album_list_t* list, const album_t* album) {
    if (!list || !album) {
        LOG_ERROR("Couldn't append album to list; list or album is NULL.");
        return false;
    }
    
    if (list->count >= list->capacity) {
        size_t new_capacity = list->capacity == 0 ? 16 : list->capacity * 2;
        album_t* tmp = realloc(list->items, new_capacity * sizeof(album_t));
        if (!tmp) {
            LOG_ERROR("Memory allocation failed; couldn't append album to list.");
            return false;
        }
        list->items = tmp;
        list->capacity = new_capacity;
    }
    
    list->items[list->count] = *album;
    list->count++;
    
    LOG_INFO("Album appended to list: %s", album->title);
    return true;
}

bool album_list_remove(album_list_t* list, size_t index) {
    if (!list || index >= list->count) {
        LOG_ERROR("Couldn't remove album from list; list is NULL or index out of bounds.");
        return false;
    }
    
    // free the album
    album_t* a = &list->items[index];
    free(a->title);
    free(a->tracks); // ONLY free the pointer, not the tracks themselves
    
    // shift remaining items
    for (size_t i = index; i < list->count - 1; i++) {
        list->items[i] = list->items[i + 1];
    }
    list->count--;
    
    LOG_INFO("Album removed from list at index %zu.", index);
    return true;
}

bool album_list_remove_by_title(album_list_t* list, const char* title) {
    if (!list || !title) {
        LOG_ERROR("Couldn't remove album by title; list or title is NULL.");
        return false;
    }
    
    for (size_t i = 0; i < list->count; i++) {
        if (strcmp(list->items[i].title, title) == 0) {
            LOG_INFO("Album found and removing: %s", title);
            return album_list_remove(list, i);
        }
    }
    
    LOG_WARN("Album not found in list: %s", title);
    return false;
}

bool album_list_clear(album_list_t* list) {
    if (!list) {
        LOG_ERROR("Couldn't clear album list; list is NULL.");
        return false;
    }
    
    for (size_t i = 0; i < list->count; i++) {
        album_t* a = &list->items[i];
        free(a->title);
        free(a->tracks);
    }
    
    list->count = 0;
    LOG_INFO("Album list cleared successfully.");
    return true;
}

album_t* album_list_get(album_list_t* list, size_t index) {
    if (!list || index >= list->count) {
        LOG_ERROR("Couldn't get album from list; list is NULL or index out of bounds.");
        return NULL;
    }
    return &list->items[index];
}

album_t* album_list_find_by_title(album_list_t* list, const char* title) {
    if (!list || !title) {
        LOG_ERROR("Couldn't find album by title; list or title is NULL.");
        return NULL;
    }
    
    for (size_t i = 0; i < list->count; i++) {
        if (strcmp(list->items[i].title, title) == 0) {
            LOG_INFO("Album found in list: %s", title);
            return &list->items[i];
        }
    }
    
    LOG_WARN("Album not found in list: %s", title);
    return NULL;
}

bool album_list_contains(album_list_t* list, const album_t* album) {
    if (!list || !album) {
        LOG_ERROR("Couldn't check if list contains album; list or album is NULL.");
        return false;
    }
    
    return album_list_find_by_title(list, album->title) != NULL;
}

// =============================================================================
// ARTIST IMPLEMENTATION
// =============================================================================

artist_t* artist_create(const char* name) {
    if (!name) {
        LOG_ERROR("Couldn't create artist; name is NULL.");
        return NULL;
    }
    
    artist_t* artist = calloc(1, sizeof(artist_t));
    if (!artist) {
        LOG_ERROR("Memory allocation failed; couldn't create artist.");
        return NULL;
    }
    
    artist->name = strdup(name);
    artist->albums = NULL;
    artist->album_count = 0;
    artist->album_capacity = 0;
    
    LOG_INFO("Artist created: %s", name);
    return artist;
}

void artist_free(artist_t* artist) {
    if (!artist) {
        LOG_ERROR("Couldn't free artist; artist is NULL.");
        return;
    }
    
    free(artist->name);
    free(artist->albums); // ONLY free the pointer, not the albums themselves
    free(artist);
    
    LOG_INFO("Artist freed successfully.");
}

bool artist_add_album(artist_t* artist, const album_t* album) {
    if (!artist || !album) {
        LOG_ERROR("Couldn't add album to artist; artist or album is NULL.");
        return false;
    }
    
    // Check if already contains this album
    for (size_t i = 0; i < artist->album_count; i++) {
        if (artist->albums[i] == album) {
            LOG_WARN("Album already exists for artist '%s': %s", artist->name, album->title);
            return false; // Already exists
        }
    }
    
    if (artist->album_count >= artist->album_capacity) {
        size_t new_capacity = artist->album_capacity == 0 ? 8 : artist->album_capacity * 2;
        album_t** tmp = realloc(artist->albums, new_capacity * sizeof(album_t*));
        if (!tmp) {
            LOG_ERROR("Memory allocation failed; couldn't add album to artist.");
            return false;
        }
        artist->albums = tmp;
        artist->album_capacity = new_capacity;
    }
    
    artist->albums[artist->album_count++] = album;
    LOG_INFO("Album added to artist '%s': %s", artist->name, album->title);
    return true;
}

bool artist_remove_album(artist_t* artist, album_t* album) {
    if (!artist || !album) {
        LOG_ERROR("Couldn't remove album from artist; artist or album is NULL.");
        return false;
    }
    
    for (size_t i = 0; i < artist->album_count; i++) {
        if (artist->albums[i] == album) {
            // shift remaining pointers
            for (size_t j = i; j < artist->album_count - 1; j++) {
                artist->albums[j] = artist->albums[j + 1];
            }
            artist->album_count--;
            LOG_INFO("Album removed from artist '%s': %s", artist->name, album->title);
            return true;
        }
    }
    
    LOG_WARN("Album not found for artist '%s': %s", artist->name, album->title);
    return false;
}

bool artist_has_album(artist_t* artist, const album_t* album) {
    if (!artist || !album) {
        LOG_ERROR("Couldn't check if artist has album; artist or album is NULL.");
        return false;
    }
    
    for (size_t i = 0; i < artist->album_count; i++) {
        if (artist->albums[i] == album) {
            return true;
        }
    }
    
    return false;
}

size_t artist_get_album_count(const artist_t* artist) {
    if (!artist) {
        LOG_ERROR("Couldn't get album count; artist is NULL.");
        return 0;
    }
    return artist->album_count;
}

album_t* artist_get_album(const artist_t* artist, size_t index) {
    if (!artist || index >= artist->album_count) {
        LOG_ERROR("Couldn't get album from artist; artist is NULL or index out of bounds.");
        return NULL;
    }
    return artist->albums[index];
}

// artist list implementation

artist_list_t* artist_list_create() {
    artist_list_t* list = calloc(1, sizeof(artist_list_t));
    if (!list) {
        LOG_ERROR("Memory allocation failed; couldn't create artist list.");
        return NULL;
    }
    
    LOG_INFO("Artist list created successfully.");
    return list;
}

void artist_list_free(artist_list_t* list) {
    if (!list) {
        LOG_ERROR("Couldn't free artist list; list is NULL.");
        return;
    }
    
    artist_list_clear(list);
    free(list->items);
    free(list);
    
    LOG_INFO("Artist list freed successfully.");
}

bool artist_list_append(artist_list_t* list, const artist_t* artist) {
    if (!list || !artist) {
        LOG_ERROR("Couldn't append artist to list; list or artist is NULL.");
        return false;
    }
    
    if (list->count >= list->capacity) {
        size_t new_capacity = list->capacity == 0 ? 16 : list->capacity * 2;
        artist_t* tmp = realloc(list->items, new_capacity * sizeof(artist_t));
        if (!tmp) {
            LOG_ERROR("Memory allocation failed; couldn't append artist to list.");
            return false;
        }
        list->items = tmp;
        list->capacity = new_capacity;
    }
    
    list->items[list->count] = *artist;
    list->count++;
    
    LOG_INFO("Artist appended to list: %s", artist->name);
    return true;
}

bool artist_list_remove(artist_list_t* list, size_t index) {
    if (!list || index >= list->count) {
        LOG_ERROR("Couldn't remove artist from list; list is NULL or index out of bounds.");
        return false;
    }
    
    // free the artist
    artist_t* a = &list->items[index];
    free(a->name);
    free(a->albums); // ONLY free the pointer, not the albums themselves
    
    // shift remaining items
    for (size_t i = index; i < list->count - 1; i++) {
        list->items[i] = list->items[i + 1];
    }
    list->count--;
    
    LOG_INFO("Artist removed from list at index %zu.", index);
    return true;
}

bool artist_list_remove_by_name(artist_list_t* list, const char* name) {
    if (!list || !name) {
        LOG_ERROR("Couldn't remove artist by name; list or name is NULL.");
        return false;
    }
    
    for (size_t i = 0; i < list->count; i++) {
        if (strcmp(list->items[i].name, name) == 0) {
            LOG_INFO("Artist found and removing: %s", name);
            return artist_list_remove(list, i);
        }
    }
    
    LOG_WARN("Artist not found in list: %s", name);
    return false;
}

bool artist_list_clear(artist_list_t* list) {
    if (!list) {
        LOG_ERROR("Couldn't clear artist list; list is NULL.");
        return false;
    }
    
    for (size_t i = 0; i < list->count; i++) {
        artist_t* a = &list->items[i];
        free(a->name);
        free(a->albums);
    }
    
    list->count = 0;
    LOG_INFO("Artist list cleared successfully.");
    return true;
}

artist_t* artist_list_get(artist_list_t* list, size_t index) {
    if (!list || index >= list->count) {
        LOG_ERROR("Couldn't get artist from list; list is NULL or index out of bounds.");
        return NULL;
    }
    return &list->items[index];
}

artist_t* artist_list_find_by_name(artist_list_t* list, const char* name) {
    if (!list || !name) {
        LOG_ERROR("Couldn't find artist by name; list or name is NULL.");
        return NULL;
    }
    
    for (size_t i = 0; i < list->count; i++) {
        if (strcmp(list->items[i].name, name) == 0) {
            LOG_INFO("Artist found in list: %s", name);
            return &list->items[i];
        }
    }
    
    LOG_WARN("Artist not found in list: %s", name);
    return NULL;
}

bool artist_list_contains(artist_list_t* list, const artist_t* artist) {
    if (!list || !artist) {
        LOG_ERROR("Couldn't check if list contains artist; list or artist is NULL.");
        return false;
    }
    
    return artist_list_find_by_name(list, artist->name) != NULL;
}

// =============================================================================
// GENRE IMPLEMENTATION
// =============================================================================

genre_t* genre_create(const char* name) {
    if (!name) {
        LOG_ERROR("Couldn't create genre; name is NULL.");
        return NULL;
    }
    
    genre_t* genre = calloc(1, sizeof(genre_t));
    if (!genre) {
        LOG_ERROR("Memory allocation failed; couldn't create genre.");
        return NULL;
    }
    
    genre->name = strdup(name);
    genre->albums = NULL;
    genre->album_count = 0;
    genre->album_capacity = 0;
    
    LOG_INFO("Genre created: %s", name);
    return genre;
}

void genre_free(genre_t* genre) {
    if (!genre) {
        LOG_ERROR("Couldn't free genre; genre is NULL.");
        return;
    }
    
    free(genre->name);
    free(genre->albums); // ONLY free the pointer, not the albums themselves
    free(genre);
    
    LOG_INFO("Genre freed successfully.");
}

bool genre_add_album(genre_t* genre, album_t* album) {
    if (!genre || !album) {
        LOG_ERROR("Couldn't add album to genre; genre or album is NULL.");
        return false;
    }
    
    // check if the genre already contains this album
    for (size_t i = 0; i < genre->album_count; i++) {
        if (genre->albums[i] == album) {
            LOG_WARN("Album already exists in genre '%s': %s", genre->name, album->title);
            return false; // already exists
        }
    }
    
    if (genre->album_count >= genre->album_capacity) {
        size_t new_capacity = genre->album_capacity == 0 ? 8 : genre->album_capacity * 2;
        album_t** tmp = realloc(genre->albums, new_capacity * sizeof(album_t*));
        if (!tmp) {
            LOG_ERROR("Memory allocation failed; couldn't add album to genre.");
            return false;
        }
        genre->albums = tmp;
        genre->album_capacity = new_capacity;
    }
    
    genre->albums[genre->album_count++] = album;
    LOG_INFO("Album added to genre '%s': %s", genre->name, album->title);
    return true;
}

bool genre_remove_album(genre_t* genre, album_t* album) {
    if (!genre || !album) {
        LOG_ERROR("Couldn't remove album from genre; genre or album is NULL.");
        return false;
    }
    
    for (size_t i = 0; i < genre->album_count; i++) {
        if (genre->albums[i] == album) {
            // shift remaining pointers
            for (size_t j = i; j < genre->album_count - 1; j++) {
                genre->albums[j] = genre->albums[j + 1];
            }
            genre->album_count--;
            LOG_INFO("Album removed from genre '%s': %s", genre->name, album->title);
            return true;
        }
    }
    
    LOG_WARN("Album not found in genre '%s': %s", genre->name, album->title);
    return false;
}

bool genre_has_album(genre_t* genre, const album_t* album) {
    if (!genre || !album) {
        LOG_ERROR("Couldn't check if genre has album; genre or album is NULL.");
        return false;
    }
    
    for (size_t i = 0; i < genre->album_count; i++) {
        if (genre->albums[i] == album) {
            return true;
        }
    }
    
    return false;
}

size_t genre_get_album_count(const genre_t* genre) {
    if (!genre) {
        LOG_ERROR("Couldn't get album count; genre is NULL.");
        return 0;
    }
    return genre->album_count;
}

album_t* genre_get_album(const genre_t* genre, size_t index) {
    if (!genre || index >= genre->album_count) {
        LOG_ERROR("Couldn't get album from genre; genre is NULL or index out of bounds.");
        return NULL;
    }
    return genre->albums[index];
}

// genre list implementation

genre_list_t* genre_list_create() {
    genre_list_t* list = calloc(1, sizeof(genre_list_t));
    if (!list) {
        LOG_ERROR("Memory allocation failed; couldn't create genre list.");
        return NULL;
    }
    
    LOG_INFO("Genre list created successfully.");
    return list;
}

void genre_list_free(genre_list_t* list) {
    if (!list) {
        LOG_ERROR("Couldn't free genre list; list is NULL.");
        return;
    }
    
    genre_list_clear(list);
    free(list->items);
    free(list);
    
    LOG_INFO("Genre list freed successfully.");
}

bool genre_list_append(genre_list_t* list, genre_t* genre) {
    if (!list || !genre) {
        LOG_ERROR("Couldn't append genre to list; list or genre is NULL.");
        return false;
    }
    
    if (list->count >= list->capacity) {
        size_t new_capacity = list->capacity == 0 ? 16 : list->capacity * 2;
        genre_t* tmp = realloc(list->items, new_capacity * sizeof(genre_t));
        if (!tmp) {
            LOG_ERROR("Memory allocation failed; couldn't append genre to list.");
            return false;
        }
        list->items = tmp;
        list->capacity = new_capacity;
    }
    
    list->items[list->count] = *genre;
    list->count++;
    
    LOG_INFO("Genre appended to list: %s", genre->name);
    return true;
}

bool genre_list_remove(genre_list_t* list, size_t index) {
    if (!list || index >= list->count) {
        LOG_ERROR("Couldn't remove genre from list; list is NULL or index out of bounds.");
        return false;
    }
    
    // free the genre
    genre_t* g = &list->items[index];
    free(g->name);
    free(g->albums); // ONLY free the pointer, not the albums themselves
    
    // shift remaining items
    for (size_t i = index; i < list->count - 1; i++) {
        list->items[i] = list->items[i + 1];
    }
    list->count--;
    
    LOG_INFO("Genre removed from list at index %zu.", index);
    return true;
}

bool genre_list_remove_by_name(genre_list_t* list, const char* name) {
    if (!list || !name) {
        LOG_ERROR("Couldn't remove genre by name; list or name is NULL.");
        return false;
    }
    
    for (size_t i = 0; i < list->count; i++) {
        if (strcmp(list->items[i].name, name) == 0) {
            LOG_INFO("Genre found and removing: %s", name);
            return genre_list_remove(list, i);
        }
    }
    
    LOG_WARN("Genre not found in list: %s", name);
    return false;
}

bool genre_list_clear(genre_list_t* list) {
    if (!list) {
        LOG_ERROR("Couldn't clear genre list; list is NULL.");
        return false;
    }
    
    for (size_t i = 0; i < list->count; i++) {
        genre_t* g = &list->items[i];
        free(g->name);
        free(g->albums);
    }
    
    list->count = 0;
    LOG_INFO("Genre list cleared successfully.");
    return true;
}

genre_t* genre_list_get(genre_list_t* list, size_t index) {
    if (!list || index >= list->count) {
        LOG_ERROR("Couldn't get genre from list; list is NULL or index out of bounds.");
        return NULL;
    }
    return &list->items[index];
}

genre_t* genre_list_find_by_name(genre_list_t* list, const char* name) {
    if (!list || !name) {
        LOG_ERROR("Couldn't find genre by name; list or name is NULL.");
        return NULL;
    }
    
    for (size_t i = 0; i < list->count; i++) {
        if (strcmp(list->items[i].name, name) == 0) {
            LOG_INFO("Genre found in list: %s", name);
            return &list->items[i];
        }
    }
    
    LOG_WARN("Genre not found in list: %s", name);
    return NULL;
}

bool genre_list_contains(genre_list_t* list, const genre_t* genre) {
    if (!list || !genre) {
        LOG_ERROR("Couldn't check if list contains genre; list or genre is NULL.");
        return false;
    }
    
    return genre_list_find_by_name(list, genre->name) != NULL;
}
