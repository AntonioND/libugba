// SPDX-License-Identifier: MIT
//
// Copyright (c) 2021 Antonio Niño Díaz

// This shows that the HBL DMA isn't triggered during the VBL period.

#include <stdio.h>

#include <ugba/ugba.h>

// Buffer with enough space for the scanlines of 2 full screen refreshes
// (including VBL period, in case the emulator/library is buggy).
uint16_t dst_buffer[230 * 2];

#define PTR_REG_VCOUNT  ((const uint16_t *)PTR_REG_16(OFFSET_VCOUNT))

int main(int argc, char *argv[])
{
    UGBA_Init(&argc, &argv);

    // Enable interrupts. This is needed for SWI_VBlankIntrWait() to work.
    IRQ_Enable(IRQ_VBLANK);

    SWI_VBlankIntrWait();

    DMA_Transfer(0, PTR_REG_VCOUNT, &(dst_buffer[0]), 2,
                 DMACNT_DST_INCREMENT | DMACNT_SRC_FIXED |
                 DMACNT_TRANSFER_16_BITS | DMACNT_START_HBLANK |
                 DMACNT_REPEAT_ON);

    SWI_VBlankIntrWait();

    SWI_VBlankIntrWait();

    DMA_Stop(0);

    DISP_ModeSet(0);

    CON_InitDefault();

    for (int i = 0; i < 18; i++)
    {
        char str[10];
        snprintf(str, sizeof(str), "%3d  ", dst_buffer[i]);
        CON_Print(str);
    }

    CON_Print("\n...\n\n");

    for (int i = 150 ; i < 230; i++)
    {
        char str[10];
        snprintf(str, sizeof(str), "%3d  ", dst_buffer[i]);
        CON_Print(str);
    }

    while (1)
        SWI_VBlankIntrWait();
}
