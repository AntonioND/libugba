// SPDX-License-Identifier: LGPL-3.0-only
//
// Copyright (c) 2011-2015, 2019-2020 Antonio Niño Díaz

#include <string.h>

#include <ugba/ugba.h>

#include "video.h"

#include "../debug_utils.h"

static int curr_screen_buffer = 0;
static uint16_t screen_buffer_array[2][240 * 160]; // Doble buffer
static uint16_t *screen_buffer = screen_buffer_array[0];

typedef void (*draw_scanline_fn)(int32_t);
static draw_scanline_fn DrawScanlineFn;

static void GBA_DrawScanlineMode0(int32_t y);
static void GBA_DrawScanlineMode1(int32_t y);
static void GBA_DrawScanlineMode2(int32_t y);
static void GBA_DrawScanlineMode3(int32_t y);
static void GBA_DrawScanlineMode4(int32_t y);
static void GBA_DrawScanlineMode5(int32_t y);
void GBA_DrawScanlineWhite(int32_t y);

static int32_t BG2lastx, BG2lasty; // For affine transformation
static int32_t BG3lastx, BG3lasty;

static int32_t MosSprX, MosSprY, MosBgX, MosBgY;
static uint32_t Win0X1, Win0X2, Win0Y1, Win0Y2;
static uint32_t Win1X1, Win1X2, Win1Y1, Win1Y2;

//-----------------------------------------------------------

static void mem_clear_32(uint32_t *ptr, uint32_t size)
{
    size >>= 2;
    while (size--)
        *ptr++ = 0;
}

//-----------------------------------------------------------

void GBA_UpdateDrawScanlineFn(void)
{
    uint32_t mode = REG_DISPCNT & 0x7;

    switch (mode)
    {
        case 0:
            DrawScanlineFn = &GBA_DrawScanlineMode0;
            break;
        case 1:
            DrawScanlineFn = &GBA_DrawScanlineMode1;
            break;
        case 2:
            DrawScanlineFn = &GBA_DrawScanlineMode2;
            break;
        case 3:
            DrawScanlineFn = &GBA_DrawScanlineMode3;
            break;
        case 4:
            DrawScanlineFn = &GBA_DrawScanlineMode4;
            break;
        case 5:
            DrawScanlineFn = &GBA_DrawScanlineMode5;
            break;
        case 6:
        case 7:
        default:
            // TODO: Check how this works in real hardware
            DrawScanlineFn = &GBA_DrawScanlineMode3;
            break;
    }
}

void GBA_DrawScanline(int y)
{
    GBA_UpdateDrawScanlineFn();

    if (y == 0)
    {
        curr_screen_buffer ^= 1;
        screen_buffer = screen_buffer_array[curr_screen_buffer];

        // Fetch initial values of the affine matrices registers

        BG2lastx = REG_BG2X;
        if (BG2lastx & BIT(27))
            BG2lastx |= 0xF0000000;
        BG2lasty = REG_BG2Y;
        if (BG2lasty & BIT(27))
            BG2lasty |= 0xF0000000;

        BG3lastx = REG_BG3X;
        if (BG3lastx & BIT(27))
            BG3lastx |= 0xF0000000;
        BG3lasty = REG_BG3Y;
        if (BG3lasty & BIT(27))
            BG3lasty |= 0xF0000000;
    }

    // Fetch values of some registers

    // WIN0H
    Win0X1 = (REG_WIN0H >> 8) & 0xFF;
    Win0X2 = REG_WIN0H & 0xFF;
    if (Win0X2 > 240)
        Win0X2 = 240;
    if (Win0X1 > Win0X2)
        Win0X2 = 240; // Real bounds
    if (Win0X1 > 240)
        Win0X1 = 240;

    // WIN0V
    Win0Y1 = (REG_WIN0V >> 8) & 0xFF;
    Win0Y2 = REG_WIN0V & 0xFF;
    if (Win0Y2 > 160)
        Win0Y2 = 160;
    if (Win0Y1 > Win0Y2)
        Win0X2 = 160; // Real bounds
    if (Win0Y1 > 160)
        Win0Y1 = 160;

    // WIN1H
    Win1X1 = (REG_WIN1H >> 8) & 0xFF;
    Win1X2 = REG_WIN1H & 0xFF;
    if (Win1X2 > 240)
        Win1X2 = 240;
    if (Win1X1 > Win1X2)
        Win1X2 = 240; // Real bounds
    if (Win1X1 > 240)
        Win1X1 = 240;

    // WIN1V
    Win1Y1 = (REG_WIN1V >> 8) & 0xFF;
    Win1Y2 = REG_WIN1V & 0xFF;
    if (Win1Y2 > 160)
        Win1Y2 = 160;
    if (Win1Y1 > Win1Y2)
        Win1X2 = 160; // Real bounds
    if (Win1Y1 > 160)
        Win1Y1 = 160;

    // MOSAIC
    int mos = REG_MOSAIC;
    MosBgX = (mos & 0xF) + 1;
    MosBgY = ((mos >> 4) & 0xF) + 1;
    MosSprX = ((mos >> 8) & 0xF) + 1;
    MosSprY = ((mos >> 12) & 0xF) + 1;

    // Draw scanline
    DrawScanlineFn(y);

    // Update values of the affine matrices internal registers
    BG2lastx += (int32_t)(int16_t)REG_BG2PB;
    BG2lasty += (int32_t)(int16_t)REG_BG2PD;

    BG3lastx += (int32_t)(int16_t)REG_BG3PB;
    BG3lasty += (int32_t)(int16_t)REG_BG3PD;
}

void GBA_DrawScanlineWhite(int y)
{
    if (y == 0)
    {
        curr_screen_buffer ^= 1;
        screen_buffer = screen_buffer_array[curr_screen_buffer];
    }
    uint32_t *destptr = (uint32_t *)&screen_buffer[240 * y];

    for (int i = 0; i < 240 / 2; i++)
        *destptr++ = 0x7FFF7FFF;
}

//------------------------------------------------------------------------------
//
uint16_t sprfb[4][240];
int sprvisible[4][240];
int sprwin[240];
int sprblend[4][240];   // This sprite pixel is in blending mode
uint16_t sprblendfb[4][240]; // One line for each sprite priority

static const int spr_size[4][4][2] = { // Inputs = [Shape][Size][{x, y}]
    { { 8, 8 }, { 16, 16 }, { 32, 32 }, { 64, 64 } }, // Square
    { { 16, 8 }, { 32, 8 }, { 32, 16 }, { 64, 32 } }, // Horizontal
    { { 8, 16 }, { 8, 32 }, { 16, 32 }, { 32, 64 } }, // Vertical
    { { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 } }        // Prohibited
};

