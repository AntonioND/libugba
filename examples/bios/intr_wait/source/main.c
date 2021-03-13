// SPDX-License-Identifier: MIT
//
// Copyright (c) 2021 Antonio Niño Díaz

// Test that SWI_IntrWait() works for any interrupt type.

#include <string.h>

#include <ugba/ugba.h>

// File generated with:
//     head -c 128 </dev/urandom > random.bin

#include "random_bin.h"

// Buffers aligned to at least 32 bit (for DMA copies). The data of random.bin
// is copied to src_buffer[] so that it can be copied with DMA to dst_buffer[].
// Only DMA 3 can copy from ROM, DMA 0, 1 and 2 can't copy from ROM, so it is
// needed to put the data somewhere else to do tests with them.
static uint64_t src_buffer[128 / sizeof(uint64_t)];
static uint64_t dst_buffer[128 / sizeof(uint64_t)];

static int vbl_count, hbl_count, vcount_count, keypad_count;
static int timer_count[4], dma_count[4];

void vbl_handler(void)
{
    vbl_count++;
}

void hbl_handler(void)
{
    hbl_count++;
}

void vcount_handler(void)
{
    vcount_count++;
}

void timer_0_handler(void)
{
    timer_count[0]++;
}

void timer_1_handler(void)
{
    timer_count[1]++;
}

void timer_2_handler(void)
{
    timer_count[2]++;
}

void timer_3_handler(void)
{
    timer_count[3]++;
}

static int DMA_Copy32IRQ(int channel, const void *src, void *dst, size_t size)
{
    return DMA_Transfer(channel, src, dst, size,
                        DMACNT_DST_INCREMENT | DMACNT_SRC_INCREMENT |
                        DMACNT_TRANSFER_32_BITS | DMACNT_START_NOW |
                        DMACNT_IRQ_ENABLE);
}

static int DMA_HBLCopy32IRQ(int channel, const void *src, void *dst, size_t size)
{
    return DMA_Transfer(channel, src, dst, size,
                        DMACNT_DST_INCREMENT | DMACNT_SRC_INCREMENT |
                        DMACNT_TRANSFER_32_BITS | DMACNT_START_HBLANK |
                        DMACNT_IRQ_ENABLE);
}

static int DMA_VBLCopy32IRQ(int channel, const void *src, void *dst, size_t size)
{
    return DMA_Transfer(channel, src, dst, size,
                        DMACNT_DST_INCREMENT | DMACNT_SRC_INCREMENT |
                        DMACNT_TRANSFER_32_BITS | DMACNT_START_VBLANK |
                        DMACNT_IRQ_ENABLE);
}

void dma_0_handler(void)
{
    dma_count[0]++;
}

void dma_1_handler(void)
{
    dma_count[1]++;
}

void dma_2_handler(void)
{
    dma_count[2]++;
}

void dma_3_handler(void)
{
    dma_count[3]++;
}

void keypad_handler(void)
{
    keypad_count++;
}

