// SPDX-License-Identifier: LGPL-3.0-only
//
// Copyright (c) 2020-2021 Antonio Niño Díaz

#include <string.h>

#include <ugba/ugba.h>

#include "dma.h"

#include "../config.h"
#include "../debug_utils.h"
#include "../sound_utils.h"
#include "../wav_utils.h"

// Banked wave RAM
// ===============

#define WAV_BUFFER_SIZE     ((32 * 4) / (8 * sizeof(uint16_t)))

static uint16_t channel_3_wave_ram[2 * WAV_BUFFER_SIZE];

volatile uint16_t *UGBA_MemWaveRam(void)
{
    if ((REG_SOUNDCNT_X & SOUNDCNT_X_MASTER_ENABLE) == 0)
        return &(channel_3_wave_ram[WAV_BUFFER_SIZE]);

    // Return the buffer that isn't selected for playback.
    if (SOUND3CNT_L_BANK_GET(REG_SOUND3CNT_L) == 0)
        return &(channel_3_wave_ram[WAV_BUFFER_SIZE]);
    else
        return &(channel_3_wave_ram[0]);
}

// This is only used by the debugger window
volatile uint16_t *UGBA_MemWaveRamTwoBanks(void)
{
    return &(channel_3_wave_ram[0]);
}

static int GetWaveRamSample(int index)
{
    uint16_t val = channel_3_wave_ram[index >> 2];

    switch (index & 3)
    {
        case 0:
            return (val >> 4) & 0xF;
        case 1:
            return (val >> 0) & 0xF;
        case 2:
            return (val >> 12) & 0xF;
        case 3:
            return (val >> 8) & 0xF;
        default:
            return 0;
    }
}

// PSG channels
// ============

static const int8_t GBA_SquareWave[4][32] = {
    {
        -128, -128,  127,  127, -128, -128, -128, -128,
        -128, -128, -128, -128, -128, -128, -128, -128,
        -128, -128,  127,  127, -128, -128, -128, -128,
        -128, -128, -128, -128, -128, -128, -128, -128,
    },
    {
         127,  127,  127,  127, -128, -128, -128, -128,
        -128, -128, -128, -128, -128, -128, -128, -128,
         127,  127,  127,  127, -128, -128, -128, -128,
        -128, -128, -128, -128, -128, -128, -128, -128,
    },
    {
         127,  127,  127,  127,  127,  127,  127,  127,
        -128, -128, -128, -128, -128, -128, -128, -128,
         127,  127,  127,  127,  127,  127,  127,  127,
        -128, -128, -128, -128, -128, -128, -128, -128,
    },
    {
        -128, -128, -128, -128,  127,  127,  127,  127,
         127,  127,  127,  127,  127,  127,  127,  127,
        -128, -128, -128, -128,  127,  127,  127,  127,
         127,  127,  127,  127,  127,  127,  127,  127,
    }
};

typedef struct
{
    struct // Tone & Sweep
    {
        int frequency;
        int sample_pointer;
        int frequency_steps; // Elapsed steps out of 'frequency'

        int sweep_shift;
        int sweep_decrease; // 0 = decrease, else increase
        int sweep_steps; // (1 / 128) second steps
        int sweep_elapsed_steps;

        int duty_cycle;

        int volume;

        int env_active; // If != 0, activate
        int env_increment; // Value to add to volume at every step
        int env_steps_to_change; // Steps needed to change envelope value
        int env_steps_left; // It counts from env_steps_to_change to 0.

        // Steps to end the sound (if 0, continue forever). Each step is 1/256 s
        int steps_total;
        int steps_elapsed;

        int running;

        int current_value;
    } ch1;

    struct // Tone
    {
        int frequency;
        int sample_pointer;
        int frequency_steps; // Elapsed steps out of 'frequency'

        int duty_cycle;

        int volume;

        int env_active; // If != 0, activate
        int env_increment; // Value to add to volume at every step
        int env_steps_to_change; // Steps needed to change envelope value
        int env_steps_left; // It counts from env_steps_to_change to 0.

        // Steps to end the sound (if 0, continue forever). Each step is 1/256 s
        int steps_total;
        int steps_elapsed;

        int running;

        int current_value;
    } ch2;

    struct // Wave Output
    {
        int frequency;
        int sample_pointer;
        int frequency_steps; // Elapsed steps out of 'frequency'

        int bank_selected;
        int bank_size;

        int volume;

        // Steps to end the sound (if 0, continue forever). Each step is 1/256 s
        int steps_total;
        int steps_elapsed;

        int running;

        int current_value;
    } ch3;

    struct // Noise
    {
        int counter_width;
        int frequency;
        int frequency_steps; // Elapsed steps out of 'frequency'

        int lfsr_state;

        int volume;

        int env_active; // If != 0, activate
        int env_increment; // Value to add to volume at every step
        int env_steps_to_change; // Steps needed to change envelope value
        int env_steps_left; // It counts from env_steps_to_change to 0.

        // Steps to end the sound (if 0, continue forever). Each step is 1/256 s
        int steps_total;
        int steps_elapsed;

        int running;

        int current_value;
    } ch4;

    int16_t buffer[GBA_SAMPLES_PER_FRAME * 2];
    int write_ptr;
    int read_ptr;

    int clocks_current_step; // Elapsed clocks of current step

    int clocks_current_frequency; // Elapsed clocks of current frequency step
    int clocks_current_frequency_ch4; // Same, but for channel 4

    int clocks_current_sample; // Elapsed clocks of current PSG sample
} sound_psg_info_t;