static void gba_sprites_draw_mode012(int32_t ly)
{
    oam_entry_t *spr = (oam_entry_t *)((uint8_t *)MEM_OAM_ADDR);

    for (int i = 0; i < 128; i++)
    {
        uint16_t attr0 = spr->attr0;

        if (attr0 & BIT(8)) // Affine sprite -- No H flip or V flip
        {
            int mosaic = attr0 & BIT(12);

            uint16_t attr1 = spr->attr1;
            uint16_t attr2 = spr->attr2;

            oam_matrix_entry_t *mat =
                    &(((oam_matrix_entry_t *)((uint8_t *)MEM_OAM_ADDR))[(attr1 >> 9) & 0x1F]);

            uint16_t shape = attr0 >> 14;
            uint16_t size = attr1 >> 14;
            uint32_t hsx = spr_size[shape][size][0] >> 1; // Half size
            uint32_t hsy = spr_size[shape][size][1] >> 1;

            int y = (attr0 & 0xFF);
            y |= (y < 160) ? 0 : 0xFFFFFF00;
            int x = (int)(attr1 & 0x1FF) | ((attr1 & BIT(8)) ? 0xFFFFFE00 : 0);

            // Is double size?
            int hrealsx = (attr0 & BIT(9)) ? hsx << 1 : hsx; // Half canvas size
            int hrealsy = (attr0 & BIT(9)) ? hsy << 1 : hsy;

            int cx = x + hrealsx; // Center of the sprite
            int cy = y + hrealsy;

            if ((y <= ly) && ((y + (hrealsy << 1)) > ly)) // If in this line
            {
                int mode = (attr0 >> 10) & 3;

                uint16_t prio = (attr2 >> 10) & 3;
                uint16_t tilebaseno = attr2 & 0x3FF;
                int ydiff = ly - cy;
                if (mosaic)
                    ydiff = ydiff - ydiff % MosSprY;

                if (attr0 & BIT(13)) // 256 colors
                {
                    tilebaseno >>= 1; // In 256 mode, they need double space

                    uint16_t *palptr = (uint16_t *)&(((uint8_t *)MEM_PALETTE_ADDR)[256 * 2]);

                    int j = (x < 0) ? 0 : x; // Search start point
                    while (j < (x + (hrealsx << 1)) && (j < 240))
                    {
                        if ((sprvisible[prio][j] == 0) || (mode == 2))
                        {
                            int xdiff = j - cx;
                            if (mosaic)
                                xdiff = xdiff - xdiff % MosSprX;

                            // Get texture coordinates (relative to center)
                            uint32_t px = (mat->pa * xdiff + mat->pb * ydiff) >> 8;
                            uint32_t py = (mat->pc * xdiff + mat->pd * ydiff) >> 8;
                            // Get texture coordinates (absolute)
                            px += hsx;
                            py += hsy;

                            // The variables are unsigned, so this also checks
                            // for negative numbers
                            if ((px < (hsx << 1)) && (py < (hsy << 1)))
                            {
                                uint32_t tileadd = 0;
                                if (REG_DISPCNT & BIT(6)) // 1D mapping
                                {
                                    int tilex = px >> 3;
                                    int tiley = py >> 3;
                                    tileadd = tilex + (tiley * (hsx * 2) / 8);
                                }
                                else // 2D mapping
                                {
                                    int tilex = px >> 3;
                                    int tiley = py >> 3;
                                    tileadd = tilex + (tiley * 16);
                                }

                                uint8_t *tile_ptr =
                                    (uint8_t *)&(((uint8_t *)MEM_VRAM_ADDR)
                                            [0x10000 + ((tilebaseno + tileadd) * 64)]);

                                int _x = px & 7;
                                int _y = py & 7;

                                uint8_t data = tile_ptr[_x + (_y * 8)];

                                if (data)
                                {
                                    if (mode == 0)
                                    {
                                        sprfb[prio][j] = palptr[data];
                                        sprvisible[prio][j] = 1;
                                    }
                                    else if (mode == 1) // Transp
                                    {
                                        sprblend[prio][j] = 1;
                                        sprblendfb[prio][j] = palptr[data];
                                        sprfb[prio][j] = palptr[data];
                                        sprvisible[prio][j] = 1;
                                    }
                                    else if (mode == 2) // 3 = prohibited
                                    {
                                        sprwin[j] = 1;
                                    }
                                }
                            }
                        }
                        j++;
                    }
                }
                else // 16 colors
                {
                    uint16_t palno = attr2 >> 12;
                    uint16_t *palptr = (uint16_t *)&((uint8_t *)MEM_PALETTE_ADDR)[512 + (palno * 32)];

                    int j = (x < 0) ? 0 : x; // Search start point
                    while (j < (x + (hrealsx << 1)) && (j < 240))
                    {
                        if ((sprvisible[prio][j] == 0) || (mode == 2))
                        {
                            int xdiff = j - cx;
                            if (mosaic)
                                xdiff = xdiff - xdiff % MosSprX;

                            // Get texture coordinates (relative to center)
                            uint32_t px = (mat->pa * xdiff + mat->pb * ydiff) >> 8;
                            uint32_t py = (mat->pc * xdiff + mat->pd * ydiff) >> 8;
                            // Get texture coordinates (absolute)
                            px += hsx;
                            py += hsy;

                            // The variables are unsigned, so this also checks
                            // for negative numbers
                            if ((px < (hsx << 1)) && (py < (hsy << 1)))
                            {
                                uint32_t tileadd = 0;
                                if (REG_DISPCNT & BIT(6)) // 1D mapping
                                {
                                    int tilex = px >> 3;
                                    int tiley = py >> 3;
                                    tileadd = tilex + (tiley * (hsx * 2) / 8);
                                }
                                else // 2D mapping
                                {
                                    int tilex = px >> 3;
                                    int tiley = py >> 3;
                                    tileadd = tilex + (tiley * 32);
                                }

                                uint8_t *tile_ptr =
                                    (uint8_t *)&(((uint8_t *)MEM_VRAM_ADDR)[0x10000 + ((tilebaseno + tileadd) * 32)]);

                                int _x = px & 7;
                                int _y = py & 7;

                                uint8_t data = tile_ptr[(_x / 2) + (_y * 4)];

                                if (_x & 1)
                                    data = data >> 4;
                                else
                                    data = data & 0xF;

                                if (data)
                                {
                                    if (mode == 0)
                                    {
                                        sprfb[prio][j] = palptr[data];
                                        sprvisible[prio][j] = 1;
                                    }
                                    else if (mode == 1) // Transp
                                    {
                                        sprblend[prio][j] = 1;
                                        sprblendfb[prio][j] = palptr[data];
                                        sprfb[prio][j] = palptr[data];
                                        sprvisible[prio][j] = 1;
                                    }
                                    else if (mode == 2) // 3 = prohibited
                                    {
                                        sprwin[j] = 1;
                                    }
                                }
                            }
                        }
                        j++;
                    }
                }
            }
        }
        else // Regular sprite
        {
            if ((attr0 & BIT(9)) == 0) // Displayed
            {
                int mosaic = attr0 & BIT(12);

                uint16_t attr1 = spr->attr1;
                uint16_t attr2 = spr->attr2;

                uint16_t shape = attr0 >> 14;
                uint16_t size = attr1 >> 14;
                int sx = spr_size[shape][size][0];
                int sy = spr_size[shape][size][1];

                int y = (attr0 & 0xFF);
                y |= (y < 160) ? 0 : 0xFFFFFF00;
                int x = (int)(attr1 & 0x1FF)
                        | ((attr1 & BIT(8)) ? 0xFFFFFE00 : 0);

                if ((y <= ly) && ((y + sy) > ly)) // If in this line
                {
                    int mode = (attr0 >> 10) & 3;

                    int ydiff = ly - y;

                    if (attr1 & BIT(13))
                        ydiff = sy - ydiff - 1; // V flip

                    if (mosaic)
                        ydiff = ydiff - ydiff % MosSprY;

                    uint16_t prio = (attr2 >> 10) & 3;
                    uint16_t tilebaseno = attr2 & 0x3FF;

                    if (attr0 & BIT(13)) // 256 colors
                    {
                        tilebaseno >>= 1; // In 256 mode, they need double space

                        uint16_t *palptr = (uint16_t *)&(((uint8_t *)MEM_PALETTE_ADDR)[256 * 2]);

                        int j = (x < 0) ? 0 : x; // Search start point
                        while (j < (x + sx) && (j < 240))
                        {
                            if ((sprvisible[prio][j] == 0) || (mode == 2))
                            {
                                int xdiff = j - x;

                                if (attr1 & BIT(12))
                                    xdiff = sx - xdiff - 1; // H flip

                                if (mosaic)
                                    xdiff = xdiff - xdiff % MosSprX;

                                uint32_t tileadd = 0;
                                if (REG_DISPCNT & BIT(6)) // 1D mapping
                                {
                                    int tilex = xdiff >> 3;
                                    int tiley = ydiff >> 3;
                                    tileadd = tilex + (tiley * sx / 8);
                                }
                                else // 2D mapping
                                {
                                    int tilex = xdiff >> 3;
                                    int tiley = ydiff >> 3;
                                    tileadd = tilex + (tiley * 16);
                                }

                                uint32_t tileindex = tilebaseno + tileadd;
                                uint8_t *tile_ptr =
                                    (uint8_t *)&(((uint8_t *)MEM_VRAM_ADDR)[0x10000 + (tileindex * 64)]);

                                int _x = xdiff & 7;
                                int _y = ydiff & 7;

                                uint8_t data = tile_ptr[_x + (_y * 8)];

                                if (data)
                                {
                                    if (mode == 0)
                                    {
                                        sprfb[prio][j] = palptr[data];
                                        sprvisible[prio][j] = 1;
                                    }
                                    else if (mode == 1) // Transp
                                    {
                                        sprblend[prio][j] = 1;
                                        sprblendfb[prio][j] = palptr[data];
                                        sprfb[prio][j] = palptr[data];
                                        sprvisible[prio][j] = 1;
                                    }
                                    else if (mode == 2) // 3 = prohibited
                                    {
                                        sprwin[j] = 1;
                                    }
                                }
                            }
                            j++;
                        }
                    }
                    else // 16 colors
                    {
                        uint16_t palno = attr2 >> 12;
                        uint16_t *palptr = (uint16_t *)&((uint8_t *)MEM_PALETTE_ADDR)[512 + (palno * 32)];

                        int j = (x < 0) ? 0 : x; // Search start point
                        while (j < (x + sx) && (j < 240))
                        {
                            if ((sprvisible[prio][j] == 0) || (mode == 2))
                            {
                                int xdiff = j - x;

                                if (attr1 & BIT(12))
                                    xdiff = sx - xdiff - 1; // H flip

                                if (mosaic)
                                    xdiff = xdiff - xdiff % MosSprX;

                                uint32_t tileadd = 0;
                                if (REG_DISPCNT & BIT(6)) // 1D mapping
                                {
                                    int tilex = xdiff >> 3;
                                    int tiley = ydiff >> 3;
                                    tileadd = tilex + (tiley * sx / 8);
                                }
                                else // 2D mapping
                                {
                                    int tilex = xdiff >> 3;
                                    int tiley = ydiff >> 3;
                                    tileadd = tilex + (tiley * 32);
                                }

                                uint32_t tileindex = tilebaseno + tileadd;
                                uint8_t *tile_ptr =
                                    (uint8_t *)&(((uint8_t *)MEM_VRAM_ADDR)[0x10000 + (tileindex * 32)]);

                                int _x = xdiff & 7;
                                int _y = ydiff & 7;

                                uint8_t data = tile_ptr[(_x / 2) + (_y * 4)];

                                if (_x & 1)
                                    data = data >> 4;
                                else
                                    data = data & 0xF;

                                if (data)
                                {
                                    if (mode == 0)
                                    {
                                        sprfb[prio][j] = palptr[data];
                                        sprvisible[prio][j] = 1;
                                    }
                                    else if (mode == 1) // Transp
                                    {
                                        sprblend[prio][j] = 1;
                                        sprblendfb[prio][j] = palptr[data];
                                        sprfb[prio][j] = palptr[data];
                                        sprvisible[prio][j] = 1;
                                    }
                                    else if (mode == 2) // 3 = prohibited
                                    {
                                        sprwin[j] = 1;
                                    }
                                }
                            }
                            j++;
                        }
                    }
                }
            }
        }
        spr++;
    }
}

