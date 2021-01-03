// SPDX-License-Identifier: MIT
//
// Copyright (c) 2020-2021 Antonio Niño Díaz

// Example of how to use the framebuffer in mode 5 and switch between the
// frontbuffer and backbuffer.

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

    DISP_ModeSet(5);
    DISP_LayersEnable(0, 0, 1, 0, 0);

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

        uint16_t keys_pressed = KEYS_Pressed();

        if (keys_pressed & KEY_A)
            BG_FramebufferSwap();

        update_affine_matrix(x, y, angle);
    }
}
