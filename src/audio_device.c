#define MINIAUDIO_IMPLEMENTATION
#include "audio_device.h"
#include "logger.h"

// helper function for clamping into a 0.0f to 1.0f range
static inline float clamp01(float x) {
    if (x < 0.0f) return 0.0f;
    if (x > 1.0f) return 1.0f;
    return x;
}

bool audio_device_init(audio_device_t* dev) {
   ma_result result = ma_engine_init(nullptr, &dev->engine);
   
   if (result != MA_SUCCESS) {
       LOG_ERROR(
           "Failed to initialize audio device; %s",
           ma_result_description(result)
       );
       return false;
   }

   dev->initialized = true;
   dev->sound_loaded = false;
   dev->paused = false;
   
   LOG_INFO("Audio device initialized successfully.");
   return true;
}

void audio_device_free(audio_device_t* dev) {
    if (dev->sound_loaded) ma_sound_uninit(&dev->sound);
    ma_engine_uninit(&dev->engine);
    dev->initialized = false;
    dev->sound_loaded = false;
    dev->paused = false;
    LOG_INFO("Audio device uninitialized successfully.");
}

bool audio_device_play_file(audio_device_t* dev, const char* path) {
    if (!dev) {
        LOG_ERROR("Failed to play file; audio device is NULL.");
        return false;
    }
    if (!path) {
        LOG_ERROR("Failed to play file; given path is NULL.");
        return false;
    }
    
    float volume;
    if (dev->sound_loaded) {
        volume = audio_device_get_volume(dev);
        ma_sound_uninit(&dev->sound);
    } else {
        volume = 1.0f;
    }

    ma_result result = ma_sound_init_from_file(
        &dev->engine,
        path,
        MA_SOUND_FLAG_STREAM,
        nullptr,
        nullptr,
        &dev->sound
    );

    if (result != MA_SUCCESS) {
        LOG_ERROR(
            "Failed to load sound; %s",
            ma_result_description(result)
        );
        dev->sound_loaded = false;
        dev->paused = false;
        return false;
    }

    dev->sound_loaded = true;
    result = ma_sound_start(&dev->sound);

    if (result != MA_SUCCESS) {
        LOG_ERROR(
            "Failed to start sound; %s",
            ma_result_description(result)
        );
        ma_sound_uninit(&dev->sound);
        dev->sound_loaded = false;
        dev->paused = false;
        return false;
    }

    audio_device_set_volume(dev, volume);
    LOG_INFO("Playing file: %s", path);
    return true;
}

bool audio_device_stop(audio_device_t* dev) {
    if (!dev) {
        LOG_ERROR("Couldn't stop; audio device is NULL.");
        return false;
    } else if (!dev->sound_loaded) {
        LOG_WARN("Couldn't stop; no sound is loaded.");
        return false;
    }

    ma_sound_stop(&dev->sound);
    ma_sound_uninit(&dev->sound);
    dev->sound_loaded = false;
    dev->paused = false;

    LOG_INFO("Playback stopped.");
    return true;
}

bool audio_device_pause(audio_device_t* dev) {
    if (!dev) {
        LOG_ERROR("Couldn't pause; audio device is NULL.");
        return false;
    } else if (!dev->sound_loaded) {
        LOG_WARN("Couldn't pause; no sound is loaded.");
        return false;
    } else if (dev->paused) {
        LOG_WARN("Couldn't pause; already paused.");
        return false;
    }

    ma_sound_stop(&dev->sound);
    dev->paused = true;

    LOG_INFO("Playback paused.");
    return true;
}

bool audio_device_resume(audio_device_t* dev) {
    if (!dev) {
        LOG_ERROR("Couldn't resume; audio device is NULL.");
        return false;
    } else if (!dev->sound_loaded) {
        LOG_WARN("Couldn't resume; no sound is loaded.");
        return false;
    } else if (!dev->paused) {
        LOG_WARN("Couldn't resume; sound isn't paused.");
        return false;
    }

    ma_result result = ma_sound_start(&dev->sound);
    if (result != MA_SUCCESS) {
        LOG_ERROR(
            "Failed to resume sound; %s",
            ma_result_description(result)
        );
        return false;
    }

    dev->paused = false;
    LOG_INFO("Playback resumed.");
    return true;
}

