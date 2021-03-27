// SPDX-License-Identifier: MIT
//
// Copyright (c) 2020-2021 Antonio Niño Díaz

// Example that combines PSG and DMA sound channels.

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

#define DMA_A_TIMER_INDEX   0 // Timer 0 controls the transfer rate of DMA A
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

void load_wave_ram(void)
{
    // Bank 0

    REG_SOUND3CNT_L = SOUND3CNT_L_BANK_SET(1);
    for (int i = 0; i < 8; i++)
        REG_WAVE_RAM[i] = 0xFF00; // 0x9F05

    // Bank 1

    REG_SOUND3CNT_L = SOUND3CNT_L_BANK_SET(0);
    for (int i = 0; i < 8; i++)
        REG_WAVE_RAM[i] = 0xF0F0;
}

void start_channels_psg(void)
{
    load_wave_ram();

    REG_SOUND1CNT_L = SOUND1CNT_L_SWEEP_TIME_SET(0);

    REG_SOUND1CNT_H = SOUND1CNT_H_WAVE_DUTY_50
                    | SOUND1CNT_H_ENV_VOLUME_SET(7)
                    | SOUND1CNT_H_ENV_STEP_TIME_SET(0);

    REG_SOUND1CNT_X = SOUND1CNT_X_FREQUENCY_SET(1900)
                    | SOUND1CNT_X_RESTART;

    REG_SOUND2CNT_L = SOUND2CNT_L_WAVE_DUTY_50
                    | SOUND2CNT_L_ENV_VOLUME_SET(3)
                    | SOUND2CNT_L_ENV_STEP_TIME_SET(1)
                    | SOUND2CNT_L_ENV_DIR_INC;

    REG_SOUND2CNT_H = SOUND2CNT_H_FREQUENCY_SET(1000)
                    | SOUND2CNT_H_RESTART;

    REG_SOUND3CNT_L = SOUND3CNT_L_SIZE_32
                    | SOUND3CNT_L_BANK_SET(0)
                    | SOUND3CNT_L_ENABLE;

    REG_SOUND3CNT_H = SOUND3CNT_H_VOLUME_100;

    REG_SOUND3CNT_X = SOUND3CNT_X_SAMPLE_RATE_SET(200)
                    | SOUND3CNT_X_RESTART;

    REG_SOUND4CNT_L = SOUND4CNT_L_ENV_VOLUME_SET(7)
                    | SOUND4CNT_L_ENV_STEP_TIME_SET(0);

    REG_SOUND4CNT_H = SOUND4CNT_H_DIV_RATIO_SET(1)
                    | SOUND4CNT_H_WIDTH_15_BITS
                    | SOUND4CNT_H_FREQUENCY_SET(4)
                    | SOUND4CNT_H_RESTART;
}

void start_channels_dma(void)
{
    generate_waves();

    IRQ_SetHandler(IRQ_VBLANK, vbl_handler);

    SOUND_DMA_TimerSetup(DMA_A_TIMER_INDEX, DMA_B_TIMER_INDEX);
    TM_TimerStart(DMA_A_TIMER_INDEX, RELOAD_VALUE, 1, 0);
    TM_TimerStart(DMA_B_TIMER_INDEX, RELOAD_VALUE, 1, 0);
}

void delay_frames(int frames)
{
    while (frames--)
        SWI_VBlankIntrWait();
}

int main(int argc, char *argv[])
{
    UGBA_Init(&argc, &argv);

    IRQ_Enable(IRQ_VBLANK);

    DISP_ModeSet(0);

    CON_InitDefault();

    char str[200];
    snprintf(str, sizeof(str), "Samples per frame: %d\n", BUFFER_SIZE);
    CON_Print(str);

    // The sound hardware needs to be enabled to write to any other register.
    SOUND_MasterEnable(1);

    SOUND_DMA_Volume(100, 100);
    SOUND_DMA_Pan(1, 0, 0, 1); // DMA A to the left, DMA B to the right

    SOUND_PSG_MasterVolume(100);
    SOUND_PSG_Volume(7, 7);
    SOUND_PSG_Pan(0, 0, 0, 0, 0, 0, 0, 0);

    SWI_VBlankIntrWait();

    start_channels_dma();
    start_channels_psg();

    CON_Print("-\n");

    delay_frames(60);

    CON_Print("1\n");
    SOUND_PSG_Pan(1, 1, 0, 0, 0, 0, 0, 0);

    delay_frames(60);

    CON_Print("2\n");
    SOUND_PSG_Pan(0, 0, 1, 1, 0, 0, 0, 0);

    delay_frames(60);

    CON_Print("3\n");
    SOUND_PSG_Pan(0, 0, 0, 0, 1, 1, 0, 0);

    delay_frames(60);

    CON_Print("4\n");
    SOUND_PSG_Pan(0, 0, 0, 0, 0, 0, 1, 1);

    delay_frames(60);

    CON_Print("2 + 4\n");
    SOUND_PSG_Pan(0, 0, 1, 1, 0, 0, 1, 1);

    delay_frames(60);

    CON_Print("-\n");
    SOUND_PSG_Pan(0, 0, 0, 0, 0, 0, 0, 0);

    delay_frames(60);

    SOUND_MasterEnable(0);

    while (1)
        SWI_VBlankIntrWait();

    return 0;
}
