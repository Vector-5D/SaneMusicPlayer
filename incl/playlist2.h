#pragma once

#include "audio_device.h"
#include "domain_models.h"
#include <limits.h>

// contains a list of tracks and the current track index
typedef struct playlist {
    track_list_t* tracks;
    size_t current;
} playlist_t;

// initialize a playlist
bool playlist_init(playlist_t* list);
// deallocate a playlist
bool playlist_free(playlist_t* list);

// append a track to a playlist
bool playlist_append(playlist_t* list, track_t* track);
// append an array of tracks to a playlist
bool playlist_append_multiple(playlist_t* list, track_t* tracks[], size_t count);
// remove a track from a playlist at the given index
bool playlist_remove(playlist_t* list, size_t index);
// clear all tracks from the playlist
// note: clearing does NOT free it
bool playlist_clear(playlist_t* list);

// plays the current track on the provided audio device
bool playlist_play_current(playlist_t* list, audio_device_t* dev);
// plays the next track on the provided audio device (will wrap around)
bool playlist_play_next(playlist_t* list, audio_device_t* dev);
// plays the previous track on the provided audio device (will wrap around)
bool playlist_play_previous(playlist_t* list, audio_device_t* dev);

// get the index of the current track
size_t playlist_get_current(const playlist_t* list);
// gets the path of the current track
// note: pointer to track in playlist, NOT valid after free
track_t* playlist_get_current_track(const playlist_t* list);
// sets the current track to the provided index
bool playlist_set_current(playlist_t* list, size_t index);
// resets the playlist index back to zero
bool playlist_reset_current(playlist_t* list);

// gets the amount of tracks in the playlist
size_t playlist_count(const playlist_t* list);
// returns true if tracks' items is empty else false
bool playlist_is_empty(const playlist_t* list);
// returns true if index + 1 doesn't go out of bounds else false
bool playlist_has_next(const playlist_t* list);
// returns true if index - 1 doesn't go out of bounds else false
bool playlist_has_previous(const playlist_t* list);

// writes the paths of tracks in the playlist seperated by newlines to the filepath
bool playlist_write_to_file(const playlist_t* list, const char* filepath);

// for stopping, pausing and resuming, call audio device functions
