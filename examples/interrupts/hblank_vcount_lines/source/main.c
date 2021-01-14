// SPDX-License-Identifier: MIT
//
// Copyright (c) 2021 Antonio Niño Díaz

// Example that makes sure that the HBL and VCOUNT interrupts happen in every
// scanline, even the VBL scanlines.

#include <stdio.h>

#include <ugba/ugba.h>

struct {
    int hbl_flag;
    int vcount_flag;
} line_info[256];

void hbl_handler(void)
{
    line_info[REG_VCOUNT].hbl_flag = 1;
}

void vcount_handler(void)
{
    uint16_t vcount = REG_VCOUNT;

    line_info[vcount].vcount_flag = 1;

    uint16_t dispstat = REG_DISPSTAT & ~DISPSTAT_VCOUNT_MASK;

    if (vcount == 227)
        REG_DISPSTAT = dispstat | DISPSTAT_VCOUNT(0);
    else
        REG_DISPSTAT = dispstat | DISPSTAT_VCOUNT(vcount + 1);
}

int main(int argc, char *argv[])
{
    UGBA_Init(&argc, &argv);

    // Enable interrupts
    IRQ_SetHandler(IRQ_HBLANK, hbl_handler);
    IRQ_SetHandler(IRQ_VCOUNT, vcount_handler);
    IRQ_Enable(IRQ_VBLANK);
    IRQ_Enable(IRQ_HBLANK);
    IRQ_Enable(IRQ_VCOUNT);

    SWI_VBlankIntrWait();
    SWI_VBlankIntrWait();
    SWI_VBlankIntrWait();

    IRQ_Disable(IRQ_HBLANK);
    IRQ_Disable(IRQ_VCOUNT);

    // Set the display to mode 0 so that all backgrounds are in regular mode,
    // and turn on background 0.
    DISP_ModeSet(0);
    DISP_LayersEnable(1, 0, 0, 0, 0);

    CON_InitDefault();

    for (int i = 0; i < 256; i++)
    {
        char text[10];
        snprintf(text, sizeof(text), "%d", line_info[i].hbl_flag);
        CON_Print(text);
    }
    CON_Print("\n");

    for (int i = 0; i < 256; i++)
    {
        char text[10];
        snprintf(text, sizeof(text), "%d", line_info[i].vcount_flag);
        CON_Print(text);
    }
    CON_Print("\n");

    while (1)
    {
        SWI_VBlankIntrWait();
    }
}
