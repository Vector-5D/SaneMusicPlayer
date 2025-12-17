#include "app.h"
#include "file_dialog.h"
#include "logger.h"
#include "raylib.h"
#include <stdlib.h>

void handle_input(app_t* app);
void update(app_t* app);
void render(app_t* app);

void app_init(app_t* app) {
    audio_device_init(&app->audio_device);
    playlist_init(&app->playlist);

    app->w_width = 800;
    app->w_height = 450;

    InitWindow(app->w_width, app->w_height, "Sane Music Player");
    SetTargetFPS(144);

    Image icon = LoadImage("assets/icons/icon4.png");
    SetWindowIcon(icon);
    UnloadImage(icon);
    
    LOG_INFO("App initialized successfully.");
}

void app_free(app_t* app) {
    audio_device_free(&app->audio_device);
    playlist_free(&app->playlist);
    CloseWindow();

    LOG_INFO("App deinitialized successfully.");
}

void app_run(app_t* app) {
    while (!WindowShouldClose()) {
        handle_input(app);
        update(app);
        render(app);
    }
}

void handle_input(app_t* app) {
    // playback controls
    if (IsKeyPressed(KEY_LEFT))
        playlist_play_previous(&app->playlist, &app->audio_device);
    if (IsKeyPressed(KEY_RIGHT))
        playlist_play_next(&app->playlist, &app->audio_device);
    
    if (IsKeyPressed(KEY_SPACE)) {
        if (audio_device_is_paused(&app->audio_device)) {
            audio_device_resume(&app->audio_device);
        } else {
            audio_device_pause(&app->audio_device);
        }
    }

    // volume
    if (IsKeyPressed(KEY_DOWN)) {
        float volume = audio_device_get_volume(&app->audio_device);
        volume = volume - 0.05f <= 0.0f ? 0.0f : volume - 0.05f;
        audio_device_set_volume(&app->audio_device, volume);
    }
    if (IsKeyPressed(KEY_UP)) {
        float volume = audio_device_get_volume(&app->audio_device);
        volume = volume + 0.05f >= 1.0f ? 1.0f : volume + 0.05f;
        audio_device_set_volume(&app->audio_device, volume);
    }

    // file IO
    if (IsKeyDown(KEY_LEFT_CONTROL) &&
        IsKeyDown(KEY_LEFT_SHIFT) &&
        IsKeyPressed(KEY_O)) {
        char* folder_path = file_dialog_open_folder();
        if (folder_path) {
            playlist_clear(&app->playlist);
            LOG_INFO("Scanning folder: %s", folder_path);
            playlist_scan_dir_recursive(&app->playlist, folder_path);
            size_t track_count = playlist_count(&app->playlist);
            LOG_INFO("Added %zu tracks from folder.", track_count);
            playlist_play_current(&app->playlist, &app->audio_device);
            free(folder_path);
        }
    } else if (IsKeyDown(KEY_LEFT_CONTROL) &&
               IsKeyPressed(KEY_O)) {
        char* path = file_dialog_open_file("mp3,flac,wav,ogg,m4a");
        if (path) {
            playlist_clear(&app->playlist);
            playlist_append(&app->playlist, path);
            playlist_play_current(&app->playlist, &app->audio_device);
            free(path);
        }
    }
}

void update(app_t* app) {
    app->w_height = GetScreenHeight();
    app->w_width = GetScreenWidth();

    if (audio_device_is_finished(&app->audio_device)) {
        playlist_play_next(&app->playlist, &app->audio_device);
    }
}

void render(app_t* app) {
    BeginDrawing();
    ClearBackground(BLACK);

    EndDrawing();
}
