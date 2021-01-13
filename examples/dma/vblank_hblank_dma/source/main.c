// SPDX-License-Identifier: MIT
//
// Copyright (c) 2021 Antonio Niño Díaz

// Test that makes sure that it is possible to do a one-shot DMA copy triggered
// by VBL or HBL.

#include <string.h>

#include <ugba/ugba.h>

// File generated with:
//     head -c 128 </dev/urandom >random.bin

#include "random_bin.h"

// 128 byte buffers aligned to at least 32 bit (for DMA copies)
static uint64_t buffer_vbl[128 / sizeof(uint64_t)];
static uint64_t buffer_hbl[128 / sizeof(uint64_t)];

int main(int argc, char *argv[])
{
    UGBA_Init(&argc, &argv);

    // Enable interrupts. This is needed for SWI_VBlankIntrWait() to work.
    IRQ_Enable(IRQ_VBLANK);

    // Enable console
    DISP_ModeSet(0);
    CON_InitDefault();

    // Test VBL trigger
    // ----------------

    // Wait until VBL happens to set up the copy
    SWI_VBlankIntrWait();

    // Use channel 3 because it's the only one that can read from ROM
    DMA_VBLCopy32(3, random_bin, buffer_vbl, random_bin_size);

    // Make sure that DMA is enabled
    int vbl_not_enabled = 0;
    if ((REG_DMA3CNT_H & DMACNT_DMA_ENABLE) == 0)
        vbl_not_enabled = 1;

    // Wait until the next frame to see the results of the copy
    SWI_VBlankIntrWait();

    // Make sure that DMA is disabled automatically
    int vbl_not_disabled = 0;
    if (REG_DMA3CNT_H & DMACNT_DMA_ENABLE)
        vbl_not_disabled = 1;

    // Test HBL trigger
    // ----------------

    // Wait until VBL happens to set up the copy
    SWI_VBlankIntrWait();

    // Use channel 3 because it's the only one that can read from ROM
    DMA_HBLCopy32(3, random_bin, buffer_hbl, random_bin_size);

    // Make sure that DMA is enabled
    int hbl_not_enabled = 0;
    if ((REG_DMA3CNT_H & DMACNT_DMA_ENABLE) == 0)
        hbl_not_enabled = 1;

    // Wait until the next frame to see the results of the copies
    SWI_VBlankIntrWait();

    // Make sure that DMA is disabled automatically
    int hbl_not_disabled = 0;
    if (REG_DMA3CNT_H & DMACNT_DMA_ENABLE)
        hbl_not_disabled = 1;

    // Print results
    // -------------

    if (vbl_not_enabled)
        CON_Print("VBL: not enabled\n");
    if (hbl_not_enabled)
        CON_Print("HBL: not enabled\n");

    if (vbl_not_disabled)
        CON_Print("VBL: not disabled\n");
    if (hbl_not_disabled)
        CON_Print("HBL: not disabled\n");

    if (memcmp(random_bin, buffer_vbl, random_bin_size) == 0)
        CON_Print("VBL: copy ok\n");
    else
        CON_Print("VBL: copy failed\n");

    if (memcmp(random_bin, buffer_hbl, random_bin_size) == 0)
        CON_Print("HBL: copy ok\n");
    else
        CON_Print("HBL: copy failed\n");

    // Loop forever
    while (1)
        SWI_VBlankIntrWait();
}
