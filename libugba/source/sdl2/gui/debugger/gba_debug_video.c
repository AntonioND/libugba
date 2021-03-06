// SPDX-License-Identifier: LGPL-3.0-only
//
// Copyright (c) 2011-2015, 2019-2021, Antonio Niño Díaz

#include <stdlib.h>
#include <string.h>

#include <ugba/ugba.h>

#include "../../debug_utils.h"

//----------------------------------------------------------------

static uint32_t rgb16to32(uint16_t color)
{
    int r = (color & 31) << 3;
    int g = ((color >> 5) & 31) << 3;
    int b = ((color >> 10) & 31) << 3;
    return (b << 16) | (g << 8) | r;
}

static inline int min(int a, int b)
{
    return (a < b) ? a : b;
}

//----------------------------------------------------------------

void GBA_Debug_PrintZoomedSpriteAt(int spritenum, int buf_has_alpha_channel,
                                   unsigned char *buffer,
                                   int bufw, UNUSED int bufh,
                                   int posx, int posy, int sizex, int sizey)
{
    // Temp buffers
    int *sprbuffer = calloc(64 * 64, sizeof(int));
    int *sprbuffer_vis = calloc(64 * 64, sizeof(int));

    if ((sprbuffer == NULL) || (sprbuffer_vis == NULL))
    {
        Debug_Log("%s(): Not enough memory.");
        goto cleanup;
    }

    static const int spr_size[4][4][2] = { // Shape, Size, (x,y)
        { { 8, 8 }, { 16, 16 }, { 32, 32 }, { 64, 64 } }, // Square
        { { 16, 8 }, { 32, 8 }, { 32, 16 }, { 64, 32 } }, // Horizontal
        { { 8, 16 }, { 8, 32 }, { 16, 32 }, { 32, 64 } }, // Vertical
        { { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 } }        // Prohibited
    };

    oam_entry *spr = &((oam_entry *)MEM_OAM)[spritenum];

    uint16_t attr0 = spr->attr0;
    uint16_t attr1 = spr->attr1;
    uint16_t attr2 = spr->attr2;
    uint16_t shape = attr0 >> 14;
    uint16_t size = attr1 >> 14;
    int sx = spr_size[shape][size][0];
    int sy = spr_size[shape][size][1];
    uint16_t tilebaseno = attr2 & 0x3FF;

    if (attr0 & BIT(13)) // 256 colors
    {
        tilebaseno >>= 1; // In 256 mode, they need double space

        uint16_t *palptr = MEM_PALETTE_OBJ;

        // Start drawing
        for (int ydiff = 0; ydiff < sy; ydiff++)
        {
            for (int xdiff = 0; xdiff < sx; xdiff++)
            {
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
                uint8_t *tile_ptr = (uint8_t *)MEM_VRAM_OBJ + (tileindex * 64);

                int _x = xdiff & 7;
                int _y = ydiff & 7;

                uint8_t data = tile_ptr[_x + (_y * 8)];

                if (data)
                {
                    sprbuffer[ydiff * 64 + xdiff] = rgb16to32(palptr[data]);
                    sprbuffer_vis[ydiff * 64 + xdiff] = 1;
                }
            }
        }
    }
    else // 16 colors
    {
        uint16_t palno = attr2 >> 12;
        uint16_t *palptr = MEM_PALETTE_OBJ + (palno * 16);

        // Start drawing
        for (int ydiff = 0; ydiff < sy; ydiff++)
        {
            for (int xdiff = 0; xdiff < sx; xdiff++)
            {
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
                uint8_t *tile_ptr = (uint8_t *)MEM_VRAM + 0x10000 + (tileindex * 32);

                int _x = xdiff & 7;
                int _y = ydiff & 7;

                uint8_t data = tile_ptr[(_x / 2) + (_y * 4)];

                if (_x & 1)
                    data = data >> 4;
                else
                    data = data & 0xF;

                if (data)
                {
                    sprbuffer[ydiff * 64 + xdiff] = rgb16to32(palptr[data]);
                    sprbuffer_vis[ydiff * 64 + xdiff] = 1;
                }
            }
        }
    }

    // Expand/copy to real buffer
    int factor = min(sizex / sx, sizey / sy);

    if (buf_has_alpha_channel)
    {
        for (int i = 0; i < sizex; i++)
        {
            for (int j = 0; j < sizey; j++)
            {
                int bufx = i + posx;
                int bufy = j + posy;

                int index = ((bufy * bufw) + bufx) * 4;
                int srcindex = (j / factor) * 64 + (i / factor);

                if (sprbuffer_vis[srcindex])
                {
                    buffer[index + 0] = sprbuffer[srcindex] & 0xFF;
                    buffer[index + 1] = (sprbuffer[srcindex] >> 8) & 0xFF;
                    buffer[index + 2] = (sprbuffer[srcindex] >> 16) & 0xFF;
                    buffer[index + 3] = 255;
                }
                else
                {
                    buffer[index + 0] = 0;
                    buffer[index + 1] = 0;
                    buffer[index + 2] = 0;
                    buffer[index + 3] = 0;
                }
            }
        }
    }
    else
    {
        for (int i = 0; i < sizex; i++)
        {
            for (int j = 0; j < sizey; j++)
            {
                int bufx = i + posx;
                int bufy = j + posy;

                int index = ((bufy * bufw) + bufx) * 3;
                int srcindex = (j / factor) * 64 + (i / factor);

                if (sprbuffer_vis[srcindex])
                {
                    buffer[index + 0] = sprbuffer[srcindex] & 0xFF;
                    buffer[index + 1] = (sprbuffer[srcindex] >> 8) & 0xFF;
                    buffer[index + 2] = (sprbuffer[srcindex] >> 16) & 0xFF;
                }
                else
                {
                    if ((i >= (sx * factor)) || (j >= (sy * factor)))
                    {
                        if (((i ^ j) & 7) == 0)
                        {
                            buffer[index + 0] = 255;
                            buffer[index + 1] = 0;
                            buffer[index + 2] = 0;
                        }
                    }
                }
            }
        }
    }

cleanup:
    free(sprbuffer);
    free(sprbuffer_vis);
}

