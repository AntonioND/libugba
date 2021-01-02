// SPDX-License-Identifier: MIT
//
// Copyright (c) 2020 Antonio Niño Díaz

// Example that plots y = arctan(x) scaled to match the screen size. The usable
// part of the function is inside the green vertical lines.

#include <ugba/ugba.h>

int main(int argc, char *argv[])
{
    UGBA_Init(&argc, &argv);

    IRQ_Enable(IRQ_VBLANK);

    DISP_ModeSet(3);
    DISP_LayersEnable(0, 0, 1, 0, 0);

    REG_BG2PA = 1 << 8;
    REG_BG2PB = 0 << 8;
    REG_BG2PC = 0 << 8;
    REG_BG2PD = 1 << 8;

    uint16_t *framebuffer = BG_Mode3FramebufferGet();

    uint32_t zero = 0;
    size_t size = GBA_SCREEN_W * GBA_SCREEN_H * sizeof(uint16_t);
    SWI_CpuFastSet_Fill32(&zero, framebuffer, size);

    for (uint16_t y = 0; y < GBA_SCREEN_H; y++)
    {
        uint16_t division = GBA_SCREEN_W / (3 * 2);

        framebuffer[y * GBA_SCREEN_W + division * 2] = RGB15(0, 31, 0);
        framebuffer[y * GBA_SCREEN_W + division * 4] = RGB15(0, 31, 0);
    }

    for (uint16_t x = 0; x < GBA_SCREEN_W; x++)
    {
        int16_t tan = x - (GBA_SCREEN_W / 2);
        tan = (tan << 14) / (GBA_SCREEN_W / 2);
        tan *= 3; // This makes it so that the graph shows out-of-bounds values
        uint16_t y = (GBA_SCREEN_H / 2) + (SWI_ArcTan(tan) >> 7);

        framebuffer[y * GBA_SCREEN_W + x] = RGB15(31, 0, 0);
    }

    while (1)
        SWI_VBlankIntrWait();
}