static sound_psg_info_t sound_psg;

static void UGBA_RefreshPSGState(void)
{
    // Channel 1

    uint16_t sound1cnt_l = REG_SOUND1CNT_L;
    uint16_t sound1cnt_h = REG_SOUND1CNT_H;
    uint16_t sound1cnt_x = REG_SOUND1CNT_X;

    if (sound1cnt_x & SOUND1CNT_X_RESTART)
    {
        REG_SOUND1CNT_X &= ~SOUND1CNT_X_RESTART;

        sound_psg.ch1.running = 1;

        // Length

        if (sound1cnt_x & SOUND1CNT_X_ONE_SHOT)
        {
            sound_psg.ch1.steps_total = 64 - SOUND1CNT_H_LENGTH_GET(sound1cnt_h);
            sound_psg.ch1.steps_elapsed = 0;
        }
        else
        {
            sound_psg.ch1.steps_total = 0;
        }

        // Sweep

        // Sweep steps are 1 / 128 of a second. The other steps are 1 / 256.
        sound_psg.ch1.sweep_steps = 2 * SOUND1CNT_L_SWEEP_TIME_GET(sound1cnt_l);

        if (sound_psg.ch1.sweep_steps)
        {
            sound_psg.ch1.sweep_shift = SOUND1CNT_L_SWEEP_SHIFT_GET(sound1cnt_l);
            sound_psg.ch1.sweep_decrease = sound1cnt_l & SOUND1CNT_L_SWEEP_DIR_DEC;
            sound_psg.ch1.sweep_elapsed_steps = 0;
        }

        // Envelope

        int step_time = SOUND1CNT_H_ENV_STEP_TIME_GET(sound1cnt_h);

        if (step_time == 0)
        {
            sound_psg.ch1.env_active = 0;
        }
        else
        {
            sound_psg.ch1.env_active = 1;

            if (sound1cnt_h & SOUND1CNT_H_ENV_DIR_INC)
                sound_psg.ch1.env_increment = 1;
            else
                sound_psg.ch1.env_increment = -1;

            sound_psg.ch1.env_steps_to_change = step_time;
            sound_psg.ch1.env_steps_left = step_time;
        }

        sound_psg.ch1.volume = SOUND1CNT_H_ENV_VOLUME_GET(sound1cnt_h);

        // Frequency

        sound_psg.ch1.frequency = SOUND1CNT_X_FREQUENCY_GET(sound1cnt_x);
        sound_psg.ch1.frequency_steps = 0;
        sound_psg.ch2.sample_pointer = 0;

        // Flag it as enabled

        REG_SOUNDCNT_X |= SOUNDCNT_X_PSG_1_IS_ON;
    }
    else
    {
        if (sound_psg.ch1.sweep_steps == 0)
            sound_psg.ch1.frequency = SOUND1CNT_X_FREQUENCY_GET(sound1cnt_x);
    }

    sound_psg.ch1.duty_cycle = SOUND1CNT_H_WAVE_DUTY_GET(sound1cnt_h);

    // Channel 2

    uint16_t sound2cnt_l = REG_SOUND2CNT_L;
    uint16_t sound2cnt_h = REG_SOUND2CNT_H;

    if (sound2cnt_h & SOUND2CNT_H_RESTART)
    {
        REG_SOUND2CNT_H &= ~SOUND2CNT_H_RESTART;

        sound_psg.ch2.running = 1;

        // Length

        if (sound2cnt_h & SOUND2CNT_H_ONE_SHOT)
        {
            sound_psg.ch2.steps_total = 64 - SOUND2CNT_L_LENGTH_GET(sound2cnt_l);
            sound_psg.ch2.steps_elapsed = 0;
        }
        else
        {
            sound_psg.ch2.steps_total = 0;
        }

        // Envelope

        int step_time = SOUND2CNT_L_ENV_STEP_TIME_GET(sound2cnt_l);

        if (step_time == 0)
        {
            sound_psg.ch2.env_active = 0;
        }
        else
        {
            sound_psg.ch2.env_active = 1;

            if (sound2cnt_l & SOUND2CNT_L_ENV_DIR_INC)
                sound_psg.ch2.env_increment = 1;
            else
                sound_psg.ch2.env_increment = -1;

            sound_psg.ch2.env_steps_to_change = step_time;
            sound_psg.ch2.env_steps_left = step_time;
        }

        sound_psg.ch2.volume = SOUND2CNT_L_ENV_VOLUME_GET(sound2cnt_l);

        // Frequency

        sound_psg.ch2.frequency = SOUND2CNT_H_FREQUENCY_GET(sound2cnt_h);
        sound_psg.ch2.frequency_steps = 0;
        sound_psg.ch2.sample_pointer = 0;

        // Flag it as enabled

        REG_SOUNDCNT_X |= SOUNDCNT_X_PSG_2_IS_ON;
    }

    sound_psg.ch2.duty_cycle = SOUND2CNT_L_WAVE_DUTY_GET(sound2cnt_l);

    // Channel 3

    uint16_t sound3cnt_l = REG_SOUND3CNT_L;
    uint16_t sound3cnt_h = REG_SOUND3CNT_H;
    uint16_t sound3cnt_x = REG_SOUND3CNT_X;

    if (sound3cnt_x & SOUND3CNT_X_RESTART)
    {
        REG_SOUND3CNT_X &= ~SOUND3CNT_X_RESTART;

        if ((sound3cnt_l & SOUND3CNT_L_ENABLE) == 0)
        {
            sound_psg.ch3.running = 0;

            // Flag it as disabled

            REG_SOUNDCNT_X &= ~SOUNDCNT_X_PSG_3_IS_ON;
        }
        else
        {
            sound_psg.ch3.running = 1;

            // Bank settings

            sound_psg.ch3.bank_selected = SOUND3CNT_L_BANK_GET(sound3cnt_l);
            sound_psg.ch3.sample_pointer = sound_psg.ch3.bank_selected * 32;

            if (sound3cnt_l & SOUND3CNT_L_SIZE_64)
                sound_psg.ch3.bank_size = 64;
            else
                sound_psg.ch3.bank_size = 32;

            // Length

            if (sound3cnt_x & SOUND3CNT_X_ONE_SHOT)
            {
                sound_psg.ch3.steps_total =
                            256 - SOUND3CNT_H_LENGTH_GET(sound3cnt_h);
                sound_psg.ch3.steps_elapsed = 0;
            }
            else
            {
                sound_psg.ch3.steps_total = 0;
            }

            // Volume

            int vol = sound3cnt_h & SOUND3CNT_H_VOLUME_MASK;

            if (vol == SOUND3CNT_H_VOLUME_0)
                sound_psg.ch3.volume = 0;
            else if (vol == SOUND3CNT_H_VOLUME_25)
                sound_psg.ch3.volume = 1;
            else if (vol == SOUND3CNT_H_VOLUME_50)
                sound_psg.ch3.volume = 2;
            else if (vol == SOUND3CNT_H_VOLUME_100)
                sound_psg.ch3.volume = 4;
            else // if (vol == SOUND3CNT_H_VOLUME_75)
                sound_psg.ch3.volume = 3;

            // Frequency

            sound_psg.ch3.frequency = SOUND3CNT_X_SAMPLE_RATE_GET(sound3cnt_x);
            sound_psg.ch3.frequency_steps = 0;
            sound_psg.ch3.sample_pointer = 0;

            // Flag it as enabled

            REG_SOUNDCNT_X |= SOUNDCNT_X_PSG_3_IS_ON;
        }
    }

    // Channel 4

    uint16_t sound4cnt_l = REG_SOUND4CNT_L;
    uint16_t sound4cnt_h = REG_SOUND4CNT_H;

    if (sound4cnt_h & SOUND4CNT_H_RESTART)
    {
        REG_SOUND4CNT_H &= ~SOUND4CNT_H_RESTART;

        sound_psg.ch4.running = 1;

        // Length

        if (sound4cnt_h & SOUND4CNT_H_ONE_SHOT)
        {
            sound_psg.ch4.steps_total = 64 - SOUND4CNT_L_LENGTH_GET(sound4cnt_l);
            sound_psg.ch4.steps_elapsed = 0;
        }
        else
        {
            sound_psg.ch4.steps_total = 0;
        }

        // Envelope

        int step_time = SOUND4CNT_L_ENV_STEP_TIME_GET(sound4cnt_l);

        if (step_time == 0)
        {
            sound_psg.ch4.env_active = 0;
        }
        else
        {
            sound_psg.ch4.env_active = 1;

            if (sound4cnt_l & SOUND4CNT_L_ENV_DIR_INC)
                sound_psg.ch4.env_increment = 1;
            else
                sound_psg.ch4.env_increment = -1;

            sound_psg.ch4.env_steps_to_change = step_time;
            sound_psg.ch4.env_steps_left = step_time;
        }

        sound_psg.ch4.volume = SOUND4CNT_L_ENV_VOLUME_GET(sound4cnt_l);

        // LFSR

        if (sound4cnt_h & SOUND4CNT_H_WIDTH_7_BITS)
        {
            sound_psg.ch4.counter_width = 7;
            sound_psg.ch4.lfsr_state = 0x7F;
        }
        else
        {
            sound_psg.ch4.counter_width = 15;
            sound_psg.ch4.lfsr_state = 0x7FFF;
        }

        int div_ratio = SOUND4CNT_H_DIV_RATIO_GET(sound4cnt_h);
        int freq_div = SOUND4CNT_H_FREQUENCY_GET(sound4cnt_h);

        if (freq_div > 0xD) // Invalid values
        {
            sound_psg.ch4.running = 0;
        }
        else
        {
            if (div_ratio > 0) // For div_ratio = 0 assume 0.5 instead
                div_ratio <<= 1;

            sound_psg.ch4.frequency = div_ratio * (1 << (freq_div + 1));
            sound_psg.ch4.frequency_steps = 0;

            // Flag it as enabled

            REG_SOUNDCNT_X |= SOUNDCNT_X_PSG_4_IS_ON;
        }
    }
}