// Starting sprite number = page * 64
void GBA_Debug_PrintSpritesPage(int page, int buf_has_alpha_channel,
                                unsigned char *buffer, int bufw, int bufh)
{
    if (buf_has_alpha_channel)
    {
        memset(buffer, 0, bufw * bufh * 4);
    }
    else
    {
        for (int i = 0; i < bufw; i++)
        {
            for (int j = 0; j < bufh; j++)
            {
                int index = (j * bufw + i) * 3;
                buffer[index + 0] = ((i & 32) ^ (j & 32)) ? 0x80 : 0xB0;
                buffer[index + 1] = ((i & 32) ^ (j & 32)) ? 0x80 : 0xB0;
                buffer[index + 2] = ((i & 32) ^ (j & 32)) ? 0x80 : 0xB0;
            }
        }
    }

    for (int i = 0; i < 8; i++)
    {
        for (int j = 0; j < 8; j++)
        {
            int sprnum = (page * 64) + (j * 8) + i;
            GBA_Debug_PrintZoomedSpriteAt(sprnum, buf_has_alpha_channel, buffer,
                    bufw, bufh, (i * (64 + 16)) + 16, (j * (64 + 16)) + 16, 64,
                    64);
        }
    }
}

//----------------------------------------------------------------

void GBA_Debug_PrintTiles(unsigned char *buffer,
                          int bufw, UNUSED int bufh, int cbb,
                          int colors, int palette)
{
    uint8_t *charbaseblockptr = (uint8_t *)MEM_VRAM_BG + (cbb - 0x06000000);

    for (int i = 0; i < 256; i++)
    {
        for (int j = 0; j < 256; j++)
        {
            int index = (j * bufw + i) * 3;
            buffer[index + 0] = ((i & 16) ^ (j & 16)) ? 0x80 : 0xB0;
            buffer[index + 1] = ((i & 16) ^ (j & 16)) ? 0x80 : 0xB0;
            buffer[index + 2] = ((i & 16) ^ (j & 16)) ? 0x80 : 0xB0;
        }
    }

    if (colors == 256) // 256 Colors
    {
        int jmax = (cbb == 0x06014000) ? 64 : 128; // Half size

        // If cbb >= 0x06010000 --> sprite
        uint32_t pal = (cbb >= 0x06010000) ? 256 : 0;

        for (int i = 0; i < 256; i++)
        {
            for (int j = 0; j < jmax; j++)
            {
                uint32_t Index = (i / 8) + (j / 8) * 32;
                uint8_t *dataptr = (uint8_t *)&(charbaseblockptr[(Index & 0x3FF) * 64]);

                int data = dataptr[(i & 7) + ((j & 7) * 8)];

                uint32_t color = rgb16to32(((uint16_t *)MEM_PALETTE)[data + pal]);

                int index = (j * bufw + i) * 3;
                buffer[index + 0] = color & 0xFF;
                buffer[index + 1] = (color >> 8) & 0xFF;
                buffer[index + 2] = (color >> 16) & 0xFF;
            }
        }
        for (int i = 0; i < 256; i++)
        {
            for (int j = jmax; j < 256; j++)
            {
                if (((i ^ j) & 7) == 0)
                {
                    int index = (j * bufw + i) * 3;
                    buffer[index + 0] = 255;
                    buffer[index + 1] = 0;
                    buffer[index + 2] = 0;
                }
            }
        }
    }
    else if (colors == 16) // 16 colors
    {
        int jmax = (cbb == 0x06014000) ? 128 : 256; // Half size

        // If cbb >= 0x06010000 --> sprite
        uint32_t pal = (cbb >= 0x06010000) ? (palette + 16) : palette;
        uint16_t *palptr = &(((uint16_t *)MEM_PALETTE)[pal * 2 * 16]);

        for (int i = 0; i < 256; i++)
        {
            for (int j = 0; j < jmax; j++)
            {
                uint32_t Index = (i / 8) + (j / 8) * 32;
                uint8_t *dataptr = (uint8_t *)&(charbaseblockptr[(Index & 0x3FF) * 32]);

                int data = dataptr[((i & 7) + ((j & 7) * 8)) / 2];

                if (i & 1)
                    data = data >> 4;
                else
                    data = data & 0xF;

                uint32_t color = rgb16to32(palptr[data]);

                int index = (j * bufw + i) * 3;
                buffer[index + 0] = color & 0xFF;
                buffer[index + 1] = (color >> 8) & 0xFF;
                buffer[index + 2] = (color >> 16) & 0xFF;
            }
        }

        for (int i = 0; i < 256; i++)
        {
            for (int j = jmax; j < 256; j++)
            {
                if (((i ^ j) & 7) == 0)
                {
                    int index = (j * bufw + i) * 3;
                    buffer[index + 0] = 255;
                    buffer[index + 1] = 0;
                    buffer[index + 2] = 0;
                }
            }
        }
    }
}