static void gba_sprites_draw_mode345(int32_t ly)
{
    oam_entry_t *spr = (oam_entry_t *)((uint8_t *)MEM_OAM_ADDR);

    for (int i = 0; i < 128; i++)
    {
        uint16_t attr0 = spr->attr0;

        if (attr0 & BIT(8)) // Affine sprite -- No H flip or V flip
        {
            int mosaic = attr0 & BIT(12);

            uint16_t attr1 = spr->attr1;
            uint16_t attr2 = spr->attr2;

            oam_matrix_entry_t *mat =
                    &(((oam_matrix_entry_t *)((uint8_t *)MEM_OAM_ADDR))[(attr1 >> 9) & 0x1F]);

            uint16_t shape = attr0 >> 14;
            uint16_t size = attr1 >> 14;
            uint32_t hsx = spr_size[shape][size][0] >> 1; // Half size
            uint32_t hsy = spr_size[shape][size][1] >> 1;

            int y = (attr0 & 0xFF);
            y |= (y < 160) ? 0 : 0xFFFFFF00;
            int x = (int)(attr1 & 0x1FF) | ((attr1 & BIT(8)) ? 0xFFFFFE00 : 0);

            // Is double size?
            int hrealsx = (attr0 & BIT(9)) ? hsx << 1 : hsx; // Half canvas size
            int hrealsy = (attr0 & BIT(9)) ? hsy << 1 : hsy;

            int cx = x + hrealsx; // Center of the sprite
            int cy = y + hrealsy;

            if ((y <= ly) && ((y + (hrealsy << 1)) > ly)) // If in this line
            {
                int mode = (attr0 >> 10) & 3;

                uint16_t prio = (attr2 >> 10) & 3;
                uint16_t tilebaseno = attr2 & 0x3FF;
                int ydiff = ly - cy;
                if (mosaic)
                    ydiff = ydiff - ydiff % MosSprY;

                if (attr0 & BIT(13)) // 256 colors
                {
                    tilebaseno >>= 1; // In 256 mode, they need double space

                    uint16_t *palptr = (uint16_t *)&(((uint8_t *)MEM_PALETTE_ADDR)[256 * 2]);

                    int j = (x < 0) ? 0 : x; // Search start point
                    while (j < (x + (hrealsx << 1)) && (j < 240))
                    {
                        if ((sprvisible[prio][j] == 0) || (mode == 2))
                        {
                            int xdiff = j - cx;
                            if (mosaic)
                                xdiff = xdiff - xdiff % MosSprX;

                            // Get texture coordinates (relative to center)
                            uint32_t px = (mat->pa * xdiff + mat->pb * ydiff) >> 8;
                            uint32_t py = (mat->pc * xdiff + mat->pd * ydiff) >> 8;
                            // Get texture coordinates (absolute)
                            px += hsx;
                            py += hsy;

                            // The variables are unsigned, so this also checks
                            // for negative numbers
                            if ((px < (hsx << 1)) && (py < (hsy << 1)))
                            {
                                uint32_t tileadd = 0;
                                if (REG_DISPCNT & BIT(6)) // 1D mapping
                                {
                                    int tilex = px >> 3;
                                    int tiley = py >> 3;
                                    tileadd = tilex + (tiley * (hsx * 2) / 8);
                                }
                                else // 2D mapping
                                {
                                    int tilex = px >> 3;
                                    int tiley = py >> 3;
                                    tileadd = tilex + (tiley * 16);
                                }

                                // Each tile needs double space
                                if (tilebaseno + tileadd >= 256)
                                {
                                    uint8_t *tile_ptr =
                                        (uint8_t *)&(((uint8_t *)MEM_VRAM_ADDR)[0x10000 + ((tilebaseno + tileadd) * 64)]);

                                    int _x = px & 7;
                                    int _y = py & 7;

                                    uint8_t data = tile_ptr[_x + (_y * 8)];

                                    if (data)
                                    {
                                        if (mode == 0)
                                        {
                                            sprfb[prio][j] = palptr[data];
                                            sprvisible[prio][j] = 1;
                                        }
                                        else if (mode == 1) // Transp
                                        {
                                            sprblend[prio][j] = 1;
                                            sprblendfb[prio][j] = palptr[data];
                                            sprfb[prio][j] = palptr[data];
                                            sprvisible[prio][j] = 1;
                                        }
                                        else if (mode == 2) // 3 = prohibited
                                        {
                                            sprwin[j] = 1;
                                        }
                                    }
                                }
                            }
                        }
                        j++;
                    }
                }
                else // 16 colors
                {
                    uint16_t palno = attr2 >> 12;
                    uint16_t *palptr = (uint16_t *)&((uint8_t *)MEM_PALETTE_ADDR)[512 + (palno * 32)];

                    int j = (x < 0) ? 0 : x; // Search start point
                    while (j < (x + (hrealsx << 1)) && (j < 240))
                    {
                        if ((sprvisible[prio][j] == 0) || (mode == 2))
                        {
                            int xdiff = j - cx;
                            if (mosaic)
                                xdiff = xdiff - xdiff % MosSprX;

                            // Get texture coordinates (relative to center)
                            uint32_t px = (mat->pa * xdiff + mat->pb * ydiff) >> 8;
                            uint32_t py = (mat->pc * xdiff + mat->pd * ydiff) >> 8;
                            // Get texture coordinates (absolute)
                            px += hsx;
                            py += hsy;

                            // The variables are unsigned, so this also checks
                            // for negative numbers
                            if ((px < (hsx << 1)) && (py < (hsy << 1)))
                            {
                                uint32_t tileadd = 0;
                                if (REG_DISPCNT & BIT(6)) // 1D mapping
                                {
                                    int tilex = px >> 3;
                                    int tiley = py >> 3;
                                    tileadd = tilex + (tiley * (hsx * 2) / 8);
                                }
                                else // 2D mapping
                                {
                                    int tilex = px >> 3;
                                    int tiley = py >> 3;
                                    tileadd = tilex + (tiley * 32);
                                }

                                if (tilebaseno + tileadd >= 512)
                                {
                                    uint8_t *tile_ptr = (uint8_t *)&(((uint8_t *)MEM_VRAM_ADDR)[0x10000
                                            + ((tilebaseno + tileadd) * 32)]);

                                    int _x = px & 7;
                                    int _y = py & 7;

                                    uint8_t data = tile_ptr[(_x / 2) + (_y * 4)];

                                    if (_x & 1)
                                        data = data >> 4;
                                    else
                                        data = data & 0xF;

                                    if (data)
                                    {
                                        if (mode == 0)
                                        {
                                            sprfb[prio][j] = palptr[data];
                                            sprvisible[prio][j] = 1;
                                        }
                                        else if (mode == 1) // Transp
                                        {
                                            sprblend[prio][j] = 1;
                                            sprblendfb[prio][j] = palptr[data];
                                            sprfb[prio][j] = palptr[data];
                                            sprvisible[prio][j] = 1;
                                        }
                                        else if (mode == 2) // 3 = prohibited
                                        {
                                            sprwin[j] = 1;
                                        }
                                    }
                                }
                            }
                        }
                        j++;
                    }
                }
            }
        }
        else // Regular sprite
        {
            if ((attr0 & BIT(9)) == 0) // Displayed
            {
                int mosaic = attr0 & BIT(12);

                uint16_t attr1 = spr->attr1;
                uint16_t attr2 = spr->attr2;

                uint16_t shape = attr0 >> 14;
                uint16_t size = attr1 >> 14;
                int sx = spr_size[shape][size][0];
                int sy = spr_size[shape][size][1];

                int y = (attr0 & 0xFF);
                y |= (y < 160) ? 0 : 0xFFFFFF00;
                int x = (int)(attr1 & 0x1FF)
                        | ((attr1 & BIT(8)) ? 0xFFFFFE00 : 0);

                if ((y <= ly) && ((y + sy) > ly)) // If in this line
                {
                    int mode = (attr0 >> 10) & 3;

                    int ydiff = ly - y;

                    if (attr1 & BIT(13))
                        ydiff = sy - ydiff - 1; // V flip

                    if (mosaic)
                        ydiff = ydiff - ydiff % MosSprY;

                    uint16_t prio = (attr2 >> 10) & 3;
                    uint16_t tilebaseno = attr2 & 0x3FF;

                    if (attr0 & BIT(13)) // 256 colors
                    {
                        tilebaseno >>= 1; // in 256 mode, they need double space

                        uint16_t *palptr = (uint16_t *)&(((uint8_t *)MEM_PALETTE_ADDR)[256 * 2]);

                        int j = (x < 0) ? 0 : x; // Search start point
                        while (j < (x + sx) && (j < 240))
                        {
                            if ((sprvisible[prio][j] == 0) || (mode == 2))
                            {
                                int xdiff = j - x;

                                if (attr1 & BIT(12))
                                    xdiff = sx - xdiff - 1; // H flip

                                if (mosaic)
                                    xdiff = xdiff - xdiff % MosSprX;

                                uint32_t tileadd = 0;
                                if (REG_DISPCNT & BIT(6)) // 1D mapping
                                {
                                    int tilex = xdiff >> 3;
                                    int tiley = ydiff >> 3;
                                    tileadd = tilex + (tiley * sx / 8);
                                }
                                else // 2D mapping
                                {
                                    int tilex = xdiff >> 3;
                                    int tiley = ydiff >> 3;
                                    tileadd = tilex + (tiley * 16);
                                }

                                uint32_t tileindex = tilebaseno + tileadd;

                                // Each tile needs double space
                                if (tileindex >= 256)
                                {
                                    uint8_t *tile_ptr = (uint8_t *)&(((uint8_t *)MEM_VRAM_ADDR)[0x10000
                                            + (tileindex * 64)]);

                                    int _x = xdiff & 7;
                                    int _y = ydiff & 7;

                                    uint8_t data = tile_ptr[_x + (_y * 8)];

                                    if (data)
                                    {
                                        if (mode == 0)
                                        {
                                            sprfb[prio][j] = palptr[data];
                                            sprvisible[prio][j] = 1;
                                        }
                                        else if (mode == 1) // Transp
                                        {
                                            sprblend[prio][j] = 1;
                                            sprblendfb[prio][j] = palptr[data];
                                            sprfb[prio][j] = palptr[data];
                                            sprvisible[prio][j] = 1;
                                        }
                                        else if (mode == 2) // 3 = prohibited
                                        {
                                            sprwin[j] = 1;
                                        }
                                    }
                                }
                            }
                            j++;
                        }
                    }
                    else // 16 colors
                    {
                        uint16_t palno = attr2 >> 12;
                        uint16_t *palptr = (uint16_t *)&((uint8_t *)MEM_PALETTE_ADDR)[512 + (palno * 32)];

                        int j = (x < 0) ? 0 : x; // Search start point
                        while (j < (x + sx) && (j < 240))
                        {
                            if ((sprvisible[prio][j] == 0) || (mode == 2))
                            {
                                int xdiff = j - x;

                                if (attr1 & BIT(12))
                                    xdiff = sx - xdiff - 1; // H flip

                                if (mosaic)
                                    xdiff = xdiff - xdiff % MosSprX;

                                uint32_t tileadd = 0;
                                if (REG_DISPCNT & BIT(6)) // 1D mapping
                                {
                                    int tilex = xdiff >> 3;
                                    int tiley = ydiff >> 3;
                                    tileadd = tilex + (tiley * sx / 8);
                                }
                                else // 2D mapping
                                {
                                    int tilex = xdiff >> 3;
                                    int tiley = ydiff >> 3;
                                    tileadd = tilex + (tiley * 32);
                                }

                                uint32_t tileindex = tilebaseno + tileadd;

                                if (tileindex >= 512)
                                {
                                    uint8_t *tile_ptr = (uint8_t *)&(((uint8_t *)MEM_VRAM_ADDR)[0x10000
                                            + (tileindex * 32)]);

                                    int _x = xdiff & 7;
                                    int _y = ydiff & 7;

                                    uint8_t data = tile_ptr[(_x / 2) + (_y * 4)];

                                    if (_x & 1)
                                        data = data >> 4;
                                    else
                                        data = data & 0xF;

                                    if (data)
                                    {
                                        if (mode == 0)
                                        {
                                            sprfb[prio][j] = palptr[data];
                                            sprvisible[prio][j] = 1;
                                        }
                                        else if (mode == 1) // Transp
                                        {
                                            sprblend[prio][j] = 1;
                                            sprblendfb[prio][j] = palptr[data];
                                            sprfb[prio][j] = palptr[data];
                                            sprvisible[prio][j] = 1;
                                        }
                                        else if (mode == 2) // 3 = prohibited
                                        {
                                            sprwin[j] = 1;
                                        }
                                    }
                                }
                            }
                            j++;
                        }
                    }
                }
            }
        }
        spr++;
    }
}

//------------------------------------------------------------------------------

uint16_t bgfb[4][240];
int bgvisible[4][240];
uint16_t backdrop[240];
int backdropvisible[240]; // This array is filled in GBA_FillFadeTables()

static const uint32_t text_bg_size[4][2] = {
    { 256, 256 }, { 512, 256 }, { 256, 512 }, { 512, 512 }
};

static uint32_t se_index(uint32_t tx, uint32_t ty, uint32_t pitch) // From tonc
{
    uint32_t sbb = (ty / 32) * (pitch / 32) + (tx / 32);
    return sbb * 1024 + (ty % 32) * 32 + tx % 32;
}

