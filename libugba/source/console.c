// SPDX-License-Identifier: MIT
//
// Copyright (c) 2020 Antonio Niño Díaz

#include <ugba/ugba.h>

#include "font.h"

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
    scroll_y = 0;
    con_x = 0;
    con_y = 0;

    uint16_t *dst = MEM_VRAM_BG;
    SWI_LZ77UnCompReadNormalWrite16bit(fontTiles, dst);

    dst = MEM_PALETTE_BG + (15 * 16);
    SWI_CpuSet_Copy16(fontPal, dst, fontPalLen);

    size_t tileset_size = 256 * (8 * 8 * 4) / 8;
    map_base = (uint16_t *)(((uintptr_t)MEM_VRAM_BG) + tileset_size);

    CON_Clear();

    REG_DISPCNT |= DISPCNT_BG0_ENABLE;

    REG_BG0CNT =
        BGCNT_BG_PRIORITY(0) |
        BGCNT_TILES_BASE((uintptr_t)MEM_VRAM_BG) |
        BGCNT_16_COLORS |
        BGCNT_MAP_BASE((uintptr_t)map_base) |
        BGCNT_REGULAR_256x256;
}

void CON_PutChar(char c)
{
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
    for (int j = 0; j < 32; j++)
        for (int i = 0; i < 32; i++)
            map_base[j * 32 + i] = 0;
}