void GBA_Debug_PrintTilesAlpha(unsigned char *buffer,
                               int bufw, int bufh, int cbb,
                               int colors, int palette)
{
    memset(buffer, 0, bufw * bufh * 4);

    uint8_t *charbaseblockptr = (uint8_t *)MEM_VRAM_BG + (cbb - 0x06000000);

    if (colors == 256) // 256 Colors
    {
        int jmax = (cbb == 0x06014000) ? 64 : 128; // Half size

        // If cbb >= 0x06010000 --> sprite
        uint32_t pal = (cbb >= 0x06010000) ? 256 : 0;

        for (int i = 0; i < 256; i++)
        {
            for (int j = 0; j < jmax; j++)
            {
                uint32_t Index = (i / 8) + (j / 8) * 32;
                uint8_t *dataptr = (uint8_t *)&(charbaseblockptr[(Index & 0x3FF) * 64]);

                int data = dataptr[(i & 7) + ((j & 7) * 8)];

                uint32_t color = rgb16to32(((uint16_t *)MEM_PALETTE)[data + pal]);

                int index = (j * bufw + i) * 4;
                buffer[index + 0] = color & 0xFF;
                buffer[index + 1] = (color >> 8) & 0xFF;
                buffer[index + 2] = (color >> 16) & 0xFF;
                buffer[index + 3] = 255;
            }
        }
        for (int i = 0; i < 256; i++)
        {
            for (int j = jmax; j < 256; j++)
            {
                if (((i ^ j) & 7) == 0)
                {
                    int index = (j * bufw + i) * 4;
                    buffer[index + 0] = 255;
                    buffer[index + 1] = 0;
                    buffer[index + 2] = 0;
                    buffer[index + 3] = 255;
                }
            }
        }
    }
    else if (colors == 16) // 16 colors
    {
        int jmax = (cbb == 0x06014000) ? 128 : 256; // Half size

        // If cbb >= 0x06010000 --> sprite
        uint32_t pal = (cbb >= 0x06010000) ? (palette + 16) : palette;
        uint16_t *palptr = &((uint16_t *)MEM_PALETTE)[pal * 2 * 16];

        for (int i = 0; i < 256; i++)
        {
            for (int j = 0; j < jmax; j++)
            {
                uint32_t Index = (i / 8) + (j / 8) * 32;
                uint8_t *dataptr = (uint8_t *)&(charbaseblockptr[(Index & 0x3FF) * 32]);

                int data = dataptr[((i & 7) + ((j & 7) * 8)) / 2];

                if (i & 1)
                    data = data >> 4;
                else
                    data = data & 0xF;

                uint32_t color = rgb16to32(palptr[data]);

                int index = (j * bufw + i) * 4;
                buffer[index + 0] = color & 0xFF;
                buffer[index + 1] = (color >> 8) & 0xFF;
                buffer[index + 2] = (color >> 16) & 0xFF;
                buffer[index + 3] = 255;
            }
        }

        for (int i = 0; i < 256; i++)
        {
            for (int j = jmax; j < 256; j++)
            {
                if (((i ^ j) & 7) == 0)
                {
                    int index = (j * bufw + i) * 4;
                    buffer[index + 0] = 255;
                    buffer[index + 1] = 0;
                    buffer[index + 2] = 0;
                    buffer[index + 3] = 255;
                }
            }
        }
    }
}

