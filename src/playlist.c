#include "playlist.h"
#include "logger.h"
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <dirent.h>
#include <sys/stat.h>
#include <stdint.h>

// helper to check if a file is an audio file
bool is_audio_file(const char* path) {
    const char* ext = strrchr(path, '.');
    if (!ext) return false;

    ext++; // to skip the dot
    return (strcasecmp(ext, "mp3") == 0 ||
            strcasecmp(ext, "flac") == 0 ||
            strcasecmp(ext, "wav") == 0 ||
            strcasecmp(ext, "ogg") == 0 ||
            strcasecmp(ext, "m4a") == 0 ||
            strcasecmp(ext, "opus") == 0 ||
            strcasecmp(ext, "aac") == 0);
}

// helper for comparing strings for qsort
int compare_strings(const void* a, const void* b) {
    return strcmp(*(const char**)a, *(const char**)b);
}

bool tracks_append(tracks_t* tracks, const char* path) {   
    if (!tracks) {
        LOG_ERROR("Couldn't append track; tracks is NULL.");
        return false;
    }

    if (tracks->count >= tracks->capacity) {
        size_t new_capacity = tracks->capacity == 0 ? 1 : tracks->capacity * 2;
        void* tmp = realloc(tracks->items, new_capacity * sizeof(*tracks->items));
        if (!tmp) {
            LOG_ERROR("Memory allocation failed; couldn't append track.");
            return false;
        }
        tracks->items = tmp;
        tracks->capacity = new_capacity;
    }
    
    char* copy = strdup(path);
    if (!copy) {
        LOG_ERROR("Memory allocation failed; couldn't copy track path.");
        return false;
    }
    
    tracks->items[tracks->count++] = copy;
    return true;
}

bool tracks_remove(tracks_t* tracks, size_t index) {
    if (!tracks || index >= tracks->count) {
        LOG_ERROR("Couldn't remove track; tracks is NULL or index out of bounds.");
        return false;
    }

    free(tracks->items[index]);
    for (size_t i = index; i < tracks->count - 1; i++) {
        tracks->items[i] = tracks->items[i + 1];
    }
    tracks->count--;
    return true;
}

bool tracks_clear(tracks_t* tracks) {
    if (!tracks) {
        LOG_ERROR("Couldn't clear tracks; tracks is NULL.");
        return false;
    }

    for (size_t i = 0; i < tracks->count; i++) {
        free(tracks->items[i]);
    }
    tracks->count = 0;
    return true;
}

bool tracks_free(tracks_t* tracks) {
    if (!tracks) {
        LOG_ERROR("Couldn't free tracks; tracks is NULL.");
        return false;
    }

    tracks_clear(tracks);
    free(tracks->items);
    tracks->items = NULL;
    tracks->capacity = 0;
    tracks->count = 0;
    return true;
}

bool playlist_init(playlist_t* list) {
    if (!list) {
        LOG_ERROR("Couldn't initialize playlist; list is NULL.");
        return false;
    }

    list->tracks = malloc(sizeof(tracks_t));
    if (!list->tracks) {
        LOG_ERROR("Memory allocation failed; couldn't initialize playlist.");
        return false;
    }

    list->tracks->items = NULL;
    list->tracks->count = 0;
    list->tracks->capacity = 0;
    list->current = 0;

    LOG_INFO("Playlist initialized successfully.");
    return true;
}

bool playlist_free(playlist_t* list) {
    if (!list || !list->tracks) {
        LOG_ERROR("Couldn't free playlist; list or tracks is NULL.");
        return false;
    }

    tracks_free(list->tracks);
    free(list->tracks);
    list->tracks = NULL;
    list->current = 0;
    
    LOG_INFO("Playlist uninitialized successfully.");
    return true;
}

bool playlist_append(playlist_t* list, const char* path) {
    if (!list || !list->tracks) {
        LOG_ERROR("Couldn't append to playlist; list or tracks is NULL.");
        return false;
    }
    return tracks_append(list->tracks, path);
}

bool playlist_append_multiple(playlist_t* list, const char* paths[], size_t count) {
    if (!list || !list->tracks) {
        LOG_ERROR("Couldn't append multiple tracks; list or tracks is NULL.");
        return false;
    }

    for (size_t i = 0; i < count; i++) {
        if (!tracks_append(list->tracks, paths[i])) return false;
    }
    return true;
}

bool playlist_remove(playlist_t* list, size_t index) {
    if (!list || !list->tracks) {
        LOG_ERROR("Couldn't remove track; list or tracks is NULL.");
        return false;
    }
    return tracks_remove(list->tracks, index);
}

bool playlist_clear(playlist_t* list) {
    if (!list || !list->tracks) {
        LOG_ERROR("Couldn't clear playlist; list or tracks is NULL.");
        return false;
    }
    list->current = 0;
    return tracks_clear(list->tracks);
}

