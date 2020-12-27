// SPDX-License-Identifier: MIT
//
// Copyright (c) 2020 Antonio Niño Díaz

// Example of sound streaming with double buffer and DMA. It sets up both DMA
// channels, and sends the values of DMA A to the left speaker and DMA B to the
// right speaker. Note that the GBA only has one speaker, it is needed to use
// headphones to hear the difference.

#include <stdio.h>
#include <string.h>

#include <ugba/ugba.h>

#define CLOCKS_PER_FRAME    (280896)
#define CLOCKS_PER_SECOND   (16 * 1024 * 1024)

// Possible values: 5734, 10512, 13379, 18157, 21024, 26758, 31536, 36314, 40137
#define SAMPLE_RATE         31536

#define TICKS_PER_RELOAD    (CLOCKS_PER_SECOND / SAMPLE_RATE)

#define RELOAD_VALUE        (65536 - TICKS_PER_RELOAD)
#define BUFFER_SIZE         (CLOCKS_PER_FRAME / TICKS_PER_RELOAD)

#define DMA_A_TIMER_INDEX   0 // Timer 1 controls the transfer rate of DMA B
#define DMA_B_TIMER_INDEX   1 // Timer 1 controls the transfer rate of DMA B

static int current_buffer = 0;

ALIGNED(32) int8_t wave_a_0[BUFFER_SIZE];
ALIGNED(32) int8_t wave_a_1[BUFFER_SIZE];
ALIGNED(32) int8_t wave_b_0[BUFFER_SIZE];
ALIGNED(32) int8_t wave_b_1[BUFFER_SIZE];

void vbl_handler(void)
{
    // The buffer swap needs to be done right at the beginning of the VBL
    // interrupt handler so that the timing is always the same in each frame.

    if (current_buffer == 0)
    {
        SOUND_DMA_Stream_A(wave_a_0);
        SOUND_DMA_Stream_B(wave_b_0);
    }
    else
    {
        SOUND_DMA_Stream_A(wave_a_1);
        SOUND_DMA_Stream_B(wave_b_1);
    }

    current_buffer ^= 1;
}

void generate_waves(void)
{
    for (size_t i = 0; i < sizeof(wave_a_0); i++)
    {
        int32_t pos = (i * FP_2_PI) / sizeof(wave_a_0);
        int32_t value = (FP_Sin(10 * pos) * 127) >> 16;

        wave_a_0[i] = value;
        wave_a_1[i] = value;
    }

    for (size_t i = 0; i < sizeof(wave_b_0); i++)
    {
        int32_t pos = (i * FP_2_PI) / sizeof(wave_b_0);
        int32_t value = (FP_Sin(15 * pos) * 127) >> 16;

        wave_b_0[i] = value;
        wave_b_1[i] = value;
    }
}

int main(int argc, char *argv[])
{
    UGBA_Init(&argc, &argv);

    IRQ_SetHandler(IRQ_VBLANK, vbl_handler);
    IRQ_Enable(IRQ_VBLANK);

    REG_DISPCNT = DISPCNT_BG_MODE(0);

    CON_InitDefault();

    char debug_info[200];
    snprintf(debug_info, sizeof(debug_info), "Samples per frame: %d", BUFFER_SIZE);
    CON_Print(debug_info);

    generate_waves();

    // The sound hardware needs to be enabled to write to any other register.
    SOUND_MasterEnable(1);
    SOUND_DMA_Volume(1, 1);
    SOUND_DMA_Pan(1, 0, 0, 1); // DMA A to the left, DMA B to the right

    SOUND_DMA_TimerSetup(DMA_A_TIMER_INDEX, DMA_B_TIMER_INDEX);
    TM_TimerStart(DMA_A_TIMER_INDEX, RELOAD_VALUE, TMCNT_PRESCALER_F_DIV_1, 0);
    TM_TimerStart(DMA_B_TIMER_INDEX, RELOAD_VALUE, TMCNT_PRESCALER_F_DIV_1, 0);

    while (1)
        SWI_VBlankIntrWait();

    return 0;
}
