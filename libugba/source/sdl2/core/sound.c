// SPDX-License-Identifier: LGPL-3.0-only
//
// Copyright (c) 2020 Antonio Niño Díaz

#include <ugba/ugba.h>

#include "dma.h"

#include "../debug_utils.h"
#include "../sound_utils.h"
#include "../wav_utils.h"

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

// DMA channels
// ============

#define GBA_CLOCKS_PER_FRAME    (280896)
#define GBA_CLOCKS_60_FRAMES    (GBA_CLOCKS_PER_FRAME * 60)

#define GBA_CLOCKS_PER_SAMPLE_60_FPS    (GBA_CLOCKS_60_FRAMES / GBA_SAMPLE_RATE)

// This isn't an exact division. Add a few extra samples in case of overflow
#define GBA_SAMPLES_PER_FRAME   ((GBA_CLOCKS_PER_FRAME / GBA_CLOCKS_PER_SAMPLE) + 10)

typedef struct {
    int8_t buffer[GBA_SAMPLES_PER_FRAME];
    int write_ptr;
    int read_ptr;

    int clocks_current_sample; // Elapsed clocks of current DMA sample
    int8_t current_sample;
    int clocks_current_buffer_index; // Elapsed clocks in sound buffer

    uint32_t sample_data; // Last 4 samples read from buffer
    int sample_count; // Count of samples read from sample_data so far
} sound_dma_info_t;

static sound_dma_info_t sound_dma[2];

// Calculate clocks per period for either timer 0 or 1
static uint32_t UGBA_TimerClocksPerPeriod(int timer)
{
    uint16_t reload_value, flags;

    if (timer)
    {
        reload_value = REG_TM1CNT_L;
        flags = REG_TM1CNT_H;
    }
    else
    {
        reload_value = REG_TM0CNT_L;
        flags = REG_TM0CNT_H;
    }

    const int prescaler_values[4] = {
        1, 64, 256, 1024
    };
    uint32_t prescaler = prescaler_values[flags & 3];
    uint32_t ticks_per_period = UINT16_MAX - reload_value;
    ticks_per_period += 1;
    uint32_t clocks_per_period = ticks_per_period * prescaler;

    return clocks_per_period;
}

// DMA A: dma_channel = 0 | DMA B: dma_channel = 1
static void Sound_FillBuffers_VBL_DMA(int dma_channel)
{
    int timer;

    if (dma_channel == 0)
        timer = REG_SOUNDCNT_H & SOUNDCNT_H_DMA_A_TIMER1;
    else
        timer = REG_SOUNDCNT_H & SOUNDCNT_H_DMA_B_TIMER1;

    uint32_t clocks_per_period = UGBA_TimerClocksPerPeriod(timer);

    sound_dma_info_t *dma = &sound_dma[dma_channel];

    for (uint32_t i = 0; i < GBA_CLOCKS_PER_FRAME; i++)
    {
        if (dma->clocks_current_sample == 0)
        {
            dma->clocks_current_sample = clocks_per_period;

            if (dma->sample_count == 0)
            {
                if (dma_channel == 0)
                    dma->sample_data = UGBA_DMA_SoundGetDataFifoA();
                else
                    dma->sample_data = UGBA_DMA_SoundGetDataFifoB();

                dma->sample_count = 4;
            }

            dma->current_sample = dma->sample_data & 0xFF;
            dma->sample_data >>= 8;
            dma->sample_count--;
        }

        dma->clocks_current_sample--;

        if (dma->clocks_current_buffer_index == 0)
        {
            dma->clocks_current_buffer_index = GBA_CLOCKS_PER_SAMPLE_60_FPS;

            dma->buffer[dma->write_ptr] = dma->current_sample;
            dma->write_ptr++;
            dma->write_ptr %= GBA_SAMPLES_PER_FRAME;

            if (dma->write_ptr == dma->read_ptr)
            {
                Debug_Log("%s(): Overflow", __func__);
                break;
            }
        }

        dma->clocks_current_buffer_index--;
    }
}

// DMA A: dma_channel = 0 | DMA B: dma_channel = 1
static int Sound_BufferIsEmpty_DMA(int dma_channel)
{
    sound_dma_info_t *dma = &sound_dma[dma_channel];

    if (dma->write_ptr == dma->read_ptr)
        return 1;

    return 0;
}

// DMA A: dma_channel = 0 | DMA B: dma_channel = 1
static int8_t Sound_GetSample_DMA(int dma_channel)
{
    sound_dma_info_t *dma = &sound_dma[dma_channel];

    int8_t sample = dma->buffer[dma->read_ptr];
    dma->read_ptr++;
    dma->read_ptr %= GBA_SAMPLES_PER_FRAME;

    return sample;
}

// Sound mixer
// ===========

#define MIXED_BUFFER_SIZE       (32 * 1024)

