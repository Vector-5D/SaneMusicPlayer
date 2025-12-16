#include "app.h"
#include "logger.h"
#include "raylib.h"

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

    const char* test_tracks[3] = {
        "/home/rick/source/sane-music-player-4/test_song_1.mp3",
        "/home/rick/source/sane-music-player-4/test_song_2.mp3",
        "/home/rick/source/sane-music-player-4/test_song_3.mp3"  
    };

    playlist_append_multiple(&app->playlist, test_tracks, 3);

    playlist_play_current(&app->playlist, &app->audio_device);
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
