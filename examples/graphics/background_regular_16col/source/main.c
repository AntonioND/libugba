// SPDX-License-Identifier: MIT
//
// Copyright (c) 2020 Antonio Niño Díaz

// Example of how to load a 16-color background

#include <ugba/ugba.h>

#include "city.h" // Autogenerated from city.png

#define CITY_MAP_PALETTE    (0)
#define CITY_TILES_BASE     MEM_BG_TILES_BLOCK_ADDR(0)
#define CITY_MAP_BASE       MEM_BG_MAP_BLOCK_ADDR(8)

int main(int argc, char *argv[])
{
    UGBA_Init(&argc, &argv);

    // Enable interrupts. This is needed for SWI_VBlankIntrWait() to work.
    IRQ_Enable(IRQ_VBLANK);

    // Load the palette
    VRAM_BGPalette16Copy(cityPal, cityPalLen, CITY_MAP_PALETTE);

    // Load the tiles
    SWI_CpuSet_Copy16(cityTiles, (void *)CITY_TILES_BASE, cityTilesLen);

    // Load the map
    SWI_CpuSet_Copy16(cityMap, (void *)CITY_MAP_BASE, cityMapLen);

    // Setup background
    BG_RegularInit(0, BG_REGULAR_512x512, BG_16_COLORS,
                   CITY_TILES_BASE, CITY_MAP_BASE);

    int x = 80, y = 120;
    BG_RegularScrollSet(0, x, y);

    // Set the display to mode 0 so that all backgrounds are in regular mode,
    // and turn on background 0.
    REG_DISPCNT = DISPCNT_BG_MODE(0) | DISPCNT_BG0_ENABLE;

    while (1)
    {
        SWI_VBlankIntrWait();

        KEYS_Update();

        uint16_t keys = KEYS_Held();

        BG_RegularScrollGet(0, &x, &y);

        if (keys & KEY_UP)
            y--;
        else if (keys & KEY_DOWN)
            y++;

        if (keys & KEY_RIGHT)
            x++;
        else if (keys & KEY_LEFT)
            x--;

        BG_RegularScrollSet(0, x, y);
    }
}