void playlist_scan_dir_recursive(playlist_t* list, const char* dir_path) {
    DIR* dir = opendir(dir_path);
    if (!dir) {
        LOG_ERROR("Failed to open directory: %s", dir_path);
        return;
    }

    // get all entries in the folder first
    // so we can sort alphabetically (good enough for this)
    char** entries = NULL;
    size_t entry_count = 0;
    size_t entry_capacity = 0;

    struct dirent* entry;
    while ((entry = readdir(dir)) != NULL) {
        // skip . and ..
        if (strcmp(entry->d_name, ".") == 0 ||
            strcmp(entry->d_name, "..") == 0) {
            continue;        
        }

        // grow array if it no longer fits
        if (entry_count >= entry_capacity) {
            size_t new_capacity = entry_capacity == 0 ? 16 : entry_capacity * 2;
            char** tmp = realloc(entries, new_capacity * sizeof(char*));
            if (!tmp) {
                LOG_ERROR("Memory allocation failed");
                break;
            }
            entries = tmp;
            entry_capacity = new_capacity;
        }

        entries[entry_count++] = strdup(entry->d_name);
    }
    closedir(dir);

    // sort entries alphabetically
    qsort(entries, entry_count, sizeof(char*), compare_strings);

    for (size_t i = 0; i < entry_count; i++) {
        // build the full path for the entry
        char full_path[4096];
        snprintf(full_path, sizeof(full_path), "%s/%s", dir_path, entries[i]);

        struct stat path_stat;
        if (stat(full_path, &path_stat) == 0) {
            if (S_ISDIR(path_stat.st_mode)) {
                // recursively scan subdirectories
                playlist_scan_dir_recursive(list, full_path);
            } else if (S_ISREG(path_stat.st_mode)) {
                // only add audio files using helper function
                if (is_audio_file(full_path)) {
                    playlist_append(list, full_path);
                    LOG_INFO("Added: %s", full_path);
                }
            }
        }
    }
}

bool playlist_play_current(playlist_t* list, audio_device_t* dev) {
    if (!list || !list->tracks) {
        LOG_ERROR("Couldn't play current track; list or tracks is NULL.");
        return false;
    }
    if (!dev || !dev->initialized) {
        LOG_ERROR("Couldn't play current track; audio device is uninitialized.");
        return false;
    }
    if (list->tracks->count == 0) {
        LOG_ERROR("Couldn't play current track; no tracks loaded in list.");
        return false;
    }
    return audio_device_play_file(dev, list->tracks->items[list->current]);
}

bool playlist_play_next(playlist_t* list, audio_device_t* dev) {
    if (!list || !list->tracks) {
        LOG_ERROR("Couldn't play next track; list or tracks is NULL.");
        return false;
    }
    if (!dev || !dev->initialized) {
        LOG_ERROR("Couldn't play next track; audio device is uninitialized.");
        return false;
    }
    if (list->tracks->count == 0) {
        LOG_ERROR("Couldn't play next track; no tracks loaded in list.");
        return false;
    }

    bool has_next = playlist_has_next(list);
    bool success = audio_device_play_file(dev,
        list->tracks->items[has_next ? list->current + 1 : 0]
    );
    if (success) list->current = has_next ? list->current + 1 : 0;
    return success;
}

bool playlist_play_previous(playlist_t* list, audio_device_t* dev) {
    if (!list || !list->tracks) {
        LOG_ERROR("Couldn't play previous track; list or tracks is NULL.");
        return false;
    }
    if (!dev || !dev->initialized) {
        LOG_ERROR("Couldn't play previous track; audio device is uninitialized.");
        return false;
    }
    if (list->tracks->count == 0) {
        LOG_ERROR("Couldn't play previous track; no tracks loaded in list.");
        return false;
    }

    bool has_previous = playlist_has_previous(list);
    size_t next_index = has_previous ? list->current - 1 : list->tracks->count - 1;
    bool success = audio_device_play_file(dev, list->tracks->items[next_index]);
    if (success) list->current = next_index;
    return success;
}

size_t playlist_get_current_track(const playlist_t* list) {
    if (!list || !list->tracks) {
        LOG_ERROR("Couldn't get current track index; list or tracks is NULL.");
        return SIZE_MAX;
    }
    return list->current;
}

char* playlist_get_current_track_path(const playlist_t* list) {
    if (!list || !list->tracks) {
        LOG_ERROR("Couldn't get current track path; list or tracks is NULL.");
        return NULL;
    }
    if (list->tracks->count == 0) {
        LOG_ERROR("Couldn't get current track path; no tracks loaded in list.");
        return NULL;
    }
    
    return list->tracks->items[list->current];
}

bool playlist_set_current_track(playlist_t* list, size_t index) {
    if (!list || !list->tracks || index >= list->tracks->count) {
        LOG_ERROR("Couldn't set current track; list, tracks is NULL or index out of bounds.");
        return false;
    }
    list->current = index;
    return true;
}

bool playlist_reset_track_index(playlist_t* list) {
    if (!list || !list->tracks) {
        LOG_ERROR("Couldn't reset track index; list or tracks is NULL.");
        return false;
    }
    list->current = 0;
    return true;
}

size_t playlist_count(const playlist_t* list) {
    if (!list || !list->tracks) {
        LOG_ERROR("Couldn't get playlist count; list or tracks is NULL.");
        return SIZE_MAX;
    }
    return list->tracks->count;
}

bool playlist_is_empty(const playlist_t* list) {
    if (!list || !list->tracks) {
        LOG_ERROR("Couldn't check if playlist is empty; list or tracks is NULL.");
        return true;
    }
    return list->tracks->count == 0;
}

bool playlist_has_next(const playlist_t* list) {
    if (!list || !list->tracks) {
        LOG_ERROR("Couldn't check for next track; list or tracks is NULL.");
        return false;
    }
    return list->current + 1 < list->tracks->count;
}

bool playlist_has_previous(const playlist_t* list) {
    if (!list || !list->tracks) {
        LOG_ERROR("Couldn't check for previous track; list or tracks is NULL.");
        return false;
    }
    return list->current != 0;
}

