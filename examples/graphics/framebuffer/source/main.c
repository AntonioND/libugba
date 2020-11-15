// SPDX-License-Identifier: MIT
//
// Copyright (c) 2020 Antonio Niño Díaz

#include <gbaline.h>

int GBA_main(int argc, char *argv[])
{
    IRQ_Enable(IRQ_VBLANK);

    REG_DISPCNT = DISPCNT_BG_MODE(3) | DISPCNT_BG2_ENABLE;

    REG_BG2PA = 1 << 8;
    REG_BG2PB = 0 << 8;
    REG_BG2PC = 0 << 8;
    REG_BG2PD = 1 << 8;

    uint16_t *vram = MEM_VRAM;

    for (int j = 0; j < GBA_SCREEN_H; j++)
    {
        for (int i = 0; i < GBA_SCREEN_W; i++)
        {
            uint16_t r = i & 0x1F;
            uint16_t g = j & 0x1F;
            uint16_t b = (i + j) & 0x1F;

            uint16_t color = RGB15(r, g, b);

            vram[240 * j + i] = color;
        }
    }

    if (Debug_Autotest())
    {
        for (int i = 0; i < 5; i++)
            SWI_VBlankIntrWait();
        Debug_Screenshot();
    }
    else
    {
        while (1)
            SWI_VBlankIntrWait();
    }

    return 0;
}