int main(int argc, char *argv[])
{
    UGBA_Init(&argc, &argv);

    // Prepare data for the DMA tests
    memcpy(&(src_buffer[0]), random_bin, random_bin_size);

    DISP_ModeSet(0);

    CON_InitDefault();

    // VBL
    // ---

    CON_Print("VBLANK: ");

    IRQ_SetHandler(IRQ_VBLANK, vbl_handler);
    IRQ_Enable(IRQ_VBLANK);

    SWI_IntrWait(0, IRQF_VBLANK);

    IRQ_Disable(IRQ_VBLANK);

    if (vbl_count == 0)
        CON_Print("Failed\n");
    else
        CON_Print("OK\n");

    // Let the rest of the test use SWI_VBlankIntrWait()
    IRQ_SetHandler(IRQ_VBLANK, NULL);
    IRQ_Enable(IRQ_VBLANK);

    // HBL
    // ---

    CON_Print("HBLANK: ");

    IRQ_SetHandler(IRQ_HBLANK, hbl_handler);
    IRQ_Enable(IRQ_HBLANK);

    SWI_IntrWait(0, IRQF_HBLANK);

    IRQ_Disable(IRQ_HBLANK);

    if (hbl_count == 0)
        CON_Print("Failed\n");
    else
        CON_Print("OK\n");

    // VCOUNT
    // ------

    CON_Print("VCOUNT: ");

    IRQ_SetHandler(IRQ_VCOUNT, vcount_handler);
    IRQ_Enable(IRQ_VCOUNT);
    IRQ_SetReferenceVCOUNT(45);

    SWI_IntrWait(0, IRQF_VCOUNT);

    IRQ_Disable(IRQ_VCOUNT);

    if (vcount_count == 0)
        CON_Print("Failed\n");
    else
        CON_Print("OK\n");

    // Timer 0
    // -------

    CON_Print("Timer 0: ");

    IRQ_SetHandler(IRQ_TIMER0, timer_0_handler);
    IRQ_Enable(IRQ_TIMER0);
    TM_TimerStartMs(0, 50, 1);

    SWI_IntrWait(0, IRQF_TIMER0);

    IRQ_Disable(IRQ_TIMER0);

    if (timer_count[0] == 0)
        CON_Print("Failed\n");
    else
        CON_Print("OK\n");

    // Timer 1
    // -------

    CON_Print("Timer 1: ");

    IRQ_SetHandler(IRQ_TIMER1, timer_1_handler);
    IRQ_Enable(IRQ_TIMER1);
    TM_TimerStartMs(1, 50, 1);

    SWI_IntrWait(0, IRQF_TIMER1);

    IRQ_Disable(IRQ_TIMER1);

    if (timer_count[1] == 0)
        CON_Print("Failed\n");
    else
        CON_Print("OK\n");

    // Timer 2
    // -------

    CON_Print("Timer 2: ");

    IRQ_SetHandler(IRQ_TIMER2, timer_2_handler);
    IRQ_Enable(IRQ_TIMER2);
    TM_TimerStartMs(2, 50, 1);

    SWI_IntrWait(0, IRQF_TIMER2);

    IRQ_Disable(IRQ_TIMER2);

    if (timer_count[2] == 0)
        CON_Print("Failed\n");
    else
        CON_Print("OK\n");

    // Timer 3
    // -------

    CON_Print("Timer 3: ");

    IRQ_SetHandler(IRQ_TIMER3, timer_3_handler);
    IRQ_Enable(IRQ_TIMER3);
    TM_TimerStartMs(3, 50, 1);

    SWI_IntrWait(1, IRQF_TIMER3);

    IRQ_Disable(IRQ_TIMER3);

    if (timer_count[2] == 0)
        CON_Print("Failed\n");
    else
        CON_Print("OK\n");

    // Serial
    // ------

    // TODO: Serial is not supported

    // DMA 0
    // -----

    CON_Print("DMA 0: ");

    IRQ_SetHandler(IRQ_DMA0, dma_0_handler);
    IRQ_Enable(IRQ_DMA0);

    memset(&(dst_buffer[0]), 0, random_bin_size);
    DMA_Copy32IRQ(0, &(src_buffer[0]), &(dst_buffer[0]), 128);

    // In this case, don't clear previous flags, as the DMA copye has already
    // happened and the IRQ handler should have already happened.
    SWI_IntrWait(0, IRQF_DMA0);

    IRQ_Disable(IRQ_DMA0);

    if ((dma_count[0] == 0) ||
        (memcmp(&(src_buffer[0]), &(dst_buffer[0]), 128) != 0))
        CON_Print("Failed\n");
    else
        CON_Print("OK\n");

    // DMA 1
    // -----

    CON_Print("DMA 1: ");

    IRQ_SetHandler(IRQ_DMA1, dma_1_handler);
    IRQ_Enable(IRQ_DMA1);

    memset(&(dst_buffer[0]), 0, random_bin_size);
    SWI_VBlankIntrWait();
    DMA_HBLCopy32IRQ(1, &(src_buffer[0]), &(dst_buffer[0]), 128);

    SWI_IntrWait(1, IRQF_DMA1);

    IRQ_Disable(IRQ_DMA1);

    if ((dma_count[1] == 0) ||
        (memcmp(&(src_buffer[0]), &(dst_buffer[0]), 128) != 0))
        CON_Print("Failed\n");
    else
        CON_Print("OK\n");

    // DMA 2
    // -----

    CON_Print("DMA 2: ");

    IRQ_SetHandler(IRQ_DMA2, dma_2_handler);
    IRQ_Enable(IRQ_DMA2);

    memset(&(dst_buffer[0]), 0, random_bin_size);
    SWI_VBlankIntrWait();
    DMA_VBLCopy32IRQ(2, &(src_buffer[0]), &(dst_buffer[0]), 128);

    SWI_IntrWait(1, IRQF_DMA2);

    IRQ_Disable(IRQ_DMA2);

    if ((dma_count[2] == 0) ||
        (memcmp(&(src_buffer[0]), &(dst_buffer[0]), 128) != 0))
        CON_Print("Failed\n");
    else
        CON_Print("OK\n");

    // DMA 3
    // -----

    CON_Print("DMA 3: ");

    IRQ_SetHandler(IRQ_DMA3, dma_3_handler);
    IRQ_Enable(IRQ_DMA3);

    memset(&(dst_buffer[0]), 0, random_bin_size);
    SWI_VBlankIntrWait();
    DMA_VBLCopy32IRQ(3, &(src_buffer[0]), &(dst_buffer[0]), 128);

    SWI_IntrWait(1, IRQF_DMA3);

    IRQ_Disable(IRQ_DMA3);

    if ((dma_count[3] == 0) ||
        (memcmp(&(src_buffer[0]), &(dst_buffer[0]), 128) != 0))
        CON_Print("Failed\n");
    else
        CON_Print("OK\n");

    // KEYPAD
    // ------

    CON_Print("Now press any key...\n");

    CON_Print("KEYPAD: ");

    IRQ_SetHandler(IRQ_KEYPAD, keypad_handler);
    IRQ_Enable(IRQ_KEYPAD);

    KEYS_IRQEnablePressedAny(KEY_LEFT | KEY_RIGHT | KEY_UP | KEY_DOWN |
                             KEY_A | KEY_B | KEY_L | KEY_R |
                             KEY_START | KEY_SELECT);

    SWI_IntrWait(1, IRQF_KEYPAD);

    IRQ_Disable(IRQ_KEYPAD);

    if (keypad_count == 0)
        CON_Print("Failed\n");
    else
        CON_Print("OK\n");

    // Gamepak
    // -------

    // TODO: Gamepak interrupt is not supported

    // Test done: Infinite loop
    // ------------------------

    while (1)
        SWI_VBlankIntrWait();
}