static void gba_bg0drawtext(int32_t y)
{
    int sx = REG_BG0HOFS;
    int sy = REG_BG0VOFS;
    uint16_t control = REG_BG0CNT;

    uint8_t *charbaseblockptr = (uint8_t *)&((uint8_t *)MEM_VRAM_ADDR)[((control >> 2) & 3) * (16 * 1024)];
    uint16_t *scrbaseblockptr = (uint16_t *)&((uint8_t *)MEM_VRAM_ADDR)[((control >> 8) & 0x1F) * (2 * 1024)];

    uint32_t maskx = text_bg_size[control >> 14][0] - 1;
    uint32_t masky = text_bg_size[control >> 14][1] - 1;

    uint32_t startx = sx & maskx;
    uint32_t starty = (y + sy) & masky;

    uint32_t sizex = text_bg_size[control >> 14][0] / 8;

    int mosaic = (control & BIT(6)); // Mosaic
    if (mosaic)
        starty -= starty % MosBgY;

    uint16_t *fb = bgfb[0];
    int *visptr = bgvisible[0];
    if (control & BIT(7)) // 256 colors
    {
        for (int i = 0; i < 240; i++)
        {
            startx = (sx + i) & maskx;
            if (mosaic)
                startx -= startx % MosBgX;

            uint32_t index = se_index(startx / 8, starty / 8, sizex);
            uint16_t SE = scrbaseblockptr[index];
            // Screen entry data:
            // 0-9 tile id
            // 10-hflip
            // 11-vflip
            int _x = startx & 7;
            if (SE & BIT(10))
                _x = 7 - _x; // H flip

            int _y = starty & 7;
            if (SE & BIT(11))
                _y = 7 - _y; // V flip

            int data = charbaseblockptr[((SE & 0x3FF) * 64) + (_x + (_y * 8))];

            *fb++ = ((uint16_t *)((uint8_t *)MEM_PALETTE_ADDR))[data];
            *visptr++ = data;

            //startx = (startx + 1) & maskx;
        }
    }
    else // 16 colors
    {
        for (int i = 0; i < 240; i++)
        {
            startx = (sx + i) & maskx;
            if (mosaic)
                startx -= startx % MosBgX;

            uint32_t index = se_index(startx / 8, starty / 8, sizex);
            uint16_t SE = scrbaseblockptr[index];
            // Screen entry data:
            // 0-9 tile id
            // 10-hflip
            // 11-vflip
            // 12-15-pal
            uint16_t *palptr = (uint16_t *)&((uint8_t *)MEM_PALETTE_ADDR)[(SE >> 12) * (2 * 16)];

            int _x = startx & 7;
            if (SE & BIT(10))
                _x = 7 - _x; // H flip

            int _y = starty & 7;
            if (SE & BIT(11))
                _y = 7 - _y; // V flip

            uint32_t data =
                    charbaseblockptr[((SE & 0x3FF) * 32) + ((_x / 2) + (_y * 4))];

            if (_x & 1)
                data = data >> 4;
            else
                data = data & 0xF;

            *fb++ = palptr[data];
            *visptr++ = data;

            //startx = (startx + 1) & maskx;
        }
    }
}

static void gba_bg1drawtext(int32_t y)
{
    int sx = REG_BG1HOFS;
    int sy = REG_BG1VOFS;
    uint16_t control = REG_BG1CNT;

    uint8_t *charbaseblockptr = (uint8_t *)&((uint8_t *)MEM_VRAM_ADDR)[((control >> 2) & 3) * (16 * 1024)];
    uint16_t *scrbaseblockptr =
            (uint16_t *)&((uint8_t *)MEM_VRAM_ADDR)[((control >> 8) & 0x1F) * (2 * 1024)];

    uint32_t maskx = text_bg_size[control >> 14][0] - 1;
    uint32_t masky = text_bg_size[control >> 14][1] - 1;

    uint32_t startx = sx & maskx;
    uint32_t starty = (y + sy) & masky;

    uint32_t sizex = text_bg_size[control >> 14][0] / 8;

    int mosaic = (control & BIT(6)); // Mosaic
    if (mosaic)
        starty -= starty % MosBgY;

    uint16_t *fb = bgfb[1];
    int *visptr = bgvisible[1];
    if (control & BIT(7)) // 256 colors
    {
        for (int i = 0; i < 240; i++)
        {
            startx = (sx + i) & maskx;
            if (mosaic)
                startx -= startx % MosBgX;

            uint32_t index = se_index(startx / 8, starty / 8, sizex);
            uint16_t SE = scrbaseblockptr[index];
            // Screen entry data:
            // 0-9 tile id
            // 10-hflip
            // 11-vflip
            int _x = startx & 7;
            if (SE & BIT(10))
                _x = 7 - _x; // H flip

            int _y = starty & 7;
            if (SE & BIT(11))
                _y = 7 - _y; // V flip

            int data = charbaseblockptr[((SE & 0x3FF) * 64) + (_x + (_y * 8))];

            *fb++ = ((uint16_t *)((uint8_t *)MEM_PALETTE_ADDR))[data];
            *visptr++ = data;

            //startx = (startx + 1) & maskx;
        }
    }
    else // 16 colors
    {
        for (int i = 0; i < 240; i++)
        {
            startx = (sx + i) & maskx;
            if (mosaic)
                startx -= startx % MosBgX;

            uint32_t index = se_index(startx / 8, starty / 8, sizex);
            uint16_t SE = scrbaseblockptr[index];
            // Screen entry data:
            // 0-9 tile id
            // 10-hflip
            // 11-vflip
            // 12-15-pal
            uint16_t *palptr = (uint16_t *)&((uint8_t *)MEM_PALETTE_ADDR)[(SE >> 12) * (2 * 16)];

            int _x = startx & 7;
            if (SE & BIT(10))
                _x = 7 - _x; // H flip

            int _y = starty & 7;
            if (SE & BIT(11))
                _y = 7 - _y; // V flip

            uint32_t data =
                    charbaseblockptr[((SE & 0x3FF) * 32) + ((_x / 2) + (_y * 4))];

            if (_x & 1)
                data = data >> 4;
            else
                data = data & 0xF;

            *fb++ = palptr[data];
            *visptr++ = data;

            //startx = (startx + 1) & maskx;
        }
    }
}

static void gba_bg2drawtext(int32_t y)
{
    int sx = REG_BG2HOFS;
    int sy = REG_BG2VOFS;
    uint16_t control = REG_BG2CNT;

    uint8_t *charbaseblockptr = (uint8_t *)&((uint8_t *)MEM_VRAM_ADDR)[((control >> 2) & 3) * (16 * 1024)];
    uint16_t *scrbaseblockptr =
            (uint16_t *)&((uint8_t *)MEM_VRAM_ADDR)[((control >> 8) & 0x1F) * (2 * 1024)];

    uint32_t maskx = text_bg_size[control >> 14][0] - 1;
    uint32_t masky = text_bg_size[control >> 14][1] - 1;

    uint32_t startx = sx & maskx;
    uint32_t starty = (y + sy) & masky;

    uint32_t sizex = text_bg_size[control >> 14][0] / 8;

    int mosaic = (control & BIT(6)); // Mosaic
    if (mosaic)
        starty -= starty % MosBgY;

    uint16_t *fb = bgfb[2];
    int *visptr = bgvisible[2];
    if (control & BIT(7)) // 256 colors
    {
        for (int i = 0; i < 240; i++)
        {
            startx = (sx + i) & maskx;
            if (mosaic)
                startx -= startx % MosBgX;

            uint32_t index = se_index(startx / 8, starty / 8, sizex);
            uint16_t SE = scrbaseblockptr[index];
            // Screen entry data:
            // 0-9 tile id
            // 10-hflip
            // 11-vflip
            int _x = startx & 7;
            if (mosaic)
                _x = (startx - startx % MosBgX) & 7;
            if (SE & BIT(10))
                _x = 7 - _x; // H flip

            int _y = starty & 7;
            if (SE & BIT(11))
                _y = 7 - _y; // V flip

            int data = charbaseblockptr[((SE & 0x3FF) * 64) + (_x + (_y * 8))];

            *fb++ = ((uint16_t *)((uint8_t *)MEM_PALETTE_ADDR))[data];
            *visptr++ = data;

            //startx = (startx + 1) & maskx;
        }
    }
    else // 16 colors
    {
        for (int i = 0; i < 240; i++)
        {
            startx = (sx + i) & maskx;
            if (mosaic)
                startx -= startx % MosBgX;

            uint32_t index = se_index(startx / 8, starty / 8, sizex);
            uint16_t SE = scrbaseblockptr[index];
            // Screen entry data
            // 0-9 tile id
            // 10-hflip
            // 11-vflip
            // 12-15-pal
            uint16_t *palptr = (uint16_t *)&((uint8_t *)MEM_PALETTE_ADDR)[(SE >> 12) * (2 * 16)];

            int _x = startx & 7;
            if (SE & BIT(10))
                _x = 7 - _x; // H flip

            int _y = starty & 7;
            if (SE & BIT(11))
                _y = 7 - _y; // V flip

            uint32_t data =
                    charbaseblockptr[((SE & 0x3FF) * 32) + ((_x / 2) + (_y * 4))];

            if (_x & 1)
                data = data >> 4;
            else
                data = data & 0xF;

            *fb++ = palptr[data];
            *visptr++ = data;

            //startx = (startx + 1) & maskx;
        }
    }
}

static void gba_bg3drawtext(int32_t y)
{
    int sx = REG_BG3HOFS;
    int sy = REG_BG3VOFS;
    uint16_t control = REG_BG3CNT;

    uint8_t *charbaseblockptr = (uint8_t *)&((uint8_t *)MEM_VRAM_ADDR)[((control >> 2) & 3) * (16 * 1024)];
    uint16_t *scrbaseblockptr =
            (uint16_t *)&((uint8_t *)MEM_VRAM_ADDR)[((control >> 8) & 0x1F) * (2 * 1024)];

    uint32_t maskx = text_bg_size[control >> 14][0] - 1;
    uint32_t masky = text_bg_size[control >> 14][1] - 1;

    uint32_t startx = sx & maskx;
    uint32_t starty = (y + sy) & masky;

    uint32_t sizex = text_bg_size[control >> 14][0] / 8;

    int mosaic = (control & BIT(6)); // Mosaic
    if (mosaic)
        starty -= starty % MosBgY;

    uint16_t *fb = bgfb[3];
    int *visptr = bgvisible[3];
    if (control & BIT(7)) // 256 colors
    {
        for (int i = 0; i < 240; i++)
        {
            startx = (sx + i) & maskx;
            if (mosaic)
                startx -= startx % MosBgX;

            uint32_t index = se_index(startx / 8, starty / 8, sizex);
            uint16_t SE = scrbaseblockptr[index];
            // Screen entry data:
            // 0-9 tile id
            // 10-hflip
            // 11-vflip
            int _x = startx & 7;
            if (SE & BIT(10))
                _x = 7 - _x; // H flip

            int _y = starty & 7;
            if (SE & BIT(11))
                _y = 7 - _y; // V flip

            int data = charbaseblockptr[((SE & 0x3FF) * 64) + (_x + (_y * 8))];

            *fb++ = ((uint16_t *)((uint8_t *)MEM_PALETTE_ADDR))[data];
            *visptr++ = data;

            //startx = (startx + 1) & maskx;
        }
    }
    else // 16 colors
    {
        for (int i = 0; i < 240; i++)
        {
            startx = (sx + i) & maskx;
            if (mosaic)
                startx -= startx % MosBgX;

            uint32_t index = se_index(startx / 8, starty / 8, sizex);
            uint16_t SE = scrbaseblockptr[index];
            // Screen entry data:
            // 0-9 tile id
            // 10-hflip
            // 11-vflip
            // 12-15-pal
            uint16_t *palptr = (uint16_t *)&((uint8_t *)MEM_PALETTE_ADDR)[(SE >> 12) * (2 * 16)];

            int _x = startx & 7;
            if (SE & BIT(10))
                _x = 7 - _x; // H flip

            int _y = starty & 7;
            if (SE & BIT(11))
                _y = 7 - _y; // V flip

            uint32_t data =
                    charbaseblockptr[((SE & 0x3FF) * 32) + ((_x / 2) + (_y * 4))];

            if (_x & 1)
                data = data >> 4;
            else
                data = data & 0xF;

            *fb++ = palptr[data];
            *visptr++ = data;

            //startx = (startx + 1) & maskx;
        }
    }
}

//------------------------------------------------------------------------------

static uint32_t se_index_affine(uint32_t tx, uint32_t ty, uint32_t tpitch)
{
    return (ty * tpitch) + tx;
}

static const uint32_t affine_bg_size[4] = {
    128, 256, 512, 1024
};

static int32_t mosBG2lastx, mosBG2lasty, mos2A, mos2C;

