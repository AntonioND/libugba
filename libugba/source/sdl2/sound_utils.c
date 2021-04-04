// SPDX-License-Identifier: LGPL-3.0-only
//
// Copyright (c) 2011-2015, 2019-2021 Antonio Niño Díaz

#include <stdlib.h>
#include <string.h>

#include <SDL2/SDL.h>

#include <ugba/ugba.h>

#include "config.h"
#include "debug_utils.h"
#include "input_utils.h"
#include "sound_utils.h"

#define SDL_BUFFER_SAMPLES              (1024)

// Number of samples to have as max in the SDL sound buffer.
#define SDL_BUFFER_SAMPLES_THRESHOLD   (SDL_BUFFER_SAMPLES * 6)

static int sound_enabled = 0;

static SDL_AudioSpec obtained_spec;
static SDL_AudioStream *stream;

static void emulate_sound_callback(Uint8 *buffer, int len)
{
    int available = SDL_AudioStreamAvailable(stream);

    if (available < len)
    {
        memset(buffer, 0, len);
    }
    else
    {
        int obtained = SDL_AudioStreamGet(stream, buffer, len);
        if (obtained == -1)
        {
            Debug_Log("Failed to get converted data: %s", SDL_GetError());
        }
        else
        {
            if (obtained != len)
            {
                Debug_Log("%s: Obtained = %d, Requested = %d", __func__,
                          obtained, len);
                // Clear the rest of the buffer
                memset(&(buffer[obtained]), 0, len - obtained);
            }
        }
    }

    //Debug_Log("Available: %d/%d",
    //          SDL_AudioStreamAvailable(stream), obtained_spec.samples);
}

static void sound_callback(UNUSED void *userdata, Uint8 *buffer, int len)
{
    // Don't play audio during speedup or if it is disabled
    if ((sound_enabled == 0) ||  Input_Speedup_Enabled() ||
        GlobalConfig.sound_mute)
    {
        // Output silence
        memset(buffer, 0, len);
        // Clear all the samples sent from the GBA so that they don't just stay
        // in the buffer.
        SDL_AudioStreamClear(stream);
    }
    else
    {
        emulate_sound_callback(buffer, len);
    }
}

static void Sound_End(void)
{
     SDL_FreeAudioStream(stream);

     sound_enabled = 0;
}

void Sound_Init(void)
{
    sound_enabled = 0;

    SDL_AudioSpec desired_spec;

    desired_spec.freq = SDL_SAMPLE_RATE;
    desired_spec.format = AUDIO_S16SYS;
    desired_spec.channels = 2;
    desired_spec.samples = SDL_BUFFER_SAMPLES;
    desired_spec.callback = sound_callback;
    desired_spec.userdata = NULL;

    if (SDL_OpenAudio(&desired_spec, &obtained_spec) < 0)
    {
        Debug_Log("Couldn't open audio: %s", SDL_GetError());
        return;
    }

    Debug_Log("Audio information:\n"
              "    Freq: %d\n"
              "    Channels: %d\n"
              "    Samples: %d",
              obtained_spec.freq,
              obtained_spec.channels,
              obtained_spec.samples);

    // Input format is int16_t, dual, ~ 32 * 1024 Hz
    // Output format is whatever SDL_OpenAudio() returned
    stream = SDL_NewAudioStream(AUDIO_S16, 2, GBA_SAMPLES_60_FRAMES,
                                obtained_spec.format, obtained_spec.channels,
                                obtained_spec.freq);
    if (stream == NULL) {
        Debug_Log("Failed to create audio stream: %s", SDL_GetError());
        return;
    }

    // Cleanup everything on exit of the program
    atexit(Sound_End);

    sound_enabled = 1;

    SDL_PauseAudio(0);
}

int Sound_IsBufferOverThreshold(void)
{
    if (SDL_AudioStreamAvailable(stream) > SDL_BUFFER_SAMPLES_THRESHOLD)
        return 1;

    return 0;
}

void Sound_SendSamples(int16_t *buffer, int len)
{
    int rc = SDL_AudioStreamPut(stream, buffer, len);
    if (rc == -1)
        Debug_Log("Failed to send samples to stream: %s", SDL_GetError());
}

void Sound_Enable(void)
{
    sound_enabled = 1;
}

void Sound_Disable(void)
{
    sound_enabled = 0;
}
