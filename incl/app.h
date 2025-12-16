#pragma once

#include "audio_device.h"
#include "playlist.h"

typedef struct app {
    audio_device_t audio_device;
    playlist_t playlist;
    int w_width;
    int w_height;
} app_t;

// functions for initializing and deinitializing app members
void app_init(app_t* app);
void app_free(app_t* app);

// starts the application loop
void app_run(app_t* app);
