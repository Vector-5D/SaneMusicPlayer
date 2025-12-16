#pragma once

#include "miniaudio.h"
#include <stdbool.h>

// contains miniaudio engine, sound and some state variables
typedef struct audio_device {
    ma_engine engine;
    ma_sound sound;
    bool initialized;
    bool sound_loaded;
    bool paused;
} audio_device_t;

// functions for initializing and uninitializing miniaudio members
bool audio_device_init(audio_device_t* dev);
void audio_device_free(audio_device_t* dev);

// load a file and play it
bool audio_device_play_file(audio_device_t* dev, const char* path);

// playback controls
bool audio_device_stop(audio_device_t* dev);
bool audio_device_pause(audio_device_t* dev);
bool audio_device_resume(audio_device_t* dev);

// sets a volume from 0.0f to 1.0f
bool audio_device_set_volume(audio_device_t* dev, float volume);
// gets current volume from 0.0f to 1.0f
float audio_device_get_volume(audio_device_t* dev);

// sets playback progress from 0.0f to 1.0f
bool audio_device_set_progress(audio_device_t* dev, float progress);
// gets current playback progress from 0.0f to 1.0f
float audio_device_get_progress(audio_device_t* dev);

// gets the duration of the loaded sound in seconds
float audio_device_get_duration_seconds(audio_device_t* dev);
// gets the position in the track of the loaded sound in seconds
float audio_device_get_position_seconds(audio_device_t* dev);

// returns true if audio device is playing else false
bool audio_device_is_playing(audio_device_t* dev);
// returns true if audio device is paused else false
bool audio_device_is_paused(audio_device_t* dev);
// checks if playback of current file is finished
bool audio_device_is_finished(audio_device_t* dev);