bool audio_device_set_volume(audio_device_t* dev, float volume) {
    if (!dev) {
        LOG_ERROR("Couldn't set volume; audio device is NULL.");
    } else if (!dev->sound_loaded) {
        LOG_WARN("Couldn't set volume; no sound is loaded.");
        return false;
    }

    volume = clamp01(volume);
    ma_sound_set_volume(&dev->sound, volume);

    LOG_INFO("Volume set to %.2f", volume);
    return true;
}

float audio_device_get_volume(audio_device_t* dev) {
    if (!dev) {
        LOG_ERROR("Couldn't get volume; audio device is NULL.");
        return -1.0f;
    }
    
    if (!dev->sound_loaded) return 0.0f;

    return ma_sound_get_volume(&dev->sound);
}

bool audio_device_set_progress(audio_device_t* dev, float progress) {
    if (!dev) {
        LOG_ERROR("Couldn't set progress; audio device is NULL.");
        return false;
    } else if (!dev->sound_loaded) {
        LOG_WARN("Couldn't set progress; no sound is loaded.");
        return false;
    }

    progress = clamp01(progress);

    ma_uint64 length_in_frames;
    ma_result result = ma_sound_get_length_in_pcm_frames(
        &dev->sound, &length_in_frames
    );

    if (result != MA_SUCCESS) {
        LOG_ERROR(
            "Failed to get sound length; %s",
            ma_result_description(result)
        );
        return false;
    }

    ma_uint64 target_frame = (ma_uint64)(progress * length_in_frames);
    result = ma_sound_seek_to_pcm_frame(&dev->sound, target_frame);

    if (result != MA_SUCCESS) {
        LOG_ERROR(
            "Failed to seek; %s",
            ma_result_description(result)
        );
        return false;
    }

    LOG_INFO("Seeked to progress: %.2f%%", progress * 100.0f);
    return true;
}

float audio_device_get_progress(audio_device_t* dev) {
    if (!dev) {
        LOG_ERROR("Couldn't get progress; audio device is NULL.");
        return -1.0f;
    }
    
    if (!dev->sound_loaded) return 0.0f;

    ma_uint64 cursor, length;
    if (ma_sound_get_cursor_in_pcm_frames(&dev->sound, &cursor) != MA_SUCCESS ||
        ma_sound_get_length_in_pcm_frames(&dev->sound, &length) != MA_SUCCESS ||
        length == 0) {
        return 0.0f;
    }

    return (float)cursor / (float)length;
}

float audio_device_get_duration_seconds(audio_device_t* dev) {
    if (!dev) {
        LOG_ERROR("Couldn't get duration in seconds; audio device is NULL.");
        return -1.0f;
    }
    
    if (!dev->sound_loaded) return 0.0f;

    ma_uint64 frames;
    if (ma_sound_get_length_in_pcm_frames(&dev->sound, &frames) != MA_SUCCESS) {
        return 0.0f;
    }

    return (float)frames / ma_engine_get_sample_rate(&dev->engine);
}

float audio_device_get_position_seconds(audio_device_t* dev) {
    if (!dev) {
        LOG_ERROR("Couldn't get position in seconds; audio device is NULL.");
    }
    
    if (!dev->sound_loaded) return 0.0f;

    ma_uint64 frames;
    if (ma_sound_get_cursor_in_pcm_frames(&dev->sound, &frames) != MA_SUCCESS) {
        return 0.0f;
    }

    return (float)frames / ma_engine_get_sample_rate(&dev->engine);
}

bool audio_device_is_playing(audio_device_t* dev) {
    if (!dev) {
        LOG_ERROR("Couldn't see if audio device is playing; audio device is NULL.");
        return false;
    }
    
    return dev->sound_loaded && ma_sound_is_playing(&dev->sound);
}

bool audio_device_is_paused(audio_device_t* dev) {
    if (!dev) {
        LOG_ERROR("Couldn't see if audio device is paused; audio device is NULL");
    }
    
    return dev->paused;
}

bool audio_device_is_finished(audio_device_t* dev) {
    if (!dev) {
        LOG_ERROR("Couldn't check if playback is finished; audio device is NULL.");
        return false;
    }
    
    if (!dev->sound_loaded) return false;
    
    // Check if sound is at the end and not playing
    return ma_sound_at_end(&dev->sound);
}
