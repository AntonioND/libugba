// SPDX-License-Identifier: MIT
//
// Copyright (c) 2021 Antonio Niño Díaz

// Test DMA interrupt handling in immediate, VBL and HBL modes.

#include <stdio.h>
#include <string.h>

#include <ugba/ugba.h>

static int immediate_interrupt_count = 0;
static int vbl_interrupt_count = 0;
static int hbl_interrupt_count = 0;

static void dma_interrupt_handler_immediate(void)
{
    immediate_interrupt_count++;
}

static void dma_interrupt_handler_vbl(void)
{
    vbl_interrupt_count++;
}

static void dma_interrupt_handler_hbl(void)
{
    hbl_interrupt_count++;
}

// Buffer aligned to at least 32 bit (for DMA copies)
static uint64_t buffer_dest[(16 * 1024) / sizeof(uint64_t)];

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
                        DMACNT_IRQ_ENABLE | DMACNT_REPEAT_ON);
}

static int DMA_VBLCopy32IRQ(int channel, const void *src, void *dst, size_t size)
{
    return DMA_Transfer(channel, src, dst, size,
                        DMACNT_DST_INCREMENT | DMACNT_SRC_INCREMENT |
                        DMACNT_TRANSFER_32_BITS | DMACNT_START_VBLANK |
                        DMACNT_IRQ_ENABLE | DMACNT_REPEAT_ON);
}

int main(int argc, char *argv[])
{
    UGBA_Init(&argc, &argv);

    IRQ_Enable(IRQ_VBLANK);

    DISP_ModeSet(0);
    CON_InitDefault();

    // Test immediate mode
    // -------------------

    CON_Print("Immediate...\n");

    IRQ_SetHandler(IRQ_DMA0, dma_interrupt_handler_immediate);
    IRQ_SetHandler(IRQ_DMA1, dma_interrupt_handler_immediate);
    IRQ_SetHandler(IRQ_DMA2, dma_interrupt_handler_immediate);
    IRQ_SetHandler(IRQ_DMA3, dma_interrupt_handler_immediate);
    IRQ_Enable(IRQ_DMA0);
    IRQ_Enable(IRQ_DMA1);
    IRQ_Enable(IRQ_DMA2);
    IRQ_Enable(IRQ_DMA3);

    // Copy random data from IWRAM using channels 0, 1 and 2, as they can't read
    // from ROM.
    DMA_Copy32IRQ(0, MEM_IWRAM, buffer_dest, 20);
    DMA_Copy32IRQ(1, MEM_IWRAM, buffer_dest, 20);
    DMA_Copy32IRQ(2, MEM_IWRAM, buffer_dest, 20);
    DMA_Copy32IRQ(3, MEM_IWRAM, buffer_dest, 20);

    IRQ_Disable(IRQ_DMA0);
    IRQ_Disable(IRQ_DMA1);
    IRQ_Disable(IRQ_DMA2);
    IRQ_Disable(IRQ_DMA3);

    // Test VBL trigger
    // ----------------

    CON_Print("VBL...\n");

    IRQ_SetHandler(IRQ_DMA3, dma_interrupt_handler_vbl);
    IRQ_Enable(IRQ_DMA3);

    // Wait until VBL happens to set up the copy
    SWI_VBlankIntrWait();

    // Use channel 3 because it's the only one that can read from ROM
    DMA_VBLCopy32IRQ(3, MEM_ROM_WS0, buffer_dest, 20);

    // Wait for a few frames to see the results of the copy
    SWI_VBlankIntrWait();
    SWI_VBlankIntrWait();
    SWI_VBlankIntrWait();

    DMA_Stop(3);
    IRQ_Disable(IRQ_DMA3);

    // Test HBL trigger
    // ----------------

    CON_Print("HBL...\n");

    IRQ_SetHandler(IRQ_DMA3, dma_interrupt_handler_hbl);
    IRQ_Enable(IRQ_DMA3);

    // Wait until VBL happens to set up the copy
    SWI_VBlankIntrWait();

    // Use channel 3 because it's the only one that can read from ROM
    DMA_HBLCopy32IRQ(3, MEM_ROM_WS0, buffer_dest, 20);

    // Wait for a few frames to see the results of the copy
    SWI_VBlankIntrWait();
    SWI_VBlankIntrWait();
    SWI_VBlankIntrWait();

    DMA_Stop(3);
    IRQ_Disable(IRQ_DMA3);

    // Print results
    // -------------

    char str[20];
    snprintf(str, sizeof(str), "%d %d %d", immediate_interrupt_count,
             vbl_interrupt_count, hbl_interrupt_count);
    CON_Print(str);

    while (1)
        SWI_VBlankIntrWait();
}