static void gba_bg2drawaffine(int32_t y)
{
    uint16_t control = REG_BG2CNT;

    uint8_t *charbaseblockptr = (uint8_t *)&((uint8_t *)MEM_VRAM_ADDR)[((control >> 2) & 3) * (16 * 1024)];
    uint8_t *scrbaseblockptr = (uint8_t *)&((uint8_t *)MEM_VRAM_ADDR)[((control >> 8) & 0x1F) * (2 * 1024)];

    uint32_t size = affine_bg_size[control >> 14];
    uint32_t sizemask = size - 1;
    uint32_t tilesize = size / 8;

    int32_t currx = BG2lastx;
    int32_t curry = BG2lasty;

    // | PA PB |
    // | PC PD |

    int32_t A = (int32_t)(int16_t)REG_BG2PA;
    int32_t C = (int32_t)(int16_t)REG_BG2PC;

    uint16_t *fb = bgfb[2];
    int *visptr = bgvisible[2];

    int mosaic = (control & BIT(6)); // Mosaic

    if (mosaic)
    {
        if (y % MosBgY == 0)
        {
            mosBG2lastx = BG2lastx;
            mosBG2lasty = BG2lasty;
            mos2A = A;
            mos2C = C;
        }
        else
        {
            currx = mosBG2lastx;
            curry = mosBG2lasty;
            A = mos2A;
            C = mos2C;
        }
    }

    uint8_t data = 0;
    for (int i = 0; i < 240; i++) // Always 256 colors
    {
        uint32_t _x = (currx >> 8);
        uint32_t _y = (curry >> 8);

        if (!mosaic || ((i % MosBgX) == 0))
        {
            data = 0;
            if (control & BIT(13)) // Wrap
            {
                _x &= sizemask;
                _y &= sizemask;

                int __x = _x & 7;
                int __y = _y & 7;

                uint32_t index = se_index_affine(_x / 8, _y / 8, tilesize);
                uint8_t SE = scrbaseblockptr[index];
                data = charbaseblockptr[(SE * 64) + (__x + (__y * 8))];
            }
            else if ((_x < size) && (_y < size))
            {
                int __x = _x & 7;
                int __y = _y & 7;

                uint32_t index = se_index_affine(_x / 8, _y / 8, tilesize);
                uint8_t SE = scrbaseblockptr[index];
                data = charbaseblockptr[(SE * 64) + (__x + (__y * 8))];
            }
        }
        *fb++ = ((uint16_t *)((uint8_t *)MEM_PALETTE_ADDR))[data];
        *visptr++ = data;

        currx += A;
        curry += C;
    }
}

static int32_t mosBG3lastx, mosBG3lasty, mos3A, mos3C;

static void gba_bg3drawaffine(int32_t y)
{
    uint16_t control = REG_BG3CNT;

    uint8_t *charbaseblockptr = (uint8_t *)&((uint8_t *)MEM_VRAM_ADDR)[((control >> 2) & 3) * (16 * 1024)];
    uint8_t *scrbaseblockptr = (uint8_t *)&((uint8_t *)MEM_VRAM_ADDR)[((control >> 8) & 0x1F) * (2 * 1024)];

    uint32_t size = affine_bg_size[control >> 14];
    uint32_t sizemask = size - 1;
    uint32_t tilesize = size / 8;

    int32_t currx = BG3lastx;
    int32_t curry = BG3lasty;

    // | PA PB |
    // | PC PD |

    int32_t A = (int32_t)(int16_t)REG_BG3PA;
    int32_t C = (int32_t)(int16_t)REG_BG3PC;

    uint16_t *fb = bgfb[3];
    int *visptr = bgvisible[3];

    int mosaic = (control & BIT(6)); // Mosaic

    if (mosaic)
    {
        if (y % MosBgY == 0)
        {
            mosBG3lastx = BG3lastx;
            mosBG3lasty = BG3lasty;
            mos3A = A;
            mos3C = C;
        }
        else
        {
            currx = mosBG3lastx;
            curry = mosBG3lasty;
            A = mos3A;
            C = mos3C;
        }
    }

    uint8_t data = 0;
    for (int i = 0; i < 240; i++) // Always 256 colors
    {
        uint32_t _x = (currx >> 8);
        uint32_t _y = (curry >> 8);

        if (!mosaic || ((i % MosBgX) == 0))
        {
            data = 0;
            if (control & BIT(13)) // Wrap
            {
                _x &= sizemask;
                _y &= sizemask;

                int __x = _x & 7;
                int __y = _y & 7;

                uint32_t index = se_index_affine(_x / 8, _y / 8, tilesize);
                uint8_t SE = scrbaseblockptr[index];
                data = charbaseblockptr[(SE * 64) + (__x + (__y * 8))];
            }
            else if ((_x < size) && (_y < size))
            {
                int __x = _x & 7;
                int __y = _y & 7;

                uint32_t index = se_index_affine(_x / 8, _y / 8, tilesize);
                uint8_t SE = scrbaseblockptr[index];
                data = charbaseblockptr[(SE * 64) + (__x + (__y * 8))];
            }
        }

        *fb++ = ((uint16_t *)((uint8_t *)MEM_PALETTE_ADDR))[data];
        *visptr++ = data;

        currx += A;
        curry += C;
    }
}

//------------------------------------------------------------------------------

static void gba_bg2drawbitmapmode3(UNUSED int32_t y)
{
    int32_t currx = BG2lastx;
    int32_t curry = BG2lasty;

    uint16_t *srcptr = (uint16_t *)MEM_VRAM_ADDR;

    // | PA PB |
    // | PC PD |

    int32_t A = (int32_t)(int16_t)REG_BG2PA;
    int32_t C = (int32_t)(int16_t)REG_BG2PC;

    uint16_t *fb = bgfb[2];
    int *visptr = bgvisible[2];

    for (int i = 0; i < 240; i++)
    {
        uint32_t _x = (currx >> 8);
        uint32_t _y = (curry >> 8);
        if (!((_x > 239) || (_y > 159)))
        {
            *fb = srcptr[_x + 240 * _y];
            *visptr = 1;
        }
        fb++;
        visptr++;
        currx += A;
        curry += C;
    }
}

static void gba_bg2drawbitmapmode4(UNUSED int32_t y)
{
    int32_t currx = BG2lastx;
    int32_t curry = BG2lasty;

    uint8_t *srcptr = (uint8_t *)&((uint8_t *)MEM_VRAM_ADDR)[(REG_DISPCNT & BIT(4)) ? 0xA000 : 0];

    // | PA PB |
    // | PC PD |

    int32_t A = (int32_t)(int16_t)REG_BG2PA;
    int32_t C = (int32_t)(int16_t)REG_BG2PC;

    uint16_t *fb = bgfb[2];
    int *visptr = bgvisible[2];

    for (int i = 0; i < 240; i++)
    {
        uint32_t _x = (currx >> 8);
        uint32_t _y = (curry >> 8);
        if (!((_x > 239) || (_y > 159)))
        {
            *fb = ((uint16_t *)((uint8_t *)MEM_PALETTE_ADDR))[srcptr[_x + 240 * _y]];
            *visptr = 1;
        }
        fb++;
        visptr++;
        currx += A;
        curry += C;
    }
}

static void gba_bg2drawbitmapmode5(UNUSED int32_t y)
{
    int32_t currx = BG2lastx;
    int32_t curry = BG2lasty;

    uint16_t *srcptr = (uint16_t *)&((uint8_t *)MEM_VRAM_ADDR)[((REG_DISPCNT & BIT(4)) ? 0xA000 : 0)];

    // | PA PB |
    // | PC PD |

    int32_t A = (int32_t)(int16_t)REG_BG2PA;
    int32_t C = (int32_t)(int16_t)REG_BG2PC;

    uint16_t *fb = bgfb[2];
    int *visptr = bgvisible[2];

    for (int i = 0; i < 240; i++)
    {
        uint32_t _x = (currx >> 8);
        uint32_t _y = (curry >> 8);
        if (!((_x > 159) || (_y > 127)))
        {
            *fb = (uint16_t)srcptr[_x + 160 * _y];
            *visptr = 1;
        }
        fb++;
        visptr++;
        currx += A;
        curry += C;
    }
}

//------------------------------------------------------------------------------

static void gba_video_all_buffers_clear(void)
{
    mem_clear_32((uint32_t *)bgfb, sizeof(bgfb));
    mem_clear_32((uint32_t *)bgvisible, sizeof(bgvisible));
    mem_clear_32((uint32_t *)sprfb, sizeof(sprfb));
    mem_clear_32((uint32_t *)sprvisible, sizeof(sprvisible));
    mem_clear_32((uint32_t *)sprblend, sizeof(sprblend));
    mem_clear_32((uint32_t *)sprblendfb, sizeof(sprblendfb));
    mem_clear_32((uint32_t *)sprwin, sizeof(sprwin));
    mem_clear_32((uint32_t *)backdrop, sizeof(backdrop));
}

//------------------------------------------------------------------------------

typedef enum
{
    BG0 = 0,
    BG1,
    BG2,
    BG3,

    SPR0,
    SPR1,
    SPR2,
    SPR3,

    BD
} _layer_type_;

// layer_fb[0] goes at the bottom, layer_fb[layer_active_num - 1] at the top
static int *layer_vis[9];
static uint16_t *layer_fb[9];
static _layer_type_ layer_id[9];
static int layer_active_num;

static void gba_sort_layers(int video_mode)
{
    // 1 if the layer is active in a specific screen mode
    static const int bg0act[6] = { 1, 1, 0, 0, 0, 0 };
    static const int bg1act[6] = { 1, 1, 0, 0, 0, 0 };
    static const int bg2act[6] = { 1, 1, 1, 1, 1, 1 };
    static const int bg3act[6] = { 1, 0, 1, 0, 0, 0 };

    uint16_t cnt = REG_DISPCNT;

    int bgprio[4];
    bgprio[0] = ((cnt & BIT(8)) && bg0act[video_mode]) ? (REG_BG0CNT & 3) : -1;
    bgprio[1] = ((cnt & BIT(9)) && bg1act[video_mode]) ? (REG_BG1CNT & 3) : -1;
    bgprio[2] = ((cnt & BIT(10)) && bg2act[video_mode]) ? (REG_BG2CNT & 3) : -1;
    bgprio[3] = ((cnt & BIT(11)) && bg3act[video_mode]) ? (REG_BG3CNT & 3) : -1;

    int spren = cnt & BIT(12);

    int cur_layer = 0;

    // Backdrop

    layer_vis[cur_layer] = backdropvisible;
    layer_fb[cur_layer] = backdrop;
    layer_id[cur_layer] = BD;
    cur_layer++;

    // Priority 3

    for (int i = 3; i >= 0; i--)
    {
        if (bgprio[i] == 3)
        {
            bgprio[i] = -1;
            layer_vis[cur_layer] = bgvisible[i];
            layer_fb[cur_layer] = bgfb[i];
            layer_id[cur_layer] = BG0 + i;
            cur_layer++;
        }
    }

    if (spren)
    {
        layer_vis[cur_layer] = sprvisible[3];
        layer_fb[cur_layer] = sprfb[3];
        layer_id[cur_layer] = SPR3;
        cur_layer++;
    }

    // Priority 2

    for (int i = 3; i >= 0; i--)
    {
        if (bgprio[i] == 2)
        {
            bgprio[i] = -1;
            layer_vis[cur_layer] = bgvisible[i];
            layer_fb[cur_layer] = bgfb[i];
            layer_id[cur_layer] = BG0 + i;
            cur_layer++;
        }
    }

    if (spren)
    {
        layer_vis[cur_layer] = sprvisible[2];
        layer_fb[cur_layer] = sprfb[2];
        layer_id[cur_layer] = SPR2;
        cur_layer++;
    }

    // Priority 1

    for (int i = 3; i >= 0; i--)
    {
        if (bgprio[i] == 1)
        {
            bgprio[i] = -1;
            layer_vis[cur_layer] = bgvisible[i];
            layer_fb[cur_layer] = bgfb[i];
            layer_id[cur_layer] = BG0 + i;
            cur_layer++;
        }
    }

    if (spren)
    {
        layer_vis[cur_layer] = sprvisible[1];
        layer_fb[cur_layer] = sprfb[1];
        layer_id[cur_layer] = SPR1;
        cur_layer++;
    }

    // Priority 0

    for (int i = 3; i >= 0; i--)
    {
        if (bgprio[i] == 0)
        {
            bgprio[i] = -1;
            layer_vis[cur_layer] = bgvisible[i];
            layer_fb[cur_layer] = bgfb[i];
            layer_id[cur_layer] = BG0 + i;
            cur_layer++;
        }
    }

    if (spren)
    {
        layer_vis[cur_layer] = sprvisible[0];
        layer_fb[cur_layer] = sprfb[0];
        layer_id[cur_layer] = SPR0;
        cur_layer++;
    }

    // End

    layer_active_num = cur_layer; // Total number of active layers
}

