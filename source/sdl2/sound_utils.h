// SPDX-License-Identifier: LGPL-3.0-only
//
// Copyright (c) 2011-2015, 2019-2021 Antonio Niño Díaz

#ifndef SDL2_SOUND_UTILS_H__
#define SDL2_SOUND_UTILS_H__

#include <stdint.h>

#define SDL_SAMPLE_RATE     (44100) // Samples per second

// The simulation always runs at 60 FPS, but the GBA runs at a slightly
// different rate.
//
//     GBA clocks per frame = 280896
//
//     GBA clocks per second = 16 * 1024 * 1024 = 16777216
//     Actual clocks in 60 frames = 280896 * 60 = 16853760
//
// The difference between the two values means that the actual sample rate can't
// be used, we run exactly 60 FPS but the GBA runs at aprox 59.73 FPS.  It is
// needed to use the sample rate as if the GBA was running at exactly 60 FPS.
// The difference shouldn't be noticeable by a person.

#define GBA_CLOCKS_PER_SECOND   (16 * 1024 * 1024) // Clocks per second
#define GBA_SAMPLE_RATE         (32 * 1024) // Samples per second
#define GBA_CLOCKS_PER_SAMPLE   (GBA_CLOCKS_PER_SECOND / GBA_SAMPLE_RATE)

// Values used to stream on SDL2

#define GBA_CLOCKS_PER_FRAME    (280896)
#define GBA_CLOCKS_60_FRAMES    (GBA_CLOCKS_PER_FRAME * 60)

#define GBA_CLOCKS_PER_SAMPLE_60_FPS    (GBA_CLOCKS_60_FRAMES / GBA_SAMPLE_RATE)

// This isn't an exact division
#define GBA_SAMPLES_PER_FRAME   ((GBA_CLOCKS_PER_FRAME / GBA_CLOCKS_PER_SAMPLE) - 1)

#define GBA_SAMPLES_60_FRAMES   (GBA_SAMPLES_PER_FRAME * 60)

void Sound_Init(void);

void Sound_SendSamples(int16_t *buffer, int len);

int Sound_IsBufferOverThreshold(void);

void Sound_Enable(void);
void Sound_Disable(void);

#endif // SDL2_SOUND_UTILS_H__
