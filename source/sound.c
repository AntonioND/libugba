// SPDX-License-Identifier: MIT
//
// Copyright (c) 2020-2021 Antonio Niño Díaz

#include <ugba/ugba.h>

void SOUND_MasterEnable(int enable)
{
    if (enable)
        REG_SOUNDCNT_X = SOUNDCNT_X_MASTER_ENABLE;
    else
        REG_SOUNDCNT_X = SOUNDCNT_X_MASTER_DISABLE;
}

void SOUND_DMA_Volume(int dma_a_max, int dma_b_max)
{
    uint16_t mask = SOUNDCNT_H_DMA_A_VOLUME_100 | SOUNDCNT_H_DMA_B_VOLUME_100;
    uint16_t value = REG_SOUNDCNT_H & ~mask;

    if (dma_a_max == 100)
        value |= SOUNDCNT_H_DMA_A_VOLUME_100;

    if (dma_b_max == 100)
        value |= SOUNDCNT_H_DMA_B_VOLUME_100;

    REG_SOUNDCNT_H = value;
}

void SOUND_PSG_MasterVolume(int volume)
{
    uint16_t mask = SOUNDCNT_H_PSG_VOLUME_MASK;
    uint16_t value = REG_SOUNDCNT_H & ~mask;

    if (volume == 100)
        value |= SOUNDCNT_H_PSG_VOLUME_100;
    else if (volume == 50)
        value |= SOUNDCNT_H_PSG_VOLUME_50;
    else
        value |= SOUNDCNT_H_PSG_VOLUME_25;

    REG_SOUNDCNT_H = value;
}

void SOUND_PSG_Volume(int volume_left, int volume_right)
{
    uint16_t mask = SOUNDCNT_L_PSG_VOL_RIGHT_SET(7)
                  | SOUNDCNT_L_PSG_VOL_LEFT_SET(7);
    uint16_t value = REG_SOUNDCNT_L & ~mask;

    value |= SOUNDCNT_L_PSG_VOL_LEFT_SET(volume_left)
           | SOUNDCNT_L_PSG_VOL_RIGHT_SET(volume_right);

    REG_SOUNDCNT_L = value;
}

void SOUND_DMA_Pan(int dma_a_left, int dma_a_right,
                   int dma_b_left, int dma_b_right)
{
    uint16_t mask =
        SOUNDCNT_H_DMA_A_ENABLE_RIGHT | SOUNDCNT_H_DMA_A_ENABLE_LEFT |
        SOUNDCNT_H_DMA_B_ENABLE_RIGHT | SOUNDCNT_H_DMA_B_ENABLE_LEFT;

    uint16_t value = REG_SOUNDCNT_H & ~mask;

    if (dma_a_left)
        value |= SOUNDCNT_H_DMA_A_ENABLE_LEFT;
    if (dma_a_right)
        value |= SOUNDCNT_H_DMA_A_ENABLE_RIGHT;

    if (dma_b_left)
        value |= SOUNDCNT_H_DMA_B_ENABLE_LEFT;
    if (dma_b_right)
        value |= SOUNDCNT_H_DMA_B_ENABLE_RIGHT;

    REG_SOUNDCNT_H = value;
}

void SOUND_PSG_Pan(int left_1, int right_1, int left_2, int right_2,
                   int left_3, int right_3, int left_4, int right_4)
{
    uint16_t mask = SOUNDCNT_L_PSG_1_ENABLE_RIGHT
                  | SOUNDCNT_L_PSG_2_ENABLE_RIGHT
                  | SOUNDCNT_L_PSG_3_ENABLE_RIGHT
                  | SOUNDCNT_L_PSG_4_ENABLE_RIGHT
                  | SOUNDCNT_L_PSG_1_ENABLE_LEFT
                  | SOUNDCNT_L_PSG_2_ENABLE_LEFT
                  | SOUNDCNT_L_PSG_3_ENABLE_LEFT
                  | SOUNDCNT_L_PSG_4_ENABLE_LEFT;

    uint16_t value = REG_SOUNDCNT_L & ~mask;

    if (left_1)
        value |= SOUNDCNT_L_PSG_1_ENABLE_LEFT;
    if (right_1)
        value |= SOUNDCNT_L_PSG_1_ENABLE_RIGHT;
    if (left_2)
        value |= SOUNDCNT_L_PSG_2_ENABLE_LEFT;
    if (right_2)
        value |= SOUNDCNT_L_PSG_2_ENABLE_RIGHT;
    if (left_3)
        value |= SOUNDCNT_L_PSG_3_ENABLE_LEFT;
    if (right_3)
        value |= SOUNDCNT_L_PSG_3_ENABLE_RIGHT;
    if (left_4)
        value |= SOUNDCNT_L_PSG_4_ENABLE_LEFT;
    if (right_4)
        value |= SOUNDCNT_L_PSG_4_ENABLE_RIGHT;

    REG_SOUNDCNT_L = value;
}