static void Sound_FillBuffers_VBL_PSG(void)
{
    uint16_t soundcnt_l = REG_SOUNDCNT_L;
    uint16_t soundcnt_h = REG_SOUNDCNT_H;

    uint32_t clocks_left = GBA_CLOCKS_PER_FRAME;

    // 256 steps per second
    const int clocks_per_step = GBA_CLOCKS_PER_SECOND / 256;

    // The frequency goes through a full cycle 131072 times per second. This is
    // the same as saying 2097152 times per second for channel 3, or 2097152
    // times per second if you imagine the waves of channels 1 and 2 to be
    // formed of 16 samples.
    const int clocks_per_frequency = GBA_CLOCKS_PER_SECOND / (131072 * 16);

    const int clocks_per_frequency_ch4 = GBA_CLOCKS_PER_SECOND / (1024 * 1024);

    const int clocks_per_sample = GBA_CLOCKS_PER_SAMPLE_60_FPS;

    // Get master volume

    int psg_vol = soundcnt_h & SOUNDCNT_H_PSG_VOLUME_MASK;

    if (psg_vol == SOUNDCNT_H_PSG_VOLUME_100)
        psg_vol = 4;
    else if (psg_vol == SOUNDCNT_H_PSG_VOLUME_50)
        psg_vol = 2;
    else if (psg_vol == SOUNDCNT_H_PSG_VOLUME_25)
        psg_vol = 1;
    else
        psg_vol = 0;

    int psg_vol_left = SOUNDCNT_L_PSG_VOL_LEFT_GET(soundcnt_l) * psg_vol;
    int psg_vol_right = SOUNDCNT_L_PSG_VOL_RIGHT_GET(soundcnt_l) * psg_vol;

    // Get individual left and right volumes (master volume is included here)

    int ch1_vol_left = 0;
    int ch1_vol_right = 0;
    int ch2_vol_left = 0;
    int ch2_vol_right = 0;
    int ch3_vol_left = 0;
    int ch3_vol_right = 0;
    int ch4_vol_left = 0;
    int ch4_vol_right = 0;

    if (soundcnt_l & SOUNDCNT_L_PSG_1_ENABLE_LEFT)
        ch1_vol_left = sound_psg.ch1.volume * psg_vol_left;
    if (soundcnt_l & SOUNDCNT_L_PSG_1_ENABLE_RIGHT)
        ch1_vol_right = sound_psg.ch1.volume * psg_vol_right;

    if (soundcnt_l & SOUNDCNT_L_PSG_2_ENABLE_LEFT)
        ch2_vol_left = sound_psg.ch2.volume * psg_vol_left;
    if (soundcnt_l & SOUNDCNT_L_PSG_2_ENABLE_RIGHT)
        ch2_vol_right = sound_psg.ch2.volume * psg_vol_right;

    if (soundcnt_l & SOUNDCNT_L_PSG_3_ENABLE_LEFT)
        ch3_vol_left = sound_psg.ch3.volume * psg_vol_left;
    if (soundcnt_l & SOUNDCNT_L_PSG_3_ENABLE_RIGHT)
        ch3_vol_right = sound_psg.ch3.volume * psg_vol_right;

    if (soundcnt_l & SOUNDCNT_L_PSG_4_ENABLE_LEFT)
        ch4_vol_left = sound_psg.ch4.volume * psg_vol_left;
    if (soundcnt_l & SOUNDCNT_L_PSG_4_ENABLE_RIGHT)
        ch4_vol_right = sound_psg.ch4.volume * psg_vol_right;

    // Always reset pointer to the start of the buffer, as all the data is
    // always sent to SDL.
    sound_psg.write_ptr = 0;

    while (clocks_left > 0)
    {
        // Handle envelope, sweep and sound length of channels 1, 2, 3 and 4
        // -----------------------------------------------------------------

        sound_psg.clocks_current_step++;
        if (sound_psg.clocks_current_step == clocks_per_step)
        {
            sound_psg.clocks_current_step = 0;

            // Channel 1

            if (sound_psg.ch1.running)
            {
                // Sound length

                if (sound_psg.ch1.steps_total > 0)
                {
                    if (sound_psg.ch1.steps_total > sound_psg.ch1.steps_elapsed)
                    {
                        sound_psg.ch1.steps_elapsed++;
                    }
                    else
                    {
                        sound_psg.ch1.running = 0;
                        sound_psg.ch1.env_active = 0;

                        // Flag it as disabled
                        REG_SOUNDCNT_X &= ~SOUNDCNT_X_PSG_1_IS_ON;
                    }
                }

                // Sweep

                if (sound_psg.ch1.sweep_steps)
                {
                    sound_psg.ch1.sweep_elapsed_steps++;

                    if (sound_psg.ch1.sweep_elapsed_steps >=
                            sound_psg.ch1.sweep_steps)
                    {
                        sound_psg.ch1.sweep_elapsed_steps = 0;

                        int value = sound_psg.ch1.frequency;
                        value >>= sound_psg.ch1.sweep_shift;

                        if (sound_psg.ch1.sweep_decrease)
                        {
                            sound_psg.ch1.frequency -= value;
                            // No need to check for underflows. "value" is, at
                            // most, the same value as the frequency, so the
                            // result can only be 0 or greater than 0.
                        }
                        else
                        {
                            if (sound_psg.ch1.frequency + value <= 2047)
                            {
                                sound_psg.ch1.frequency += value;
                            }
                            else
                            {
                                sound_psg.ch1.running = 0;

                                // Flag it as disabled
                                REG_SOUNDCNT_X &= ~SOUNDCNT_X_PSG_1_IS_ON;
                            }
                        }
                    }
                }

                // Envelope

                if (sound_psg.ch1.env_active)
                {
                    sound_psg.ch1.env_steps_left--;
                    if (sound_psg.ch1.env_steps_left == 0)
                    {
                        sound_psg.ch1.env_steps_left =
                            sound_psg.ch1.env_steps_to_change;

                        sound_psg.ch1.volume += sound_psg.ch1.env_increment;

                        if (sound_psg.ch1.volume < 0)
                        {
                            sound_psg.ch1.volume = 0;
                            sound_psg.ch1.env_active = 0;
                        }
                        else if (sound_psg.ch1.volume > 7)
                        {
                            sound_psg.ch1.volume = 7;
                            sound_psg.ch1.env_active = 0;
                        }
                    }

                    if (soundcnt_l & SOUNDCNT_L_PSG_1_ENABLE_LEFT)
                        ch1_vol_left = sound_psg.ch1.volume * psg_vol_left;
                    if (soundcnt_l & SOUNDCNT_L_PSG_1_ENABLE_RIGHT)
                        ch1_vol_right = sound_psg.ch1.volume * psg_vol_right;
                }
            }

            // Channel 2

            if (sound_psg.ch2.running)
            {
                // Sound length

                if (sound_psg.ch2.steps_total > 0)
                {
                    if (sound_psg.ch2.steps_total > sound_psg.ch2.steps_elapsed)
                    {
                        sound_psg.ch2.steps_elapsed++;
                    }
                    else
                    {
                        sound_psg.ch2.running = 0;
                        sound_psg.ch2.env_active = 0;

                        // Flag it as disabled
                        REG_SOUNDCNT_X &= ~SOUNDCNT_X_PSG_2_IS_ON;
                    }
                }

                // Envelope

                if (sound_psg.ch2.env_active)
                {
                    sound_psg.ch2.env_steps_left--;
                    if (sound_psg.ch2.env_steps_left == 0)
                    {
                        sound_psg.ch2.env_steps_left =
                            sound_psg.ch2.env_steps_to_change;

                        sound_psg.ch2.volume += sound_psg.ch2.env_increment;

                        if (sound_psg.ch2.volume < 0)
                        {
                            sound_psg.ch2.volume = 0;
                            sound_psg.ch2.env_active = 0;
                        }
                        else if (sound_psg.ch2.volume > 7)
                        {
                            sound_psg.ch2.volume = 7;
                            sound_psg.ch2.env_active = 0;
                        }
                    }

                    if (soundcnt_l & SOUNDCNT_L_PSG_2_ENABLE_LEFT)
                        ch2_vol_left = sound_psg.ch2.volume * psg_vol_left;
                    if (soundcnt_l & SOUNDCNT_L_PSG_2_ENABLE_RIGHT)
                        ch2_vol_right = sound_psg.ch2.volume * psg_vol_right;
                }
            }

            // Channel 3

            if (sound_psg.ch3.running)
            {
                // Sound length

                if (sound_psg.ch3.steps_total > 0)
                {
                    if (sound_psg.ch3.steps_total > sound_psg.ch3.steps_elapsed)
                    {
                        sound_psg.ch3.steps_elapsed++;
                    }
                    else
                    {
                        sound_psg.ch3.running = 0;

                        // Flag it as disabled
                        REG_SOUNDCNT_X &= ~SOUNDCNT_X_PSG_3_IS_ON;
                    }
                }
            }

            // Channel 4

            if (sound_psg.ch4.running)
            {
                // Sound length

                if (sound_psg.ch4.steps_total > 0)
                {
                    if (sound_psg.ch4.steps_total > sound_psg.ch4.steps_elapsed)
                    {
                        sound_psg.ch4.steps_elapsed++;
                    }
                    else
                    {
                        sound_psg.ch4.running = 0;
                        sound_psg.ch4.env_active = 0;

                        // Flag it as disabled
                        REG_SOUNDCNT_X &= ~SOUNDCNT_X_PSG_4_IS_ON;
                    }
                }

                // Envelope

                if (sound_psg.ch4.env_active)
                {
                    sound_psg.ch4.env_steps_left--;
                    if (sound_psg.ch4.env_steps_left == 0)
                    {
                        sound_psg.ch4.env_steps_left =
                            sound_psg.ch4.env_steps_to_change;

                        sound_psg.ch4.volume += sound_psg.ch4.env_increment;

                        if (sound_psg.ch4.volume < 0)
                        {
                            sound_psg.ch4.volume = 0;
                            sound_psg.ch4.env_active = 0;
                        }
                        else if (sound_psg.ch4.volume > 7)
                        {
                            sound_psg.ch4.volume = 7;
                            sound_psg.ch4.env_active = 0;
                        }
                    }

                    if (soundcnt_l & SOUNDCNT_L_PSG_4_ENABLE_LEFT)
                        ch4_vol_left = sound_psg.ch4.volume * psg_vol_left;
                    if (soundcnt_l & SOUNDCNT_L_PSG_4_ENABLE_RIGHT)
                        ch4_vol_right = sound_psg.ch4.volume * psg_vol_right;
                }
            }
        }

        // Handle waveform changes of channels 1, 2 and 3
        // ----------------------------------------------

        sound_psg.clocks_current_frequency++;
        if (sound_psg.clocks_current_frequency == clocks_per_frequency)
        {
            sound_psg.clocks_current_frequency = 0;

            // Channel 1

            if (sound_psg.ch1.running)
            {
                sound_psg.ch1.frequency_steps++;

                if (sound_psg.ch1.frequency_steps >= 2048)
                {
                    sound_psg.ch1.frequency_steps = sound_psg.ch1.frequency;

                    int duty = sound_psg.ch1.duty_cycle;
                    int pointer = sound_psg.ch1.sample_pointer;

                    sound_psg.ch1.current_value = GBA_SquareWave[duty][pointer];

                    sound_psg.ch1.sample_pointer++;
                    sound_psg.ch1.sample_pointer &= 31;
                }
            }

            // Channel 2

            if (sound_psg.ch2.running)
            {
                sound_psg.ch2.frequency_steps++;

                if (sound_psg.ch2.frequency_steps >= 2048)
                {
                    sound_psg.ch2.frequency_steps = sound_psg.ch2.frequency;

                    int duty = sound_psg.ch2.duty_cycle;
                    int pointer = sound_psg.ch2.sample_pointer;

                    sound_psg.ch2.current_value = GBA_SquareWave[duty][pointer];

                    sound_psg.ch2.sample_pointer++;
                    sound_psg.ch2.sample_pointer &= 31;
                }
            }

            // Channel 3

            if (sound_psg.ch3.running)
            {
                sound_psg.ch3.frequency_steps++;

                if (sound_psg.ch3.frequency_steps >= 2048)
                {
                    sound_psg.ch3.frequency_steps = sound_psg.ch3.frequency;

                    int pointer = sound_psg.ch3.sample_pointer;
                    int sample = (GetWaveRamSample(pointer) - 7) << 5;

                    sound_psg.ch3.current_value = sample;

                    if (sound_psg.ch3.bank_size == 64)
                    {
                        sound_psg.ch3.sample_pointer++;
                        sound_psg.ch3.sample_pointer &= 63;
                    }
                    else // if (sound_psg.ch3.bank_size == 32)
                    {
                        sound_psg.ch3.sample_pointer++;
                        sound_psg.ch3.sample_pointer &= 31;

                        sound_psg.ch3.sample_pointer |=
                            sound_psg.ch3.bank_selected << 5;
                    }
                }
            }
        }

        // Handle waveform changes of channel 4
        // ------------------------------------

        sound_psg.clocks_current_frequency_ch4++;
        if (sound_psg.clocks_current_frequency_ch4 == clocks_per_frequency_ch4)
        {
            sound_psg.clocks_current_frequency_ch4 = 0;

            if (sound_psg.ch4.running)
            {
                sound_psg.ch4.frequency_steps++;

                if (sound_psg.ch4.frequency_steps >= sound_psg.ch4.frequency)
                {
                    sound_psg.ch4.frequency_steps = 0;

                    if (sound_psg.ch4.counter_width == 7)
                    {
                        if (sound_psg.ch4.lfsr_state & 1)
                        {
                            sound_psg.ch4.lfsr_state >>= 1;
                            sound_psg.ch4.lfsr_state ^= 0x60;
                            sound_psg.ch4.current_value = 127;
                        }
                        else
                        {
                            sound_psg.ch4.lfsr_state >>= 1;
                            sound_psg.ch4.current_value = -128;
                        }
                    }
                    else if (sound_psg.ch4.counter_width == 15)
                    {
                        if (sound_psg.ch4.lfsr_state & 1)
                        {
                            sound_psg.ch4.lfsr_state >>= 1;
                            sound_psg.ch4.lfsr_state ^= 0x6000;
                            sound_psg.ch4.current_value = 127;
                        }
                        else
                        {
                            sound_psg.ch4.lfsr_state >>= 1;
                            sound_psg.ch4.current_value = -128;
                        }
                    }
                }
            }
        }

        // Generate sample combining the 4 channels
        // ----------------------------------------

        // GBATEK: Each of the four PSGs can span one QUARTER of the output
        // range (+/-80h).

        sound_psg.clocks_current_sample++;
        if (sound_psg.clocks_current_sample == clocks_per_sample)
        {
            sound_psg.clocks_current_sample = 0;

            int sound_left = 0;
            int sound_right = 0;

            if ((sound_psg.ch1.running) && (GlobalConfig.channel_flags & (1 << 0)))
            {
                sound_left += sound_psg.ch1.current_value * ch1_vol_left;
                sound_right += sound_psg.ch1.current_value * ch1_vol_right;
            }

            if ((sound_psg.ch2.running) && (GlobalConfig.channel_flags & (1 << 1)))
            {
                sound_left += sound_psg.ch2.current_value * ch2_vol_left;
                sound_right += sound_psg.ch2.current_value * ch2_vol_right;
            }

            if ((sound_psg.ch3.running) && (GlobalConfig.channel_flags & (1 << 2)))
            {
                sound_left += sound_psg.ch3.current_value * ch3_vol_left;
                sound_right += sound_psg.ch3.current_value * ch3_vol_right;
            }

            if ((sound_psg.ch4.running) && (GlobalConfig.channel_flags & (1 << 3)))
            {
                sound_left += sound_psg.ch4.current_value * ch4_vol_left;
                sound_right += sound_psg.ch4.current_value * ch4_vol_right;
            }

            sound_left >>= 10;
            sound_right >>= 10;

            sound_psg.buffer[sound_psg.write_ptr++] = sound_left;
            sound_psg.buffer[sound_psg.write_ptr++] = sound_right;
        }

        clocks_left--;
    }

    // Hack: Fill buffer with the last value if there are empty samples.
    while (sound_psg.write_ptr < (GBA_SAMPLES_PER_FRAME * 2))
    {
        sound_psg.buffer[sound_psg.write_ptr + 0] =
                sound_psg.buffer[sound_psg.write_ptr - 2];
        sound_psg.buffer[sound_psg.write_ptr + 1] =
                sound_psg.buffer[sound_psg.write_ptr - 1];
        sound_psg.write_ptr += 2;
    }
}

