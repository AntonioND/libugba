// SPDX-License-Identifier: MIT
//
// Copyright (c) 2020 Antonio Niño Díaz

// Example of how to use the framebuffer in video mode 3.

#include <ugba/ugba.h>

void update_affine_matrix(int x, int y, int angle)
{
    bg_affine_src bg_src_start = {
        x << 8, y << 8,
        0, 0,
        1 << 8, 1 << 8,
        angle << 8,
        0
    };

    bg_affine_dst bg_dst;

    SWI_BgAffineSet(&bg_src_start, &bg_dst, 1);

    BG_AffineTransformSet(2, &bg_dst);
}

int main(int argc, char *argv[])
{
    UGBA_Init(&argc, &argv);

    IRQ_Enable(IRQ_VBLANK);

    REG_DISPCNT = DISPCNT_BG_MODE(3) | DISPCNT_BG2_ENABLE;

    uint16_t *vram = BG_Mode3FramebufferGet();

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

    int x = 0, y = 0;
    int angle = 0;

    update_affine_matrix(x, y, angle);

    while (1)
    {
        SWI_VBlankIntrWait();

        KEYS_Update();

        uint16_t keys = KEYS_Held();

        if (keys & KEY_UP)
            y++;
        else if (keys & KEY_DOWN)
            y--;

        if (keys & KEY_RIGHT)
            x--;
        else if (keys & KEY_LEFT)
            x++;

        if (keys & KEY_L)
            angle++;
        else if (keys & KEY_R)
            angle--;

        update_affine_matrix(x, y, angle);
    }
}
