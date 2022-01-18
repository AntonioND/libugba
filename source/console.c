// SPDX-License-Identifier: MIT
//
// Copyright (c) 2020-2022 Antonio Niño Díaz

#include <ugba/ugba.h>

#include "graphics/font_palette_bin.h"
#include "graphics/font_tiles_bin.h"

// This is needed because the scroll registers are write-only.
static int scroll_y;

// Coordinates of the map
static int con_x, con_y;

// Base of the tile map used by the console
static uint16_t *map_base;

static void CON_NewLine(void)
{
    con_x = 0;
    con_y = (con_y + 1) & 31;

    if (con_y == ((scroll_y + 20) & 31))
    {
        scroll_y = (scroll_y + 1) & 31;

        // Clear the next line that is going to appear on the screen
        for (int i = 0; i < 32; i++)
            map_base[con_y * 32 + i] = 0;

        // Scroll the background to show the next line
        REG_BG0VOFS = scroll_y * 8;
    }
}

void CON_InitDefault(void)
{
    // Set global console variables to initial values

    scroll_y = 0;
    con_x = 0;
    con_y = 0;

    // Calculate base addresses of tileset and map

    // The tileset size is 256 * (8 * 8 * 4) / 8 = 8 KB = 0x2000
    // This means that the map has to be placed 4 blocks away from the tileset

    uintptr_t tiles_block_base_addr = MEM_BG_TILES_BLOCK_ADDR(3);
    uintptr_t map_block_base_addr = MEM_BG_MAP_BLOCK_ADDR((3 * 8) + 4);

    // Save pointer so that other functions can use it

    map_base = (uint16_t *)map_block_base_addr;

    // Load font tileset and palette

    uint16_t *dst = (uint16_t *)tiles_block_base_addr;
    SWI_LZ77UnCompReadNormalWrite16bit(font_tiles_bin, dst);

    dst = MEM_PALETTE_BG + (15 * 16);
    SWI_CpuSet_Copy16(font_palette_bin, dst, font_palette_bin_size);

    // Clear console map

    CON_Clear();

    // Enable background

    REG_DISPCNT |= DISPCNT_BG0_ENABLE;

    REG_BG0CNT =
        BGCNT_BG_PRIORITY(0) |
        BGCNT_TILES_BLOCK_BASE(tiles_block_base_addr) |
        BGCNT_16_COLORS |
        BGCNT_MAP_BLOCK_BASE(map_block_base_addr) |
        BGCNT_REGULAR_256x256;
}

void CON_PutChar(char c)
{
    if (map_base == NULL)
        return;

    if (c == '\n')
    {
        CON_NewLine();
        return;
    }

    uint16_t entry = MAP_REGULAR_TILE(c) | MAP_REGULAR_PALETTE(15);
    uint32_t index = con_y * 32 + con_x;
    map_base[index] = entry;

    con_x++;
    if (con_x == 30)
        CON_NewLine();
}

void CON_Print(const char *string)
{
    while (1)
    {
        char c = *string++;
        if (c == '\0')
            break;

        CON_PutChar(c);
    }
}

void CON_CursorSet(int x, int y)
{
    con_x = x;
    con_y = (y + scroll_y) & 31;
}

void CON_Clear(void)
{
    if (map_base == NULL)
        return;

    for (int j = 0; j < 32; j++)
        for (int i = 0; i < 32; i++)
            map_base[j * 32 + i] = 0;

    CON_CursorSet(0, 0);
}