static void gba_blit_layers(int y)
{
    uint16_t *destptr = (uint16_t *)&screen_buffer[240 * y];

    for (int i = 0; i < layer_active_num; i++)
    {
        uint16_t *dest = destptr;

        int *vis = layer_vis[i];
        uint16_t *fb = layer_fb[i];

        for (int j = 0; j < 240; j++)
        {
            if (*vis++)
                *dest = *fb;
            dest++;
            fb++;
        }
    }
}

//------------------------------------------------------------------------------

// Color effect is enabled / disabled by windows
int win_coloreffect_enable[240];

// bits 13-15 of DISPCNT
static void gba_window_apply(uint32_t y, uint32_t win0,
                             uint32_t win1, uint32_t winobj)
{
    if (!(winobj || win1 || win0))
    {
        if ((REG_BLDCNT >> 6) & 3) // Special effect
        {
            for (int i = 0; i < 240; i++)
                win_coloreffect_enable[i] = 1;
        }
        return;
    }

    uint32_t in0 = REG_WININ & 0xFF;
    uint32_t in1 = (REG_WININ >> 8) & 0xFF;
    uint32_t out = REG_WINOUT & 0xFF;
    uint32_t inobj = (REG_WINOUT >> 8) & 0xFF;

    int win_show[240];

    if (REG_DISPCNT & BIT(8))
    {
        if (out & BIT(0))
        {
            for (int i = 0; i < 240; i++)
                win_show[i] = 1;
        }
        else
        {
            memset(win_show, 0, sizeof(win_show));
        }

        if (winobj) // obj has lowest priority
        {
            int *show = win_show;
            int *ptrsprwin = sprwin;
            if (inobj & BIT(0))
            {
                for (int i = 0; i < 240; i++)
                {
                    if (*ptrsprwin)
                        *show = 1;
                    show++;
                    ptrsprwin++;
                }
            }
            else
            {
                for (int i = 0; i < 240; i++)
                {
                    if (*ptrsprwin)
                        *show = 0;
                    show++;
                    ptrsprwin++;
                }
            }
        }
        if (win1) // Intermediate priority
        {
            if (in1 & BIT(0))
            {
                if (y >= Win1Y1 && y <= Win1Y2)
                {
                    for (uint32_t i = Win1X1; i < Win1X2; i++)
                        win_show[i] = 1;
                }
            }
            else
            {
                if (y >= Win1Y1 && y <= Win1Y2)
                {
                    for (uint32_t i = Win1X1; i < Win1X2; i++)
                        win_show[i] = 0;
                }
            }
        }
        if (win0) // Highest priority
        {
            if (in0 & BIT(0))
            {
                if (y >= Win0Y1 && y <= Win0Y2)
                {
                    for (uint32_t i = Win0X1; i < Win0X2; i++)
                        win_show[i] = 1;
                }
            }
            else
            {
                if (y >= Win0Y1 && y <= Win0Y2)
                {
                    for (uint32_t i = Win0X1; i < Win0X2; i++)
                        win_show[i] = 0;
                }
            }
        }

        int *vis = bgvisible[0];
        int *show = win_show;
        for (int i = 0; i < 240; i++)
        {
            *vis = *vis && *show;
            vis++;
            show++;
        }
    }

    if (REG_DISPCNT & BIT(9))
    {
        if (out & BIT(1))
        {
            for (int i = 0; i < 240; i++)
                win_show[i] = 1;
        }
        else
        {
            memset(win_show, 0, sizeof(win_show));
        }
        if (winobj) // obj has lowest priority
        {
            int *show = win_show;
            int *ptrsprwin = sprwin;
            if (inobj & BIT(1))
            {
                for (int i = 0; i < 240; i++)
                {
                    if (*ptrsprwin)
                        *show = 1;
                    show++;
                    ptrsprwin++;
                }
            }
            else
            {
                for (int i = 0; i < 240; i++)
                {
                    if (*ptrsprwin)
                        *show = 0;
                    show++;
                    ptrsprwin++;
                }
            }
        }
        if (win1) // Intermediate priority
        {
            if (in1 & BIT(1))
            {
                if (y >= Win1Y1 && y <= Win1Y2)
                {
                    for (uint32_t i = Win1X1; i < Win1X2; i++)
                        win_show[i] = 1;
                }
            }
            else
            {
                if (y >= Win1Y1 && y <= Win1Y2)
                {
                    for (uint32_t i = Win1X1; i < Win1X2; i++)
                        win_show[i] = 0;
                }
            }
        }
        if (win0) // Highest priority
        {
            if (in0 & BIT(1))
            {
                if (y >= Win0Y1 && y <= Win0Y2)
                {
                    for (uint32_t i = Win0X1; i < Win0X2; i++)
                        win_show[i] = 1;
                }
            }
            else
            {
                if (y >= Win0Y1 && y <= Win0Y2)
                {
                    for (uint32_t i = Win0X1; i < Win0X2; i++)
                        win_show[i] = 0;
                }
            }
        }

        int *vis = bgvisible[1];
        int *show = win_show;
        for (int i = 0; i < 240; i++)
        {
            *vis = *vis && *show;
            vis++;
            show++;
        }
    }

    if (REG_DISPCNT & BIT(10))
    {
        if (out & BIT(2))
        {
            for (int i = 0; i < 240; i++)
                win_show[i] = 1;
        }
        else
        {
            memset(win_show, 0, sizeof(win_show));
        }
        if (winobj) // obj has lowest priority
        {
            int *show = win_show;
            int *ptrsprwin = sprwin;
            if (inobj & BIT(2))
            {
                for (int i = 0; i < 240; i++)
                {
                    if (*ptrsprwin)
                        *show = 1;
                    show++;
                    ptrsprwin++;
                }
            }
            else
            {
                for (int i = 0; i < 240; i++)
                {
                    if (*ptrsprwin)
                        *show = 0;
                    show++;
                    ptrsprwin++;
                }
            }
        }
        if (win1) // intermediate priority
        {
            if (in1 & BIT(2))
            {
                if (y >= Win1Y1 && y <= Win1Y2)
                {
                    for (uint32_t i = Win1X1; i < Win1X2; i++)
                        win_show[i] = 1;
                }
            }
            else
            {
                if (y >= Win1Y1 && y <= Win1Y2)
                {
                    for (uint32_t i = Win1X1; i < Win1X2; i++)
                        win_show[i] = 0;
                }
            }
        }
        if (win0) // Highest priority
        {
            if (in0 & BIT(2))
            {
                if (y >= Win0Y1 && y <= Win0Y2)
                {
                    for (uint32_t i = Win0X1; i < Win0X2; i++)
                        win_show[i] = 1;
                }
            }
            else
            {
                if (y >= Win0Y1 && y <= Win0Y2)
                {
                    for (uint32_t i = Win0X1; i < Win0X2; i++)
                        win_show[i] = 0;
                }
            }
        }

        int *vis = bgvisible[2];
        int *show = win_show;
        for (int i = 0; i < 240; i++)
        {
            *vis = *vis && *show;
            vis++;
            show++;
        }
    }

    if (REG_DISPCNT & BIT(11))
    {
        if (out & BIT(3))
        {
            for (int i = 0; i < 240; i++)
                win_show[i] = 1;
        }
        else
        {
            memset(win_show, 0, sizeof(win_show));
        }
        if (winobj) // obj has lowest priority
        {
            int *show = win_show;
            int *ptrsprwin = sprwin;
            if (inobj & BIT(3))
            {
                for (int i = 0; i < 240; i++)
                {
                    if (*ptrsprwin)
                        *show = 1;
                    show++;
                    ptrsprwin++;
                }
            }
            else
            {
                for (int i = 0; i < 240; i++)
                {
                    if (*ptrsprwin)
                        *show = 0;
                    show++;
                    ptrsprwin++;
                }
            }
        }
        if (win1) // Intermediate priority
        {
            if (in1 & BIT(3))
            {
                if (y >= Win1Y1 && y <= Win1Y2)
                {
                    for (uint32_t i = Win1X1; i < Win1X2; i++)
                        win_show[i] = 1;
                }
            }
            else
            {
                if (y >= Win1Y1 && y <= Win1Y2)
                {
                    for (uint32_t i = Win1X1; i < Win1X2; i++)
                        win_show[i] = 0;
                }
            }
        }
        if (win0) // Highest priority
        {
            if (in0 & BIT(3))
            {
                if (y >= Win0Y1 && y <= Win0Y2)
                {
                    for (uint32_t i = Win0X1; i < Win0X2; i++)
                        win_show[i] = 1;
                }
            }
            else
            {
                if (y >= Win0Y1 && y <= Win0Y2)
                {
                    for (uint32_t i = Win0X1; i < Win0X2; i++)
                        win_show[i] = 0;
                }
            }
        }

        int *vis = bgvisible[3];
        int *show = win_show;
        for (int i = 0; i < 240; i++)
        {
            *vis = *vis && *show;
            vis++;
            show++;
        }
    }

    if (REG_DISPCNT & BIT(12)) // Sprites
    {
        if (out & BIT(4))
        {
            for (int i = 0; i < 240; i++)
                win_show[i] = 1;
        }
        else
        {
            memset(win_show, 0, sizeof(win_show));
        }
        if (winobj) // obj has lowest priority
        {
            int *show = win_show;
            int *ptrsprwin = sprwin;
            if (inobj & BIT(4))
            {
                for (int i = 0; i < 240; i++)
                {
                    if (*ptrsprwin)
                        *show = 1;
                    show++;
                    ptrsprwin++;
                }
            }
            else
            {
                for (int i = 0; i < 240; i++)
                {
                    if (*ptrsprwin)
                        *show = 0;
                    show++;
                    ptrsprwin++;
                }
            }
        }
        if (win1) // Intermediate priority
        {
            if (in1 & BIT(4))
            {
                if (y >= Win1Y1 && y <= Win1Y2)
                {
                    for (uint32_t i = Win1X1; i < Win1X2; i++)
                        win_show[i] = 1;
                }
            }
            else
            {
                if (y >= Win1Y1 && y <= Win1Y2)
                {
                    for (uint32_t i = Win1X1; i < Win1X2; i++)
                        win_show[i] = 0;
                }
            }
        }
        if (win0) // Highest priority
        {
            if (in0 & BIT(4))
            {
                if (y >= Win0Y1 && y <= Win0Y2)
                {
                    for (uint32_t i = Win0X1; i < Win0X2; i++)
                        win_show[i] = 1;
                }
            }
            else
            {
                if (y >= Win0Y1 && y <= Win0Y2)
                {
                    for (uint32_t i = Win0X1; i < Win0X2; i++)
                        win_show[i] = 0;
                }
            }
        }

        int *vis = sprvisible[0];
        int *show = win_show;
        for (int i = 0; i < 240; i++)
        {
            *vis = *vis && *show;
            vis++;
            show++;
        }
        vis = sprvisible[1];
        show = win_show;
        for (int i = 0; i < 240; i++)
        {
            *vis = *vis && *show;
            vis++;
            show++;
        }
        vis = sprvisible[2];
        show = win_show;
        for (int i = 0; i < 240; i++)
        {
            *vis = *vis && *show;
            vis++;
            show++;
        }
        vis = sprvisible[3];
        show = win_show;
        for (int i = 0; i < 240; i++)
        {
            *vis = *vis && *show;
            vis++;
            show++;
        }
    }

    if ((REG_BLDCNT >> 6) & 3) // Special effect
    {
        if (out & BIT(5))
        {
            for (int i = 0; i < 240; i++)
                win_show[i] = 1;
        }
        else
        {
            memset(win_show, 0, sizeof(win_show));
        }
        if (winobj) // obj has lowest priority
        {
            int *show = win_show;
            int *ptrsprwin = sprwin;
            if (inobj & BIT(5))
            {
                for (int i = 0; i < 240; i++)
                {
                    if (*ptrsprwin)
                        *show = 1;
                    show++;
                    ptrsprwin++;
                }
            }
            else
            {
                for (int i = 0; i < 240; i++)
                {
                    if (*ptrsprwin)
                        *show = 0;
                    show++;
                    ptrsprwin++;
                }
            }
        }
        if (win1) // Intermediate priority
        {
            if (in1 & BIT(5))
            {
                if (y >= Win1Y1 && y <= Win1Y2)
                {
                    for (uint32_t i = Win1X1; i < Win1X2; i++)
                        win_show[i] = 1;
                }
            }
            else
            {
                if (y >= Win1Y1 && y <= Win1Y2)
                {
                    for (uint32_t i = Win1X1; i < Win1X2; i++)
                        win_show[i] = 0;
                }
            }
        }
        if (win0) // Highest priority
        {
            if (in0 & BIT(5))
            {
                if (y >= Win0Y1 && y <= Win0Y2)
                {
                    for (uint32_t i = Win0X1; i < Win0X2; i++)
                        win_show[i] = 1;
                }
            }
            else
            {
                if (y >= Win0Y1 && y <= Win0Y2)
                {
                    for (uint32_t i = Win0X1; i < Win0X2; i++)
                        win_show[i] = 0;
                }
            }
        }

        memcpy(win_coloreffect_enable, win_show,
               sizeof(win_coloreffect_enable));
    }
}

