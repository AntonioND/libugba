// SPDX-License-Identifier: MIT
//
// Copyright (c) 2020 Antonio Niño Díaz

#include <ugba/ugba.h>

int main(int argc, char *argv[])
{
    UGBA_Init(&argc, &argv);

    IRQ_Enable(IRQ_VBLANK);

    REG_DISPCNT = DISPCNT_BG_MODE(5) | DISPCNT_BG2_ENABLE;

    REG_BG2PA = 1 << 8;
    REG_BG2PB = 0 << 8;
    REG_BG2PC = 0 << 8;
    REG_BG2PD = 1 << 8;

    uint16_t *vram_front = BG_Mode5FramebufferActiveGet();

    for (int j = 0; j < 128; j++)
    {
        for (int i = 0; i < 160; i++)
        {
            uint16_t r = i & 0x1F;
            uint16_t g = j & 0x1F;
            uint16_t b = (i + j) & 0x1F;

            uint16_t color = RGB15(r, g, b);

            vram_front[160 * j + i] = color;
        }
    }

    uint16_t *vram_back = BG_Mode5FramebufferBackGet();

    for (int j = 0; j < 128; j++)
    {
        for (int i = 0; i < 160; i++)
        {
            uint16_t r = (i + j) & 0x1F;
            uint16_t g = j & 0x1F;
            uint16_t b = i & 0x1F;

            uint16_t color = RGB15(r, g, b);

            vram_back[160 * j + i] = color;
        }
    }

    while (1)
    {
        SWI_VBlankIntrWait();

        KEYS_Update();

        uint16_t keys = KEYS_Pressed();

        if (keys & KEY_A)
            BG_FramebufferSwap();
    }
}
