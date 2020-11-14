// SPDX-License-Identifier: MIT
//
// Copyright (c) 2020 Antonio Niño Díaz

#include <gbaline.h>

int GBA_main(int argc, char *argv[])
{
    IRQ_Enable(IRQ_VBLANK);

    REG_DISPCNT = 3 | (1 << 10);

    REG_BG2PA = 1 << 8;
    REG_BG2PB = 0 << 8;
    REG_BG2PC = 0 << 8;
    REG_BG2PD = 1 << 8;

    uint16_t *vram = MEM_VRAM;

    for (int j = 0; j < 160; j++)
    {
        for (int i = 0; i < 240; i++)
        {
            uint16_t r = i & 0x1F;
            uint16_t g = j & 0x1F;
            uint16_t b = (i + j) & 0x1F;

            vram[240 * j + i] = (b << 10) | (g << 5) | r;
        }
    }

    while (1)
    {
        SWI_VBlankIntrWait();
    }

    return 0;
}
