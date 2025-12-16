#pragma once

#include "audio_device.h"
#include <limits.h>

// dynamic array of strings
typedef struct tracks {
    char** items;
    size_t count;
    size_t capacity;
} tracks_t;

// contains a dynamic array of strings and the current track index
typedef struct playlist {
    tracks_t* tracks;
    size_t current;
} playlist_t;

// functions for editing the items of a tracks instance
// note: clearing tracks does NOT free it
bool tracks_append(tracks_t* tracks, const char* path);
bool tracks_remove(tracks_t* tracks, size_t index);
bool tracks_clear(tracks_t* tracks);

// frees the provided tracks instance
bool tracks_free(tracks_t* tracks);

// initialize a playlist
bool playlist_init(playlist_t* list);
// deallocate a playlist
bool playlist_free(playlist_t* list);

// append a track to a playlist
bool playlist_append(playlist_t* list, const char* path);
// append multiple tracks to a playlist
bool playlist_append_multiple(playlist_t* list, const char* paths[], size_t count);
// remove a track from a playlist at the given index
bool playlist_remove(playlist_t* list, size_t index);
// clear all tracks from the playlist
// note: clearing playlist does NOT free it
bool playlist_clear(playlist_t* list);

// plays the current track on the provided audio device
bool playlist_play_current(playlist_t* list, audio_device_t* dev);
// plays the next track on the provided audio device (will wrap around)
bool playlist_play_next(playlist_t* list, audio_device_t* dev);
// plays the previous track on the provided audio device (will wrap around)
bool playlist_play_previous(playlist_t* list, audio_device_t* dev);

// gets the index of the current track
size_t playlist_get_current_track(const playlist_t* list);
// gets the path of the current track (pointer to playlist member, NOT valid after free)
char* playlist_get_current_track_path(const playlist_t* list);
// sets the current track to the provided index
bool playlist_set_current_track(playlist_t* list, size_t index);
// resets the playlist index back to zero
bool playlist_reset_track_index(playlist_t* list);

// gets the amount of tracks in the playlist
size_t playlist_count(const playlist_t* list);
// returns true if tracks' items is empty else false
bool playlist_is_empty(const playlist_t* list);
// returns true if index + 1 doesn't go out of bounds else false
bool playlist_has_next(const playlist_t* list);
// returns true if index - 1 doesn't go otu of bounds else false
bool playlist_has_previous(const playlist_t* list);

// for stopping, pausing and resuming, call the functions in audio_device.h
