// SPDX-License-Identifier: MIT
//
// Copyright (c) 2020 Antonio Niño Díaz

// Example of how to load 16-color sprites

#include <ugba.h>

#include "ball_green.h" // Autogenerated from ball_green.png
#include "ball_red.h"   // Autogenerated from ball_red.png

// This defines the tile index where the data of the ball is loaded in tile VRAM
#define BALL_RED_TILES_BASE         (16)
#define BALL_GREEN_TILES_BASE       (64)

// This is the palette index to be used for the sprite. There are 16 palettes of
// 16 colors each available.
#define BALL_RED_PALETTE            (2)
#define BALL_GREEN_PALETTE          (6)

int main(int argc, char *argv[])
{
    UGBA_Init(&argc, &argv);

    // Enable interrupts. This is needed for SWI_VBlankIntrWait() to work.
    IRQ_Enable(IRQ_VBLANK);

    // Set object attributes
    // ---------------------

    for (int j = 0; j < 4; j++)
    {
        for (int i = 0; i < 6; i++)
        {
            int index = j * 6 + i;
            int x = 4 + i * 40;
            int y = 4 + j * 40;

            if ((i ^ j) & 1)
            {
                OBJ_RegularInit(index, x, y, OBJ_SIZE_32x32,
                                0, BALL_RED_PALETTE, BALL_RED_TILES_BASE);
            }
            else
            {
                OBJ_RegularInit(index, x, y, OBJ_SIZE_32x32,
                                0, BALL_GREEN_PALETTE, BALL_GREEN_TILES_BASE);
            }
        }
    }

    // Load the palettes
    // -----------------

    VRAM_SpritePalette16Copy(ball_redPal, ball_redPalLen,
                             BALL_RED_PALETTE);
    VRAM_SpritePalette16Copy(ball_greenPal, ball_greenPalLen,
                             BALL_GREEN_PALETTE);

    // Load the tiles
    // --------------

    VRAM_SpriteTiles16Copy(ball_redTiles, ball_redTilesLen,
                           BALL_RED_TILES_BASE);
    VRAM_SpriteTiles16Copy(ball_greenTiles, ball_greenTilesLen,
                           BALL_GREEN_TILES_BASE);

    // Turn on the screen
    // ------------------

    REG_DISPCNT =
        // The mode doesn't matter here, it only affects the backgrounds. This
        // is just an arbitrary value.
        DISPCNT_BG_MODE(0) |
        // Turn on the rendering of sprites.
        DISPCNT_OBJ_ENABLE |
        // Enable 1D mapping. Check "8.2.1. The sprite mapping mode" in the
        // following link for more information:
        //     https://www.coranac.com/tonc/text/regobj.htm#sec-tiles
        DISPCNT_OBJ_1D_MAPPING;

    while (1)
        SWI_VBlankIntrWait();
}