static uint16_t white_table[32][17]; // color, evy
static uint16_t black_table[32][17]; // color, evy

void GBA_FillFadeTables(void)
{
    for (int i = 0; i < 32; i++)
    {
        for (int y = 0; y < 17; y++)
        {
            white_table[i][y] = i + (((31 - i) * y) >> 4);
            black_table[i][y] = i - ((i * y) >> 4);
        }
    }

    // Fill array: Backdrop is always visible
    for (int i = 0; i < 240; i++)
    {
        backdropvisible[i] = 1;
        win_coloreffect_enable[i] = 1;
    }
}

static uint16_t fade_white(uint16_t col, uint16_t evy)
{
    return (white_table[(col >> 10) & 0x1F][evy] << 10)
           | (white_table[(col >> 5) & 0x1F][evy] << 5)
           | white_table[col & 0x1F][evy];
}
static uint16_t fade_black(uint16_t col, uint16_t evy)
{
    return (black_table[(col >> 10) & 0x1F][evy] << 10)
           | (black_table[(col >> 5) & 0x1F][evy] << 5)
           | black_table[col & 0x1F][evy];
}

static uint16_t min(uint16_t a, uint16_t b)
{
    return (a < b) ? a : b;
}

static uint16_t blend(uint16_t col_1, uint16_t col_2,
                      uint16_t eva, uint16_t evb)
{
    uint16_t r = min(31, (((col_1 & 0x1F) * eva) >> 4)
                            + (((col_2 & 0x1F) * evb) >> 4));
    uint16_t g = min(31, ((((col_1 >> 5) & 0x1F) * eva) >> 4)
                            + ((((col_2 >> 5) & 0x1F) * evb) >> 4));
    uint16_t b = min(31, ((((col_1 >> 10) & 0x1F) * eva) >> 4)
                            + ((((col_2 >> 10) & 0x1F) * evb) >> 4));
    return (b << 10) | (g << 5) | r;
}

static void gba_effects_apply(void)
{
    // Semi-Transparent OBJs
    //
    // OBJs that are defined as 'Semi-Transparent' in OAM memory are always
    // selected as 1st Target (regardless of BLDCNT Bit 4), and are always using
    // Alpha Blending mode (regardless of BLDCNT Bit 6-7).
    //
    // The BLDCNT register may be used to perform Brightness effects on the OBJ
    // (and/or other BG/BD layers).  However, if a semi-transparent OBJ pixel
    // does overlap a 2nd target pixel, then semi-transparency becomes priority,
    // and the brightness effect will not take place (neither on 1st, nor 2nd
    // target).

    // The OBJ Layer
    //
    // Before special effects are applied, the display controller computes the
    // OBJ priority ordering, and isolates the top-most OBJ pixel. In result,
    // only the top-most OBJ pixel is recursed at the time when processing
    // special effects. Ie. alpha blending and semi-transparency can be used for
    // OBJ-to-BG or BG-to-OBJ , but not for OBJ-to-OBJ.

    for (int l = 0; l < 3; l++)
    {
        // This clears sprite layer pixels that have another layer with higher
        // priority
        for (int i = 0; i < 240; i++)
        {
            if (sprvisible[l][i])
            {
                int k = l + 1;
                for (; k < 4; k++)
                {
                    sprvisible[k][i] = 0;
                    sprblend[k][i] = 0;
                }
            }
        }
    }

    uint16_t bldcnt = REG_BLDCNT;
    int mode = (bldcnt >> 6) & 3;

    if (mode == 0) // Nothing -- only blend transparent sprites
    {
        if (bldcnt >> 8) // At least there must be one 2nd target
        {
            // If any sprite is in blending mode, continue, else return
            int ret = 1;
            int i = 0;
            while (i < 240)
            {
                if (win_coloreffect_enable[i])
                {
                    if (sprblend[0][i] | sprblend[1][i] | sprblend[2][i]
                        | sprblend[3][i])
                    {
                        ret = 0;
                        break;
                    }
                }
                i++;
            }
            if (ret)
                return;
        }
        else
        {
            return;
        }
    }

    int layer_is_first_target[9];
    int layer_is_second_target[9];
    int layer_is_sprite[9];
    memset((void *)layer_is_first_target, 0, sizeof(layer_is_first_target));
    memset((void *)layer_is_second_target, 0, sizeof(layer_is_second_target));
    memset((void *)layer_is_sprite, 0, sizeof(layer_is_sprite));

    for (int l = 0; l < layer_active_num; l++)
    {
        switch (layer_id[l])
        {
            case BG0:
                layer_is_first_target[l] = bldcnt & BIT(0);
                layer_is_second_target[l] = bldcnt & BIT(8);
                break;
            case BG1:
                layer_is_first_target[l] = bldcnt & BIT(1);
                layer_is_second_target[l] = bldcnt & BIT(9);
                break;
            case BG2:
                layer_is_first_target[l] = bldcnt & BIT(2);
                layer_is_second_target[l] = bldcnt & BIT(10);
                break;
            case BG3:
                layer_is_first_target[l] = bldcnt & BIT(3);
                layer_is_second_target[l] = bldcnt & BIT(11);
                break;
            case SPR0:
                layer_is_sprite[l] = 1;
                layer_is_first_target[l] = bldcnt & BIT(4);
                layer_is_second_target[l] = bldcnt & BIT(12);
                break;
            case SPR1:
                layer_is_sprite[l] = 2;
                layer_is_first_target[l] = bldcnt & BIT(4);
                layer_is_second_target[l] = bldcnt & BIT(12);
                break;
            case SPR2:
                layer_is_sprite[l] = 3;
                layer_is_first_target[l] = bldcnt & BIT(4);
                layer_is_second_target[l] = bldcnt & BIT(12);
                break;
            case SPR3:
                layer_is_sprite[l] = 4;
                layer_is_first_target[l] = bldcnt & BIT(4);
                layer_is_second_target[l] = bldcnt & BIT(12);
                break;
            case BD:
                layer_is_first_target[l] = bldcnt & BIT(5);
                layer_is_second_target[l] = bldcnt & BIT(13);
                break;
            default:
                break; // ???
        }
    }

    uint32_t eva = REG_BLDALPHA & 0x1F;
    if (eva > 16)
        eva = 16;
    uint32_t evb = (REG_BLDALPHA >> 8) & 0x1F;
    if (evb > 16)
        evb = 16;

    if (mode == 1)
    {
        // Disable blending for transparent sprites when a 1st-target visible
        // pixel of any layer has higher priority
        int already_first_target[240];
        mem_clear_32((void *)already_first_target,
                     sizeof(already_first_target));

        for (int l = (layer_active_num - 1); l >= 0; l--)
        {
            if (!((layer_id[l] >= SPR0) && (layer_id[l] <= SPR3)))
            {
                for (int i = 0; i < 240; i++)
                {
                    if (layer_vis[l][i])
                        already_first_target[i] = 1;
                }
            }
            else
            {
                int sprite_layer = layer_id[l] - SPR0;
                for (int i = 0; i < 240; i++)
                {
                    if (already_first_target[i])
                        sprblend[sprite_layer][i] = 0;
                }
            }
        }
    }

    // Blend transparent-enabled sprites
    for (int l = layer_active_num - 1; l >= 0; l--)
    {
        if (layer_is_sprite[l])
        {
            int sprlayer = layer_is_sprite[l] - 1;

            // Transparent sprites are always affected by blending even if
            // window disables special effects!!! Tested on hardware
            for (int i = 0; i < 240; i++) //if (win_coloreffect_enable[i])
            {
                if (sprblend[sprlayer][i])
                {
                    // Search a non-transparent second target pixel
                    int k = l - 1;
                    for (; k >= 0; k--)
                    {
                        if (layer_vis[k][i])
                        {
                            if (layer_is_second_target[k])
                            {
                                sprfb[sprlayer][i] = blend(
                                        sprblendfb[sprlayer][i],
                                        layer_fb[k][i], eva, evb);
                            }
                            else
                            {
                                sprblend[sprlayer][i] = 0;
                            }
                            break;
                        }
                    }
                }
            }
        }
    }

    if (mode == 0) // Nothing
    {

    }
    else if (mode == 1) // Blend
    {
        for (int l = layer_active_num - 1; l >= 0; l--)
        {
            if (layer_is_first_target[l])
            {
                for (int i = 0; i < 240; i++)
                {
                    if (win_coloreffect_enable[i])
                    {
                        // Search a non-transparent second target pixel
                        int k = l - 1;
                        for (; k >= 0; k--)
                        {
                            if (layer_vis[k][i])
                            {
                                // Blending is only applied if the two layers
                                // are together, not if anything in between
                                if (layer_is_second_target[k])
                                {
                                    if (layer_is_sprite[l])
                                    {
                                        int sprlayer = layer_is_sprite[l] - 1;

                                        if (sprblend[sprlayer][i] == 0)
                                        {
                                            sprfb[sprlayer][i] = blend(
                                                    sprfb[sprlayer][i],
                                                    layer_fb[k][i], eva, evb);
                                        }
                                    }
                                    else
                                    {
                                        layer_fb[l][i] = blend(layer_fb[l][i],
                                                               layer_fb[k][i],
                                                               eva, evb);
                                    }
                                }
                                break;
                            }
                        }
                    }
                }
            }
        }
    }
    else if (mode == 2) // White
    {
        uint32_t evy = REG_BLDY & 0x1F;
        if (evy > 16)
            evy = 16;

        for (int l = layer_active_num - 1; l >= 0; l--)
        {
            if (layer_is_first_target[l])
            {
                if (layer_is_sprite[l])
                {
                    int sprlayer = layer_is_sprite[l] - 1;

                    for (int i = 0; i < 240; i++)
                    {
                        if (win_coloreffect_enable[i])
                        {
                            if (sprblend[sprlayer][i] == 0)
                            {
                                layer_fb[l][i] = fade_white(layer_fb[l][i], evy);
                            }
                        }
                    }
                }
                else
                {
                    for (int i = 0; i < 240; i++)
                    {
                        if (win_coloreffect_enable[i])
                        {
                            layer_fb[l][i] = fade_white(layer_fb[l][i], evy);
                        }
                    }
                }
            }
        }
    }
    if (mode == 3) // Black
    {
        uint32_t evy = REG_BLDY & 0x1F;
        if (evy > 16)
            evy = 16;

        for (int l = layer_active_num - 1; l >= 0; l--)
        {
            if (layer_is_first_target[l])
            {
                if (layer_is_sprite[l])
                {
                    int sprlayer = layer_is_sprite[l] - 1;

                    for (int i = 0; i < 240; i++)
                    {
                        if (win_coloreffect_enable[i])
                        {
                            if (sprblend[sprlayer][i] == 0)
                            {
                                layer_fb[l][i] = fade_black(layer_fb[l][i], evy);
                            }
                        }
                    }
                }
                else
                {
                    for (int i = 0; i < 240; i++)
                    {
                        if (win_coloreffect_enable[i])
                        {
                            layer_fb[l][i] = fade_black(layer_fb[l][i], evy);
                        }
                    }
                }
            }
        }
    }
}