void SOUND_DMA_Stream_A(const void *source)
{
    uintptr_t dest = (uintptr_t)REG_FIFO_A;

    uint16_t flags = DMACNT_DST_FIXED | DMACNT_SRC_INCREMENT |
                     DMACNT_REPEAT_ON | DMACNT_TRANSFER_32_BITS |
                     DMACNT_START_SPECIAL | DMACNT_DMA_ENABLE;

    REG_DMA1SAD = (uintptr_t)source;
    REG_DMA1DAD = dest;
    REG_DMA1CNT_L = 0;

    // Stop it first
    REG_DMA1CNT_H = 0;
    UGBA_RegisterUpdatedOffset(OFFSET_DMA1CNT_H);

    REG_DMA1CNT_H = flags;
    UGBA_RegisterUpdatedOffset(OFFSET_DMA1CNT_H);
}

void SOUND_DMA_Stream_B(const void *source)
{
    uintptr_t dest = (uintptr_t)REG_FIFO_B;

    uint16_t flags = DMACNT_DST_FIXED | DMACNT_SRC_INCREMENT |
                     DMACNT_REPEAT_ON | DMACNT_TRANSFER_32_BITS |
                     DMACNT_START_SPECIAL | DMACNT_DMA_ENABLE;

    REG_DMA2SAD = (uintptr_t)source;
    REG_DMA2DAD = dest;
    REG_DMA2CNT_L = 0;

    // Stop it first
    REG_DMA2CNT_H = 0;
    UGBA_RegisterUpdatedOffset(OFFSET_DMA2CNT_H);

    REG_DMA2CNT_H = flags;
    UGBA_RegisterUpdatedOffset(OFFSET_DMA2CNT_H);
}

void SOUND_DMA_Setup_AB(const void *source_a, const void *source_b)
{
    // Stop the channels first
    REG_DMA1CNT_H = 0;
    UGBA_RegisterUpdatedOffset(OFFSET_DMA1CNT_H);

    REG_DMA2CNT_H = 0;
    UGBA_RegisterUpdatedOffset(OFFSET_DMA2CNT_H);

    REG_DMA1SAD = (uintptr_t)source_a;
    REG_DMA1DAD = (uintptr_t)REG_FIFO_A;
    REG_DMA1CNT_L = 0;

    REG_DMA2SAD = (uintptr_t)source_b;
    REG_DMA2DAD = (uintptr_t)REG_FIFO_B;
    REG_DMA2CNT_L = 0;
}

void SOUND_DMA_Retrigger_AB(void)
{
    uint16_t flags = DMACNT_DST_FIXED | DMACNT_SRC_INCREMENT |
                     DMACNT_REPEAT_ON | DMACNT_TRANSFER_32_BITS |
                     DMACNT_START_SPECIAL | DMACNT_DMA_ENABLE;

    // Stop the channels first

    REG_DMA1CNT_H = 0;
    UGBA_RegisterUpdatedOffset(OFFSET_DMA1CNT_H);

    REG_DMA2CNT_H = 0;
    UGBA_RegisterUpdatedOffset(OFFSET_DMA2CNT_H);

    // Enable them again

    REG_DMA1CNT_H = flags;
    UGBA_RegisterUpdatedOffset(OFFSET_DMA1CNT_H);

    REG_DMA2CNT_H = flags;
    UGBA_RegisterUpdatedOffset(OFFSET_DMA2CNT_H);
}

void SOUND_DMA_TimerSetup(int dma_a_timer, int dma_b_timer)
{
    // Keep both channels in reset state while they are set up

    uint16_t mask_reset = SOUNDCNT_H_DMA_A_RESET | SOUNDCNT_H_DMA_B_RESET;

    REG_SOUNDCNT_H |= mask_reset;

    uint16_t value = REG_SOUNDCNT_H;

    // Set up the timers

    uint16_t mask_timer = SOUNDCNT_H_DMA_A_TIMER1 | SOUNDCNT_H_DMA_B_TIMER1;

    value &= ~mask_timer;

    if (dma_a_timer)
        value |= SOUNDCNT_H_DMA_A_TIMER1;
    else
        value |= SOUNDCNT_H_DMA_A_TIMER0;

    if (dma_b_timer)
        value |= SOUNDCNT_H_DMA_B_TIMER1;
    else
        value |= SOUNDCNT_H_DMA_B_TIMER0;

    // End the reset state

    value &= ~mask_reset;

    REG_SOUNDCNT_H = value;
}