void GBA_Debug_TilePrint64x64(unsigned char *buffer, UNUSED int bufw,
                              UNUSED int bufh, int cbb, int tile,
                              int palcolors, int selected_pal)
{
    int tiletempbuffer[8 * 8];
    int tiletempvis[8 * 8];
    memset(tiletempvis, 0, sizeof(tiletempvis));

    uint8_t *charbaseblockptr = (uint8_t *)MEM_VRAM + (cbb - 0x06000000);

    if (palcolors == 256) // 256 Colors
    {
        uint8_t *data = (uint8_t *)&(charbaseblockptr[(tile & 0x3FF) * 64]);

        // If SelectedBase >= 0x06010000 --> sprite
        uint32_t pal = (cbb >= 0x06010000) ? 256 : 0;

        for (int i = 0; i < 8; i++)
        {
            for (int j = 0; j < 8; j++)
            {
                uint8_t dat_ = data[j * 8 + i];

                tiletempbuffer[j * 8 + i] =
                        rgb16to32(((uint16_t *)MEM_PALETTE)[dat_ + pal]);
                tiletempvis[j * 8 + i] = dat_;
            }
        }
    }
    else if (palcolors == 16) // 16 Colors
    {
        uint8_t *data = (uint8_t *)&(charbaseblockptr[(tile & 0x3FF) * 32]);

        // If cbb >= 0x06010000 --> sprite
        uint32_t pal = (cbb >= 0x06010000) ? (selected_pal + 16) : selected_pal;
        uint16_t *palptr = &((uint16_t *)MEM_PALETTE)[pal * 2 * 16];

        for (int i = 0; i < 8; i++)
        {
            for (int j = 0; j < 8; j++)
            {
                uint8_t dat_ = data[(j * 8 + i) / 2];
                if (i & 1)
                    dat_ = dat_ >> 4;
                else
                    dat_ = dat_ & 0xF;

                tiletempbuffer[j * 8 + i] = rgb16to32(palptr[dat_]);
                tiletempvis[j * 8 + i] = dat_;
            }
        }
    }

    // Expand to 64x64
    for (int i = 0; i < 64; i++)
    {
        for (int j = 0; j < 64; j++)
        {
            int dst = (j * 64 + i) * 3;
            uint8_t color = ((i & 16) ^ (j & 16)) ? 0x80 : 0xB0;
            buffer[dst + 0] = color;
            buffer[dst + 1] = color;
            buffer[dst + 2] = color;
        }
    }

    for (int i = 0; i < 64; i++)
    {
        for (int j = 0; j < 64; j++)
        {
            if (tiletempvis[(j / 8) * 8 + (i / 8)])
            {
                int dst = (j * 64 + i) * 3;
                uint32_t color = tiletempbuffer[(j / 8) * 8 + (i / 8)];
                buffer[dst + 0] = color & 0xFF;
                buffer[dst + 1] = (color >> 8) & 0xFF;
                buffer[dst + 2] = (color >> 16) & 0xFF;
            }
        }
    }
}