//------------------------------------------------------------------------------

static void gba_greenswap_apply(int y)
{
    if (REG_GREENSWAP & 1)
    {
        uint16_t *destptr = (uint16_t *)&screen_buffer[240 * y];
        for (int i = 0; i < 240; i += 2)
        {
            uint16_t pix1 = *destptr;
            uint16_t pix2 = *(destptr + 1);
            *destptr++ = (pix1 & ~(31 << 5)) | (pix2 & (31 << 5));
            *destptr++ = (pix2 & ~(31 << 5)) | (pix1 & (31 << 5));
        }
    }
}

//------------------------------------------------------------------------------

static void GBA_DrawScanlineMode0(int32_t y)
{
    gba_video_all_buffers_clear();

    // Draw layers
    uint16_t bd_col = *((uint16_t *)((uint8_t *)MEM_PALETTE_ADDR));
    for (int i = 0; i < 240; i++)
        backdrop[i] = bd_col;
    if (REG_DISPCNT & BIT(8))
        gba_bg0drawtext(y);
    if (REG_DISPCNT & BIT(9))
        gba_bg1drawtext(y);
    if (REG_DISPCNT & BIT(10))
        gba_bg2drawtext(y);
    if (REG_DISPCNT & BIT(11))
        gba_bg3drawtext(y);
    if (REG_DISPCNT & BIT(12))
        gba_sprites_draw_mode012(y);

    gba_window_apply(y, REG_DISPCNT & BIT(13), REG_DISPCNT & BIT(14),
                     REG_DISPCNT & BIT(15));

    // Mix
    gba_sort_layers(0);
    gba_effects_apply();
    gba_blit_layers(y);
    gba_greenswap_apply(y);
}

static void GBA_DrawScanlineMode1(int32_t y)
{
    gba_video_all_buffers_clear();

    // Draw layers
    uint16_t bd_col = *((uint16_t *)((uint8_t *)MEM_PALETTE_ADDR));
    for (int i = 0; i < 240; i++)
        backdrop[i] = bd_col;
    if (REG_DISPCNT & BIT(8))
        gba_bg0drawtext(y);
    if (REG_DISPCNT & BIT(9))
        gba_bg1drawtext(y);
    if (REG_DISPCNT & BIT(10))
        gba_bg2drawaffine(y);
    if (REG_DISPCNT & BIT(12))
        gba_sprites_draw_mode012(y);
    gba_window_apply(y, REG_DISPCNT & BIT(13), REG_DISPCNT & BIT(14),
                     REG_DISPCNT & BIT(15));

    // Mix
    gba_sort_layers(1);
    gba_effects_apply();
    gba_blit_layers(y);
    gba_greenswap_apply(y);
}

static void GBA_DrawScanlineMode2(int32_t y)
{
    gba_video_all_buffers_clear();

    // Draw layers
    uint16_t bd_col = *((uint16_t *)((uint8_t *)MEM_PALETTE_ADDR));
    for (int i = 0; i < 240; i++)
        backdrop[i] = bd_col;
    if (REG_DISPCNT & BIT(10))
        gba_bg2drawaffine(y);
    if (REG_DISPCNT & BIT(11))
        gba_bg3drawaffine(y);
    if (REG_DISPCNT & BIT(12))
        gba_sprites_draw_mode012(y);
    gba_window_apply(y, REG_DISPCNT & BIT(13), REG_DISPCNT & BIT(14),
                     REG_DISPCNT & BIT(15));

    // Mix
    gba_sort_layers(2);
    gba_effects_apply();
    gba_blit_layers(y);
    gba_greenswap_apply(y);
}

static void GBA_DrawScanlineMode3(int32_t y)
{
    gba_video_all_buffers_clear();

    // Draw layers
    uint16_t bd_col = *((uint16_t *)((uint8_t *)MEM_PALETTE_ADDR));
    for (int i = 0; i < 240; i++)
        backdrop[i] = bd_col;
    if (REG_DISPCNT & BIT(12))
        gba_sprites_draw_mode345(y);
    if (REG_DISPCNT & BIT(10)) // BG2 enabled
        gba_bg2drawbitmapmode3(y);
    gba_window_apply(y, REG_DISPCNT & BIT(13), REG_DISPCNT & BIT(14),
                     REG_DISPCNT & BIT(15));

    // Mix
    gba_sort_layers(3);
    gba_effects_apply();
    gba_blit_layers(y);
    gba_greenswap_apply(y);
}

static void GBA_DrawScanlineMode4(int32_t y)
{
    gba_video_all_buffers_clear();

    // Draw layers
    uint16_t bd_col = *((uint16_t *)((uint8_t *)MEM_PALETTE_ADDR));
    for (int i = 0; i < 240; i++)
        backdrop[i] = bd_col;
    if (REG_DISPCNT & BIT(12))
        gba_sprites_draw_mode345(y);
    if (REG_DISPCNT & BIT(10)) // BG2 enabled
        gba_bg2drawbitmapmode4(y);
    gba_window_apply(y, REG_DISPCNT & BIT(13), REG_DISPCNT & BIT(14),
                     REG_DISPCNT & BIT(15));

    // Mix
    gba_sort_layers(4);
    gba_effects_apply();
    gba_blit_layers(y);
    gba_greenswap_apply(y);
}

static void GBA_DrawScanlineMode5(int32_t y)
{
    gba_video_all_buffers_clear();

    // Draw layers
    uint16_t bd_col = *((uint16_t *)((uint8_t *)MEM_PALETTE_ADDR));
    for (int i = 0; i < 240; i++)
        backdrop[i] = bd_col;
    if (REG_DISPCNT & BIT(12))
        gba_sprites_draw_mode345(y);
    if (REG_DISPCNT & BIT(10)) // BG2 enabled
        gba_bg2drawbitmapmode5(y);
    gba_window_apply(y, REG_DISPCNT & BIT(13), REG_DISPCNT & BIT(14),
                     REG_DISPCNT & BIT(15));

    // Mix
    gba_sort_layers(5);
    gba_effects_apply();
    gba_blit_layers(y);
    gba_greenswap_apply(y);
}

//------------------------------------------------------------------------------

void GBA_VideoUpdateRegister(uint32_t offset)
{
    switch (offset)
    {
        case OFFSET_BG2X_L:
        case OFFSET_BG2X_H:
            BG2lastx = REG_BG2X;
            if (BG2lastx & BIT(27))
                BG2lastx |= 0xF0000000;
            break;
        case OFFSET_BG2Y_L:
        case OFFSET_BG2Y_H:
            BG2lasty = REG_BG2Y;
            if (BG2lasty & BIT(27))
                BG2lasty |= 0xF0000000;
            break;

        case OFFSET_BG3X_L:
        case OFFSET_BG3X_H:
            BG3lastx = REG_BG3X;
            if (BG3lastx & BIT(27))
                BG3lastx |= 0xF0000000;
            break;
        case OFFSET_BG3Y_L:
        case OFFSET_BG3Y_H:
            BG3lasty = REG_BG3Y;
            if (BG3lasty & BIT(27))
                BG3lasty |= 0xF0000000;
            break;

        default:
            break;
    }
}

//------------------------------------------------------------------------------

void GBA_ConvertScreenBufferTo32RGB(void *dst)
{
    uint16_t *src = screen_buffer_array[curr_screen_buffer ^ 1];
    uint32_t *dest = (uint32_t *)dst;
    for (int i = 0; i < 240 * 160; i++)
    {
        uint32_t data = (uint32_t)*src++;
        *dest++ = ((data & 0x1F) << 3)
                  | ((data & (0x1F << 5)) << 6)
                  | ((data & (0x1F << 10)) << 9)
                  | (0xFF << 24);
    }
}

void GBA_ConvertScreenBufferTo24RGB(void *dst)
{
    uint16_t *src = screen_buffer_array[curr_screen_buffer ^ 1];
    uint8_t *dest = (void *)dst;

    for (int i = 0; i < 240 * 160; i++)
    {
        uint32_t data = (uint32_t)*src++;
        *dest++ = (data & 0x1F) << 3;
        *dest++ = (data & (0x1F << 5)) >> 2;
        *dest++ = (data & (0x1F << 10)) >> 7;
    }
}