// DMA channels
// ============

typedef struct {
    int8_t buffer[GBA_SAMPLES_PER_FRAME];
    int write_ptr;

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

    if ((flags & TMCNT_START) == 0)
        return 0;

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
    sound_dma_info_t *dma = &sound_dma[dma_channel];

    // If this channel is disabled in the global configuration,skip it.
    if ((GlobalConfig.channel_flags & (1 << (dma_channel + 4))) == 0)
    {
        memset(dma->buffer, 0, sizeof(dma->buffer));
        return;
    }

    int timer;

    if (dma_channel == 0)
        timer = REG_SOUNDCNT_H & SOUNDCNT_H_DMA_A_TIMER1;
    else
        timer = REG_SOUNDCNT_H & SOUNDCNT_H_DMA_B_TIMER1;

    uint32_t clocks_per_period = UGBA_TimerClocksPerPeriod(timer);

    if (clocks_per_period == 0)
    {
        memset(dma->buffer, 0, sizeof(dma->buffer));
        return;
    }

    // Always reset pointer to the start of the buffer, as all the data is
    // always sent to SDL.
    dma->write_ptr = 0;

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

            dma->buffer[dma->write_ptr++] = dma->current_sample;
        }

        dma->clocks_current_buffer_index--;
    }

    // Hack: Fill buffer with the last value if there are empty samples.
    while (sound_psg.write_ptr < GBA_SAMPLES_PER_FRAME)
    {
        dma->buffer[dma->write_ptr] = dma->buffer[dma->write_ptr - 1];
        dma->write_ptr++;
    }
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

    // GBATEK: The BIAS value is added to that signed value. With default BIAS
    // (200h), the possible range becomes -400h..+800h
    int16_t bias = SOUNDBIAS_BIAS_LEVEL_GET(REG_SOUNDBIAS) << 1;

    for (int read_ptr = 0; read_ptr < GBA_SAMPLES_PER_FRAME; read_ptr++)
    {
        int16_t sample_left = bias;
        int16_t sample_right = bias;

        sample_left += sound_psg.buffer[read_ptr * 2 + 0];
        sample_right += sound_psg.buffer[read_ptr * 2 + 1];

        {
            int16_t sample_dma_a = sound_dma[0].buffer[read_ptr];
            sample_left += sample_dma_a * dma_a_left_vol;
            sample_right += sample_dma_a * dma_a_right_vol;
        }

        {
            int16_t sample_dma_b = sound_dma[1].buffer[read_ptr];
            sample_left += sample_dma_b * dma_b_left_vol;
            sample_right += sample_dma_b * dma_b_right_vol;
        }

        // GBATEK: Values that exceed the unsigned 10bit output range of 0..3FFh
        // are clipped to MinMax(0,3FFh).

        if (sample_left > 0x3FF)
            sample_left = 0x3FF;
        if (sample_left < 0)
            sample_left = 0;

        if (sample_right > 0x3FF)
            sample_right = 0x3FF;
        if (sample_right < 0)
            sample_right = 0;

        // Turn values into signed values and increase the volume a bit so that
        // it reaches the full 16-bit range
        sample_left = (sample_left - 0x200) << 6;
        sample_right = (sample_right - 0x200) << 6;

        mixed.buffer[mixed.write_ptr++] =
                (sample_left * GlobalConfig.volume) / 100;
        mixed.buffer[mixed.write_ptr++] =
                (sample_right * GlobalConfig.volume) / 100;
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
        UGBA_RefreshPSGState();

        Sound_FillBuffers_VBL_PSG();
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

int Sound_PSG_GetChannelVolume(int channel)
{
    switch (channel)
    {
        case 1:
            return sound_psg.ch1.volume;
        case 2:
            return sound_psg.ch2.volume;
        case 3:
            return sound_psg.ch3.volume;
        case 4:
            return sound_psg.ch4.volume;
        default:
            break;
    }

    return 0;
}

void Sound_Initialize(void)
{
    // Fill bank 1 with 0xFF00 and bank 0 with 0x0000
    size_t size = sizeof(channel_3_wave_ram) / sizeof(uint16_t);

    for (size_t i = 0; i < size; i++)
        channel_3_wave_ram[i] = 0x0000;

    // Reset DMA channels FIFO

    sound_dma[0].current_sample = 0;
    sound_dma[0].sample_data = 0;
    sound_dma[0].sample_count = 0;

    sound_dma[1].current_sample = 0;
    sound_dma[1].sample_data = 0;
    sound_dma[1].sample_count = 0;

    REG_SOUNDBIAS = SOUNDBIAS_BIAS_LEVEL_SET(0x100);
}