//----------------------------------------------------------------

static uint32_t se_index(uint32_t tx, uint32_t ty, uint32_t pitch) // From tonc
{
    uint32_t sbb = (ty / 32) * (pitch / 32) + (tx / 32);
    return sbb * 1024 + (ty % 32) * 32 + tx % 32;
}

static uint32_t se_index_affine(uint32_t tx, uint32_t ty, uint32_t tpitch)
{
    return (ty * tpitch) + tx;
}

// bgmode => 1 = text, 2 = affine, 3,4,5 = bmp mode 3,4,5
void GBA_Debug_PrintBackgroundAlpha(unsigned char *buffer, int bufw, int bufh,
                                    uint16_t control, int bgmode, int page)
{
    if (bgmode == 0) // Shouldn't happen
        return;

    memset(buffer, 0, bufw * bufh * 4);

    if (bgmode == 1) // Text
    {
        static const uint32_t text_bg_size[4][2] = {
            { 256, 256 }, { 512, 256 }, { 256, 512 }, { 512, 512 }
        };

        uint8_t *charbaseblockptr =
                (uint8_t *)MEM_VRAM + (((control >> 2) & 3) * (16 * 1024));
        uint16_t *scrbaseblockptr = (uint16_t *)
                ((uint8_t *)MEM_VRAM + (((control >> 8) & 0x1F) * (2 * 1024)));

        uint32_t sizex = text_bg_size[control >> 14][0];
        uint32_t sizey = text_bg_size[control >> 14][1];

        if (control & BIT(7)) // 256 colors
        {
            for (uint32_t i = 0; i < sizex; i++)
            {
                for (uint32_t j = 0; j < sizey; j++)
                {
                    uint32_t index = se_index(i / 8, j / 8, sizex / 8);
                    uint16_t SE = scrbaseblockptr[index];
                    // Screen entry data:
                    // 0-9 tile id
                    // 10-hflip
                    // 11-vflip
                    int _x = i & 7;
                    if (SE & BIT(10))
                        _x = 7 - _x; // H flip

                    int _y = j & 7;
                    if (SE & BIT(11))
                        _y = 7 - _y; // V flip

                    int data = charbaseblockptr[((SE & 0x3FF) * 64) + (_x + (_y * 8))];

                    uint32_t color = rgb16to32(MEM_PALETTE_BG[data]);
                    buffer[(j * bufw + i) * 4 + 0] = color & 0xFF;
                    buffer[(j * bufw + i) * 4 + 1] = (color >> 8) & 0xFF;
                    buffer[(j * bufw + i) * 4 + 2] = (color >> 16) & 0xFF;
                    buffer[(j * bufw + i) * 4 + 3] = data ? 0xFF : 0;
                }
            }
        }
        else // 16 colors
        {
            for (uint32_t i = 0; i < sizex; i++)
            {
                for (uint32_t j = 0; j < sizey; j++)
                {
                    uint32_t index = se_index(i / 8, j / 8, sizex / 8);
                    uint16_t SE = scrbaseblockptr[index];
                    // Screen entry data
                    // 0-9 tile id
                    // 10-hflip
                    // 11-vflip
                    // 12-15-pal
                    uint16_t *palptr = &(MEM_PALETTE_BG[(SE >> 12) * 16]);

                    int _x = i & 7;
                    if (SE & BIT(10))
                        _x = 7 - _x; // H flip

                    int _y = j & 7;
                    if (SE & BIT(11))
                        _y = 7 - _y; // V flip

                    uint32_t data = charbaseblockptr[((SE & 0x3FF) * 32)
                                                + ((_x / 2) + (_y * 4))];

                    if (_x & 1)
                        data = data >> 4;
                    else
                        data = data & 0xF;

                    uint32_t color = rgb16to32(palptr[data]);
                    buffer[(j * bufw + i) * 4 + 0] = color & 0xFF;
                    buffer[(j * bufw + i) * 4 + 1] = (color >> 8) & 0xFF;
                    buffer[(j * bufw + i) * 4 + 2] = (color >> 16) & 0xFF;
                    buffer[(j * bufw + i) * 4 + 3] = data ? 0xFF : 0;
                }
            }
        }
    }
    else if (bgmode == 2) // Affine
    {
        static const uint32_t affine_bg_size[4] = { 128, 256, 512, 1024 };

        uint8_t *charbaseblockptr =
                ((uint8_t *)MEM_VRAM) + ((control >> 2) & 3) * (16 * 1024);
        uint8_t *scrbaseblockptr =
                ((uint8_t *)MEM_VRAM) + ((control >> 8) & 0x1F) * (2 * 1024);

        uint32_t size = affine_bg_size[control >> 14];
        uint32_t tilesize = size / 8;

        // Always 256 color

        for (uint32_t i = 0; i < size; i++)
        {
            for (uint32_t j = 0; j < size; j++)
            {
                int _x = i & 7;
                int _y = j & 7;

                uint32_t index = se_index_affine(i / 8, j / 8, tilesize);
                uint8_t SE = scrbaseblockptr[index];
                uint16_t data = charbaseblockptr[(SE * 64) + (_x + (_y * 8))];

                uint32_t color = rgb16to32(MEM_PALETTE_BG[data]);
                buffer[(j * bufw + i) * 4 + 0] = color & 0xFF;
                buffer[(j * bufw + i) * 4 + 1] = (color >> 8) & 0xFF;
                buffer[(j * bufw + i) * 4 + 2] = (color >> 16) & 0xFF;
                buffer[(j * bufw + i) * 4 + 3] = data ? 0xFF : 0;
            }
        }
    }
    else if (bgmode == 3) // BG2 mode 3
    {
        uint16_t *srcptr = (uint16_t *)MEM_VRAM;

        for (int i = 0; i < 240; i++)
        {
            for (int j = 0; j < 160; j++)
            {
                uint16_t data = srcptr[i + 240 * j];
                uint32_t color = rgb16to32(data);
                buffer[(j * bufw + i) * 4 + 0] = color & 0xFF;
                buffer[(j * bufw + i) * 4 + 1] = (color >> 8) & 0xFF;
                buffer[(j * bufw + i) * 4 + 2] = (color >> 16) & 0xFF;
                buffer[(j * bufw + i) * 4 + 3] = 0xFF;
            }
        }
    }
    else if (bgmode == 4) // BG2 mode 4
    {
        uint8_t *srcptr = (((uint8_t *)MEM_VRAM) + (page ? 0xA000 : 0));

        for (int i = 0; i < 240; i++)
        {
            for (int j = 0; j < 160; j++)
            {
                uint16_t data = MEM_PALETTE_BG[srcptr[i + 240 * j]];
                uint32_t color = rgb16to32(data);
                buffer[(j * bufw + i) * 4 + 0] = color & 0xFF;
                buffer[(j * bufw + i) * 4 + 1] = (color >> 8) & 0xFF;
                buffer[(j * bufw + i) * 4 + 2] = (color >> 16) & 0xFF;
                buffer[(j * bufw + i) * 4 + 3] = 0xFF;
            }
        }
    }
    else if (bgmode == 5) // BG2 mode 5
    {
        uint16_t *srcptr = (uint16_t *)(((uint8_t *)MEM_VRAM) + (page ? 0xA000 : 0));

        for (int i = 0; i < 160; i++)
        {
            for (int j = 0; j < 128; j++)
            {
                uint16_t data = srcptr[i + 160 * j];
                uint32_t color = rgb16to32(data);
                buffer[(j * bufw + i) * 4 + 0] = color & 0xFF;
                buffer[(j * bufw + i) * 4 + 1] = (color >> 8) & 0xFF;
                buffer[(j * bufw + i) * 4 + 2] = (color >> 16) & 0xFF;
                buffer[(j * bufw + i) * 4 + 3] = 0xFF;
            }
        }
    }
}
