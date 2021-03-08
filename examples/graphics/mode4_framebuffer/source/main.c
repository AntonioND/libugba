// SPDX-License-Identifier: MIT
//
// Copyright (c) 2020-2021 Antonio Niño Díaz

// Example of how to use the framebuffer in mode 4 and switch between the
// frontbuffer and backbuffer.

#include <ugba/ugba.h>

void update_affine_matrix(int x, int y, int angle)
{
    bg_affine_src bg_src_start = {
        x << 8, y << 8,
        0, 0,
        1 << 8, 1 << 8,
        angle << 8
    };

    bg_affine_dst bg_dst;

    SWI_BgAffineSet(&bg_src_start, &bg_dst, 1);

    BG_AffineTransformSet(2, &bg_dst);
}

int main(int argc, char *argv[])
{
    UGBA_Init(&argc, &argv);

    IRQ_Enable(IRQ_VBLANK);

    DISP_ModeSet(4);
    DISP_LayersEnable(0, 0, 1, 0, 0);

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

    uint16_t *vram_front = BG_Mode4FramebufferActiveGet();

    uint16_t data;

    int data_index = 0;

    for (int j = 0; j < GBA_SCREEN_H; j++)
    {
        for (int i = 0; i < GBA_SCREEN_W; i++)
        {
            uint16_t index = (i + j) & 0x1F;
            if (data_index == 0)
            {
                data = index;
            }
            else
            {
                data |= index << 8;
                *vram_front++ = data;
            }
            data_index ^= 1;
        }
    }

    uint16_t *vram_back = BG_Mode4FramebufferBackGet();

    data_index = 0;

    for (int j = 0; j < GBA_SCREEN_H; j++)
    {
        for (int i = 0; i < GBA_SCREEN_W; i++)
        {
            uint16_t index = (i - j) & 0x1F;
            if (data_index == 0)
            {
                data = index;
            }
            else
            {
                data |= index << 8;
                *vram_back++ = data;
            }
            data_index ^= 1;
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
