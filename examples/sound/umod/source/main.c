// SPDX-License-Identifier: MIT
//
// Copyright (c) 2021 Antonio Niño Díaz

// Example of sound streaming with double buffer and DMA. It sets up both DMA
// channels, and sends the values of DMA A to the left speaker and DMA B to the
// right speaker. Note that the GBA only has one speaker, it is needed to use
// headphones to hear the difference.

#include <stdio.h>
#include <string.h>

#include <ugba/ugba.h>

#include <umod/umod.h>

#include "umod_pack_bin.h"
#include "umod_pack_info.h"

// Buffer size needs to be a multiple of 16 (the amount of bytes copied to the
// FIFO whenever it gets data from DMA).
//
// Timer Reload = Clocks per frame / buffer size = 280896 / buffer size
// It needs to be an exact division.
//
// Sample rate = Buffer size * FPS = Buffer size * CPU Freq / Clocks per frame
// Sample rate = (Buffer size * 16 * 1024 * 1024) / 280896
//
// Valid combinations:
//
// Sample Rate | Timer Reload | Buffer Size
// ------------+--------------+------------
// 10512.04    | 1596         | 176
// 13378.96    | 1254         | 224
// 18157.16    | 924          | 304
// 21024.08    | 798          | 352
// 26757.92    | 627          | 448
// 31536.12    | 532          | 528
// 36314.32    | 462          | 608
// 40136.88    | 418          | 672

#define SAMPLE_RATE         31536

#define TICKS_PER_RELOAD    532
#define RELOAD_VALUE        (65536 - TICKS_PER_RELOAD)

#define BUFFER_SIZE         528

#define DMA_TIMER_INDEX     0 // Timer 0 controls the transfer rate of DMA A/B

static int current_dma_buffer = 0;

ALIGNED(32) int8_t wave_a[BUFFER_SIZE * 2];
ALIGNED(32) int8_t wave_b[BUFFER_SIZE * 2];

IWRAM_CODE ARM_CODE void vbl_handler(void)
{
    // The buffer swap needs to be done right at the beginning of the VBL
    // interrupt handler so that the timing is always the same in each frame.

    if (current_dma_buffer == 0)
        SOUND_DMA_Retrigger_AB();
}

int main(int argc, char *argv[])
{
    UGBA_Init(&argc, &argv);

    IRQ_SetHandler(IRQ_VBLANK, vbl_handler);
    IRQ_Enable(IRQ_VBLANK);

    DISP_ModeSet(0);

    CON_InitDefault();

    char debug_info[200];
    snprintf(debug_info, sizeof(debug_info),
             "Sample rate: %d\n"
             "Samples per frame: %d\n"
             "Ticks per reload: %d\n",
             SAMPLE_RATE,
             BUFFER_SIZE,
             TICKS_PER_RELOAD);
    CON_Print(debug_info);

    UMOD_Init(SAMPLE_RATE);
    UMOD_LoadPack(umod_pack_bin);

    // The sound hardware needs to be enabled to write to any other register.
    SOUND_MasterEnable(1);
    SOUND_DMA_Volume(100, 100);
    SOUND_DMA_Pan(1, 0, 0, 1); // DMA A to the left, DMA B to the right

    SOUND_DMA_TimerSetup(DMA_TIMER_INDEX, DMA_TIMER_INDEX);
    TM_TimerStop(DMA_TIMER_INDEX);
    TM_TimerStart(DMA_TIMER_INDEX, RELOAD_VALUE, 1, 0);

    SOUND_DMA_Setup_AB(wave_a, wave_b);

    UMOD_Song_Play(SONG_KAOS_OCH_DEKADENS_MOD);

    umod_handle helicopter_handle = UMOD_HANDLE_INVALID;

    while (1)
    {
        if (current_dma_buffer == 1)
            UMOD_Mix(wave_a, wave_b, BUFFER_SIZE);
        else
            UMOD_Mix(&wave_a[BUFFER_SIZE], &wave_b[BUFFER_SIZE], BUFFER_SIZE);

        current_dma_buffer ^= 1;

        KEYS_Update();

        uint16_t keys_pressed = KEYS_Pressed();

        if (keys_pressed & KEY_A)
            UMOD_SFX_Play(SFX_LASER2_1_WAV, UMOD_LOOP_DEFAULT);

        if (keys_pressed & KEY_B)
        {
            if (helicopter_handle == 0)
            {
                helicopter_handle = UMOD_SFX_Play(SFX_HELICOPTER_WAV, UMOD_LOOP_ENABLE);
            }
            else
            {
                UMOD_SFX_Stop(helicopter_handle);
                helicopter_handle = 0;
            }
        }

        if (keys_pressed & KEY_L)
            UMOD_Song_SetMasterVolume(64);
        if (keys_pressed & KEY_R)
            UMOD_Song_SetMasterVolume(256);

        if (keys_pressed & KEY_DOWN)
            UMOD_SFX_SetMasterVolume(64);
        if (keys_pressed & KEY_UP)
            UMOD_SFX_SetMasterVolume(256);

        if (keys_pressed & KEY_LEFT)
            UMOD_Song_Stop();
        if (keys_pressed & KEY_RIGHT)
            UMOD_Song_Play(SONG_KAOS_OCH_DEKADENS_MOD);

        if (keys_pressed & KEY_START)
            UMOD_Song_Pause();
        if (keys_pressed & KEY_SELECT)
            UMOD_Song_Resume();

        SWI_VBlankIntrWait();
    }

    return 0;
}
