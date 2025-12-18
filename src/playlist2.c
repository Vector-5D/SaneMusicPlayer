#include "playlist2.h"
#include "logger.h"
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

bool playlist_init(playlist_t* list) {
    if (!list) {
        LOG_ERROR("Couldn't initialize playlist; list is NULL.");
        return false;
    }

    list->tracks = malloc(sizeof(track_list_t));
    if (!list->tracks) {
        LOG_ERROR("Memory allocation failed; couldn't initialize playlist.");
        return false;
    }

    list->tracks->items = NULL;
    list->tracks->count = 0;
    list->tracks->capacity = 0;
    list->current = 0;

    LOG_INFO("Playlist initialized succesfully.");
    return true;
}

bool playlist_free(playlist_t* list) {
    if (!list || !list->tracks) {
        LOG_ERROR("Couldn't free playlist; list or tracks is NULL.");
        return false;
    }

    track_list_free(list->tracks);
    free(list->tracks);
    list->tracks = NULL;
    list->current = 0;

    LOG_INFO("Playlist uninitialized successfully");
    return true;
}

bool playlist_append(playlist_t* list, track_t* track) {
    if (!list || !list->tracks) {
        LOG_ERROR("Couldn't append to playlist; list or tracks is NULL.");
        return false;
    }
    return track_list_append(list->tracks, track);
}

bool playlist_append_multiple(playlist_t* list, track_t* tracks[], size_t count) {
    if (!list || !list->tracks) {
        LOG_ERROR("Couldn't append multiple tracks; list or tracks is NULL.");
        return false;
    }

    for (size_t i = 0; i < count; i++) {
        if (!track_list_append(list->tracks, tracks[i])) return false;
    }

    return true;
}

bool playlist_remove(playlist_t* list, size_t index) {
    if (!list || !list->tracks) {
        LOG_ERROR("Couldn't remove track; list or tracks is NULL.");
        return false;
    }
    return track_list_remove(list->tracks, index);
}

bool playlist_clear(playlist_t* list) {
    if (!list || !list->tracks) {
        LOG_ERROR("Couldn't clear playlist; list or tracks is NULL.");
        return false;
    }
    list->current = 0;
    return track_list_clear(list->tracks);
}

bool playlist_play_current(playlist_t* list, audio_device_t* dev) {
    if (!list || !list->tracks) {
        LOG_ERROR("Couldn't play current track; list or tracks is NULL.");
        return false;
    } else if (!dev || !dev->initialized) {
        LOG_ERROR("Couldn't play current track; audio device is uninitialized.");
        return false;
    } else if (playlist_count(list) == 0) {
        LOG_WARN("Couldn't play current track; no tracks loaded in list.");
        return false;
    }
    return audio_device_play_file(dev, list->tracks->items[list->current].path);
}

bool playlist_play_next(playlist_t* list, audio_device_t* dev) {
    if (!list || !list->tracks) {
        LOG_ERROR("Couldn't play the next track; list or tracks is NULL.");
        return false;
    } else if (!dev || !dev->initialized) {
        LOG_ERROR("Couldn't play the next track; audio device is uninitialized.");
        return false;
    } else if (playlist_count(list) == 0) {
        LOG_WARN("Couldn't play the next track; no tracks loaded in list.");
        return false;
    }

    bool has_next = playlist_has_next(list);
    size_t next_index = has_next ? list->current + 1 : 0;
    bool success = audio_device_play_file( dev, list->tracks->items[next_index].path);
    if (success) list->current = next_index;
    return success;
}

bool playlist_play_previous(playlist_t* list, audio_device_t* dev) {
    if (!list || !list->tracks) {
        LOG_ERROR("Couldn't play the previous track; list or tracks is NULL.");
        return false;
    } else if (!dev || !dev->initialized) {
        LOG_ERROR("Couldn't play the previous track; audio device is uninitialized.");
        return false;
    } else if (playlist_count(list) == 0) {
        LOG_WARN("Couldn't play the previous track; no tracks loaded in list.");
        return false;
    }

    bool has_previous = playlist_has_previous(list);
    size_t next_index = has_previous ? list->current - 1 : list->tracks->count - 1;
    bool success = audio_device_play_file(dev, list->tracks->items[next_index].path);
    if (success) list->current = next_index;
    return success;
}

size_t playlist_get_current(const playlist_t* list) {
    if (!list || !list->tracks) {
        LOG_ERROR("Couldn't get current track index; list or tracks is NULL.");
        return SIZE_MAX;
    }
    return list->current;
}

track_t* playlist_get_current_track(const playlist_t* list) {
    if (!list || !list->tracks) {
        LOG_ERROR("Couldn't get current track path; list or tracks is NULL.");
        return NULL;
    }
    if (list->tracks->count == 0) {
        LOG_ERROR("Couldn't get current track path; no tracks loaded in list.");
        return NULL;
    }
    
    return &list->tracks->items[list->current];
}

bool playlist_set_current(playlist_t* list, size_t index) {
    if (!list || !list->tracks || index >= list->tracks->count) {
        LOG_ERROR("Couldn't set current track; list, tracks is NULL or index out of bounds.");
        return false;
    }
    list->current = index;
    return true;
}

bool playlist_reset_current(playlist_t* list) {
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

bool playlist_write_to_file(const playlist_t* list, const char* filepath) {
    if (!list || !list->tracks) {
        LOG_ERROR("Couldn't write playlist to file; list or tracks is NULL.");
        return false;
    }
    if (!filepath) {
        LOG_ERROR("Couldn't write playlist to file; filepath is NULL.");
        return false;
    }

    FILE* file = fopen(filepath, "w");
    if (!file) {
        LOG_ERROR("Couldn't open file for writing: %s", filepath);
        return false;
    }

    // Write each track path on a separate line
    for (size_t i = 0; i < list->tracks->count; i++) {
        if (fprintf(file, "%s\n", list->tracks->items[i].path) < 0) {
            LOG_ERROR("Failed to write track to file: %s", list->tracks->items[i].path);
            fclose(file);
            return false;
        }
    }

    fclose(file);
    LOG_INFO("Playlist written to file successfully: %s (%zu tracks)", filepath, list->tracks->count);
    return true;
}
