// SPDX-License-Identifier: MIT
//
// Copyright (c) 2020 Antonio Niño Díaz

#include <ugba/ugba.h>

int main(int argc, char *argv[])
{
    UGBA_Init(&argc, &argv);

    IRQ_Enable(IRQ_VBLANK);

    REG_DISPCNT = DISPCNT_BG_MODE(4) | DISPCNT_BG2_ENABLE;

    REG_BG2PA = 1 << 8;
    REG_BG2PB = 0 << 8;
    REG_BG2PC = 0 << 8;
    REG_BG2PD = 1 << 8;

    uint16_t *pal = MEM_PALETTE_BG;

    for (int i = 0; i < 64; i++)
    {
        uint16_t r = i;
        uint16_t g = i + 10;
        uint16_t b = i + 20;

        pal[i] = RGB15(r, g, b);
    }

    uint8_t *vram_front = BG_Mode4FramebufferActiveGet();

    for (int j = 0; j < GBA_SCREEN_H; j++)
    {
        for (int i = 0; i < GBA_SCREEN_W; i++)
        {
            uint8_t index = (i + j) & 0x1F;

            vram_front[240 * j + i] = index;
        }
    }

    uint8_t *vram_back = BG_Mode4FramebufferBackGet();

    for (int j = 0; j < GBA_SCREEN_H; j++)
    {
        for (int i = 0; i < GBA_SCREEN_W; i++)
        {
            uint8_t index = (i - j) & 0x1F;

            vram_back[240 * j + i] = index;
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