typedef struct {
    int16_t buffer[MIXED_BUFFER_SIZE];
    int write_ptr;
} mixed_sound_info_t;

static mixed_sound_info_t mixed;

static void Sound_Mix_Buffers_VBL(void)
{
    // DMA channels control
    // --------------------

    // Note: The reset bits in SOUNDCNT_H are ignored.

    // TODO: Read PSG master volume from SOUNDCNT_H

    int dma_a_right_enabled = REG_SOUNDCNT_H & SOUNDCNT_H_DMA_A_ENABLE_RIGHT;
    int dma_a_left_enabled = REG_SOUNDCNT_H & SOUNDCNT_H_DMA_A_ENABLE_LEFT;

    int dma_b_right_enabled = REG_SOUNDCNT_H & SOUNDCNT_H_DMA_B_ENABLE_RIGHT;
    int dma_b_left_enabled = REG_SOUNDCNT_H & SOUNDCNT_H_DMA_B_ENABLE_LEFT;

    // GBATEK: Each of the two FIFOs can span the FULL output range (+/-200h).
    //
    // Each sample is just 8 bit (+/-100h), so the volume multiplication below
    // will take the range to the final range.

    int dma_a_right_vol = (dma_a_right_enabled == 0) ? 0 :
                    (REG_SOUNDCNT_H & SOUNDCNT_H_DMA_A_VOLUME_100 ? 2 : 1);
    int dma_a_left_vol = (dma_a_left_enabled == 0) ? 0 :
                    (REG_SOUNDCNT_H & SOUNDCNT_H_DMA_A_VOLUME_100 ? 2 : 1);

    int dma_b_right_vol = (dma_b_right_enabled == 0) ? 0 :
                    (REG_SOUNDCNT_H & SOUNDCNT_H_DMA_B_VOLUME_100 ? 2 : 1);
    int dma_b_left_vol = (dma_b_left_enabled == 0) ? 0 :
                    (REG_SOUNDCNT_H & SOUNDCNT_H_DMA_B_VOLUME_100 ? 2 : 1);

    // Mix channels
    // ------------

    // Always reset pointer to the start of the buffer, as all the data is
    // always sent to SDL.
    mixed.write_ptr = 0;

    // Loop until one of the buffers is empty
    while (1)
    {
        if (Sound_BufferIsEmpty_DMA(0) || Sound_BufferIsEmpty_DMA(1))
            break;

        // TODO: PSG channels

        int16_t sample_dma_a = Sound_GetSample_DMA(0);
        int16_t sample_dma_b = Sound_GetSample_DMA(1);

        int16_t sample_left = 0;
        int16_t sample_right = 0;

        // TODO: PSG channels

        sample_left += sample_dma_a * dma_a_left_vol;
        sample_left += sample_dma_b * dma_b_left_vol;

        sample_right += sample_dma_a * dma_a_right_vol;
        sample_right += sample_dma_b * dma_b_right_vol;

        // Increase the volume a bit so that it reaches the full 16-bit range
        mixed.buffer[mixed.write_ptr++] = sample_left << 7;
        mixed.buffer[mixed.write_ptr++] = sample_right << 7;
    }
}

// General sound helpers
// =====================

static void Sound_MixBuffers_Empty(void)
{
    // Always reset pointer to the start of the buffer, as all the data is
    // always sent to SDL.
    mixed.write_ptr = 0;

    uint32_t num_samples = GBA_CLOCKS_PER_FRAME / GBA_CLOCKS_PER_SAMPLE_60_FPS;

    for (uint32_t i = 0; i < num_samples + 1; i++)
    {
        mixed.buffer[mixed.write_ptr++] = 0;
        mixed.buffer[mixed.write_ptr++] = 0;
    }
}

// Function that sends the mixed buffer to SDL
static void Sound_SendToStream(void)
{
    int samples = mixed.write_ptr;
    int size;

    if (WAV_FileIsOpen())
    {
        size = samples * sizeof(int16_t);
        WAV_FileStream(mixed.buffer, size);
    }

    // If the sound buffer is too full, drop one left and one right sample
    if (Sound_IsBufferOverThreshold())
        samples -= 2;

    size = samples * sizeof(int16_t);

    Sound_SendSamples(mixed.buffer, size);
}

// Public interfaces
// =================

void Sound_Handle_VBL(void)
{
    // Check if the sound master enable flag is disabled
    if (REG_SOUNDCNT_X & SOUNDCNT_X_MASTER_ENABLE)
    {
        // TODO: PSG channels

        Sound_FillBuffers_VBL_DMA(0);
        Sound_FillBuffers_VBL_DMA(1);

        Sound_Mix_Buffers_VBL();
    }
    else
    {
        Sound_MixBuffers_Empty();
    }

    Sound_SendToStream();
}
