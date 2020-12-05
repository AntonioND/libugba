// SPDX-License-Identifier: MIT
//
// Copyright (c) 2020 Antonio Niño Díaz

// Example that plots y = sqrt(x) scaled to match the screen size

#include <ugba/ugba.h>

int main(int argc, char *argv[])
{
    UGBA_Init(&argc, &argv);

    IRQ_Enable(IRQ_VBLANK);

    DISPLAY_ModeSet(3);
    DISPLAY_LayersEnable(0, 0, 1, 0, 0);

    REG_BG2PA = 1 << 8;
    REG_BG2PB = 0 << 8;
    REG_BG2PC = 0 << 8;
    REG_BG2PD = 1 << 8;

    uint16_t *framebuffer = BG_Mode3FramebufferGet();

    uint32_t zero = 0;
    size_t size = GBA_SCREEN_W * GBA_SCREEN_H * sizeof(uint16_t);
    SWI_CpuFastSet_Fill32(&zero, framebuffer, size);

    uint32_t max_x = GBA_SCREEN_W * 256;
    uint32_t max_y = SWI_Sqrt(max_x);
    float scale_y = (float)GBA_SCREEN_H / (float)max_y;

    for (uint16_t x = 0; x < GBA_SCREEN_W; x++)
    {
        uint16_t y = SWI_Sqrt(x * 256) * scale_y;

        framebuffer[y * GBA_SCREEN_W + x] = RGB15(31, 0, 0);
    }

    while (1)
        SWI_VBlankIntrWait();
}
