// SPDX-License-Identifier: LGPL-3.0-only
//
// Copyright (c) 2011-2015, 2019-2021, Antonio Niño Díaz

#include <string.h>

#include <SDL2/SDL.h>

#include <ugba/ugba.h>

#include "../../debug_utils.h"
#include "../../png_utils.h"

#include "../font_utils.h"
#include "../win_utils.h"
#include "../window_handler.h"

#include "file_utils.h"
#include "gba_debug_video.h"
#include "win_gba_debugger.h"

//------------------------------------------------------------------------------

static int WinIDGBAMapViewer;

#define WIN_GBA_MAPVIEWER_WIDTH  461
#define WIN_GBA_MAPVIEWER_HEIGHT 280

static int GBAMapViewerCreated = 0;

static unsigned char *gba_mapviewer_buffer = NULL;

//------------------------------------------------------------------------------

static uint32_t gba_mapview_selected_bg = 0;
static uint32_t gba_mapview_selected_tilex = 0;
static uint32_t gba_mapview_selected_tiley = 0;
static uint32_t gba_mapview_selected_bitmap_page = 0;

static uint32_t gba_mapview_sizex = 0;
static uint32_t gba_mapview_sizey = 0;
static uint32_t gba_mapview_bgmode = 0;
static uint32_t gba_mapview_bgcontrolreg = 0;

static int gba_mapview_scrollx = 0;
static int gba_mapview_scrolly = 0;
static int gba_mapview_scrollx_max = 0;
static int gba_mapview_scrolly_max = 0;

#define GBA_MAP_BUFFER_WIDTH  (32 * 8)
#define GBA_MAP_BUFFER_HEIGHT (32 * 8)

// The complete map is copied to gba_map_buffer[] as needed
static unsigned char gba_complete_map_buffer[1024 * 1024 * 4];
static unsigned char gba_map_buffer[GBA_MAP_BUFFER_WIDTH *
                                    GBA_MAP_BUFFER_HEIGHT * 3];

static unsigned char gba_map_zoomed_tile_buffer[64 * 64 * 3];

//------------------------------------------------------------------------------

static gui_console gba_mapview_con;
static gui_element gba_mapview_textbox;

static gui_console gba_mapview_tileinfo_con;
static gui_element gba_mapview_tileinfo_textbox;

static gui_element gba_mapview_bg_label;
static gui_element gba_mapview_bg0_radbtn, gba_mapview_bg1_radbtn,
                   gba_mapview_bg2_radbtn, gba_mapview_bg3_radbtn;

static gui_element gba_mapview_page_label;
static gui_element gba_mapview_page0_radbtn, gba_mapview_page1_radbtn;

static gui_element gba_mapview_zoomed_tile_bmp;

static gui_element gba_mapview_dumpbtn;

static gui_element gba_mapview_tiles_bmp;

static gui_element gba_mapview_scrollx_scrollbar,
                   gba_mapview_scrolly_scrollbar;

static gui_element *gba_mapviwer_window_gui_elements[] = {
    &gba_mapview_bg_label,
    &gba_mapview_bg0_radbtn, &gba_mapview_bg1_radbtn,
    &gba_mapview_bg2_radbtn, &gba_mapview_bg3_radbtn,
    &gba_mapview_page_label,
    &gba_mapview_page0_radbtn, &gba_mapview_page1_radbtn,
    &gba_mapview_zoomed_tile_bmp,
    &gba_mapview_dumpbtn,
    &gba_mapview_textbox,
    &gba_mapview_tileinfo_textbox,
    &gba_mapview_tiles_bmp,
    &gba_mapview_scrollx_scrollbar, &gba_mapview_scrolly_scrollbar,
    NULL
};

static gui_global gba_mapviewer_window_gui = {
    gba_mapviwer_window_gui_elements,
    NULL,
    NULL
};

//----------------------------------------------------------------

static int _win_gba_mapviewer_tiles_bmp_callback(int x, int y)
{
    gba_mapview_selected_tilex = (x / 8) + gba_mapview_scrollx;
    gba_mapview_selected_tiley = (y / 8) + gba_mapview_scrolly;

    Win_GBAMapViewerUpdate();

    return 1;
}

static void _win_gba_mapviewer_scrollbar_x_callback(int value)
{
    gba_mapview_scrollx = value;
}

static void _win_gba_mapviewer_scrollbar_y_callback(int value)
{
    gba_mapview_scrolly = value;
}

static void _win_gba_mapviewer_bgnum_radbtn_callback(int num)
{
    gba_mapview_selected_bg = num;
}

static void _win_gba_mapviewer_pagenum_radbtn_callback(int num)
{
    gba_mapview_selected_bitmap_page = num;
}

//----------------------------------------------------------------

static void _win_gba_mapviewer_update_mapbuffer_from_temp_buffer(void)
{
    // Copy to real buffer
    for (int i = 0; i < GBA_MAP_BUFFER_WIDTH; i++)
    {
        for (int j = 0; j < GBA_MAP_BUFFER_HEIGHT; j++)
        {
            unsigned int index = (j * GBA_MAP_BUFFER_WIDTH + i) * 3;
            unsigned char v = ((i & 32) ^ (j & 32)) ? 0x80 : 0xB0;
            gba_map_buffer[index + 0] = v;
            gba_map_buffer[index + 1] = v;
            gba_map_buffer[index + 2] = v;
        }
    }

    int scrollx = gba_mapview_scrollx * 8;
    int scrolly = gba_mapview_scrolly * 8;

    for (uint32_t i = 0; i < GBA_MAP_BUFFER_WIDTH; i++)
    {
        for (uint32_t j = 0; j < GBA_MAP_BUFFER_HEIGHT; j++)
        {
            if ((i >= gba_mapview_sizex) || (j >= gba_mapview_sizey))
            {
                if (((i ^ j) & 7) == 0)
                {
                    unsigned int index = (j * GBA_MAP_BUFFER_WIDTH + i) * 3;
                    gba_map_buffer[index + 0] = 0xFF;
                    gba_map_buffer[index + 1] = 0x00;
                    gba_map_buffer[index + 2] = 0x00;
                }
            }
            else //if (tempvismapbuffer[(j + scrolly) * 1024 + (i + scrollx)])
            {
                unsigned int index = ((j + scrolly) * 1024 + (i + scrollx)) * 4;
                int r = gba_complete_map_buffer[index + 0];
                int g = gba_complete_map_buffer[index + 1];
                int b = gba_complete_map_buffer[index + 2];
                int a = gba_complete_map_buffer[index + 3];

                if (a)
                {
                    unsigned int dest = (j * GBA_MAP_BUFFER_WIDTH + i) * 3;
                    gba_map_buffer[dest + 0] = r;
                    gba_map_buffer[dest + 1] = g;
                    gba_map_buffer[dest + 2] = b;
                }
            }
        }
    }
}

static uint32_t rgb16to32(uint16_t color)
{
    int r = (color & 31) << 3;
    int g = ((color >> 5) & 31) << 3;
    int b = ((color >> 10) & 31) << 3;
    return (b << 16) | (g << 8) | r;
}

static uint32_t se_index(uint32_t tx, uint32_t ty, uint32_t pitch) // from tonc
{
    uint32_t sbb = (ty / 32) * (pitch / 32) + (tx / 32);
    return sbb * 1024 + (ty % 32) * 32 + tx % 32;
}

static uint32_t se_index_affine(uint32_t tx, uint32_t ty, uint32_t tpitch)
{
    return (ty * tpitch) + tx;
}

void Win_GBAMapViewerUpdate(void)
{
    if (GBAMapViewerCreated == 0)
        return;

    GUI_ConsoleClear(&gba_mapview_con);
    GUI_ConsoleClear(&gba_mapview_tileinfo_con);

    uint32_t mode = REG_DISPCNT & 0x7;

    //--------------------------------------------------------------------------

    static const uint32_t bgenabled[8][4] = {
        {1, 1, 1, 1}, {1, 1, 1, 0}, {0, 0, 1, 1}, {0, 0, 1, 0},
        {0, 0, 1, 0}, {0, 0, 1, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}
    };

    if (bgenabled[mode][gba_mapview_selected_bg] == 0)
    {
        gba_mapview_selected_bg = 0;

        while (bgenabled[mode][gba_mapview_selected_bg] == 0)
        {
            gba_mapview_selected_bg++;
            if (gba_mapview_selected_bg >= 4)
                break;
        }

        if (gba_mapview_selected_bg == 0)
        {
            GUI_RadioButtonSetPressed(&gba_mapviewer_window_gui,
                                      &gba_mapview_bg0_radbtn);
        }
        else if (gba_mapview_selected_bg == 1)
        {
            GUI_RadioButtonSetPressed(&gba_mapviewer_window_gui,
                                      &gba_mapview_bg1_radbtn);
        }
        else if (gba_mapview_selected_bg == 2)
        {
            GUI_RadioButtonSetPressed(&gba_mapviewer_window_gui,
                                      &gba_mapview_bg2_radbtn);
        }
        else if (gba_mapview_selected_bg == 3)
        {
            GUI_RadioButtonSetPressed(&gba_mapviewer_window_gui,
                                      &gba_mapview_bg3_radbtn);
        }
    }

    GUI_RadioButtonSetEnabled(&gba_mapview_bg0_radbtn, bgenabled[mode][0]);
    GUI_RadioButtonSetEnabled(&gba_mapview_bg1_radbtn, bgenabled[mode][1]);
    GUI_RadioButtonSetEnabled(&gba_mapview_bg2_radbtn, bgenabled[mode][2]);
    GUI_RadioButtonSetEnabled(&gba_mapview_bg3_radbtn, bgenabled[mode][3]);

    //--------------------------------------------------------------------------

    uint16_t control = 0;

    switch (gba_mapview_selected_bg)
    {
        case 0:
            control = REG_BG0CNT;
            break;
        case 1:
            control = REG_BG1CNT;
            break;
        case 2:
            control = REG_BG2CNT;
            break;
        case 3:
            control = REG_BG3CNT;
            break;
        default:
            return;
    }

    // 1 = text, 2 = affine, 3,4,5 = bmp mode 3,4,5
    static const uint32_t bgtypearray[8][4] = {
        {1, 1, 1, 1}, {1, 1, 2, 0}, {0, 0, 2, 2}, {0, 0, 3, 0},
        {0, 0, 4, 0}, {0, 0, 5, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}
    };
    gba_mapview_bgmode = bgtypearray[mode][gba_mapview_selected_bg];

    gba_mapview_bgcontrolreg = control;

    if (gba_mapview_bgmode == 0) // We shouldn't get here...
    {
        gba_mapview_sizex = gba_mapview_sizey = 0;
        GUI_ConsoleModePrintf(&gba_mapview_con, 0, 0,
                              "Size: ------- (Unused)\n"
                              "Colors: ---\n"
                              "Priority: -\n"
                              "Wrap: -\n"
                              "Char Base: ----------\n"
                              "Scrn Base: ----------\n"
                              "Mosaic: -");
        GUI_ConsoleModePrintf(&gba_mapview_tileinfo_con, 0, 0,
                              "Tile: --- (--)\n"
                              "Pos: ---------\n"
                              "Pal: --\n"
                              "Ad: ----------\n");

        gba_mapview_sizex = 0;
        gba_mapview_sizey = 0;
    }
    else if (gba_mapview_bgmode == 1) // Text
    {
        static const uint32_t text_bg_size[4][2] = {
            { 256, 256 }, { 512, 256 }, { 256, 512 }, { 512, 512 }
        };

        uint32_t CBB = (((control >> 2) & 3) * (16 * 1024)) + 0x06000000;
        uint32_t SBB = (((control >> 8) & 0x1F) * (2 * 1024)) + 0x06000000;
        gba_mapview_sizex = text_bg_size[control >> 14][0];
        gba_mapview_sizey = text_bg_size[control >> 14][1];
        int mosaic = (control & BIT(6)) != 0; // Mosaic
        int colors = (control & BIT(7)) ? 256 : 16;
        int prio = control & 3;
        GUI_ConsoleModePrintf(&gba_mapview_con, 0, 0,
                              "Size: %dx%d (Text)\n"
                              "Colors: %d\n"
                              "Priority: %d\n"
                              "Wrap: 1\n"
                              "Char Base: 0x%08X\n"
                              "Scrn Base: 0x%08X\n"
                              "Mosaic: %d",
                              gba_mapview_sizex, gba_mapview_sizey, colors,
                              prio, CBB, SBB, mosaic);
    }
    else if (gba_mapview_bgmode == 2) // Affine
    {
        static const uint32_t affine_bg_size[4] = {
            128, 256, 512, 1024
        };

        uint32_t CBB = (((control >> 2) & 3) * (16 * 1024)) + 0x06000000;
        uint32_t SBB = (((control >> 8) & 0x1F) * (2 * 1024)) + 0x06000000;
        gba_mapview_sizex = affine_bg_size[control >> 14];
        gba_mapview_sizey = gba_mapview_sizex;
        int mosaic = (control & BIT(6)) != 0; // mosaic
        int wrap = (control & BIT(13)) != 0;
        int prio = control & 3;
        GUI_ConsoleModePrintf(&gba_mapview_con, 0, 0,
                              "Size: %dx%d (Affine)\n"
                              "Colors: 256\n"
                              "Priority: %d\n"
                              "Wrap: %d\n"
                              "Char Base: 0x%08X\n"
                              "Scrn Base: 0x%08X\n"
                              "Mosaic: %d",
                              gba_mapview_sizex, gba_mapview_sizey, prio, wrap,
                              CBB, SBB, mosaic);
    }
    else if (gba_mapview_bgmode == 3) // BG2 mode 3
    {
        int mosaic = (control & BIT(6)) != 0; // Mosaic
        int prio = control & 3;
        gba_mapview_sizex = 240;
        gba_mapview_sizey = 160;
        GUI_ConsoleModePrintf(&gba_mapview_con, 0, 0,
                              "Size: 240x160 (Bitmap)\n"
                              "Colors: 16bit\n"
                              "Priority: %d\n"
                              "Wrap: 0\n"
                              "Char Base: ----------\n"
                              "Scrn Base: 0x06000000\n"
                              "Mosaic: %d",
                              prio, mosaic);
        GUI_ConsoleModePrintf(&gba_mapview_tileinfo_con, 0, 0,
                              "Tile: --- (--)\n"
                              "Pos: ---------\n"
                              "Pal: --\n"
                              "Ad: ----------\n");
    }
    else if (gba_mapview_bgmode == 4) // BG2 mode 4
    {
        int mosaic = (control & BIT(6)) != 0; // mosaic
        int prio = control & 3;
        uint32_t SBB = 0x06000000 + (gba_mapview_selected_bitmap_page ? 0xA000 : 0);
        gba_mapview_sizex = 240;
        gba_mapview_sizey = 160;
        GUI_ConsoleModePrintf(&gba_mapview_con, 0, 0,
                              "Size: 240x160 (Bitmap)\n"
                              "Colors: 256\n"
                              "Priority: %d\n"
                              "Wrap: 0\n"
                              "Char Base: ----------\n"
                              "Scrn Base: 0x%08X\n"
                              "Mosaic: %d",
                              prio, SBB, mosaic);
        GUI_ConsoleModePrintf(&gba_mapview_tileinfo_con, 0, 0,
                              "Tile: --- (--)\n"
                              "Pos: ---------\n"
                              "Pal: --\n"
                              "Ad: ----------\n");
    }
    else if (gba_mapview_bgmode == 5) // BG2 mode 5
    {
        int mosaic = (control & BIT(6)) != 0; // Mosaic
        int prio = control & 3;
        uint32_t SBB = 0x06000000 + (gba_mapview_selected_bitmap_page ? 0xA000 : 0);
        gba_mapview_sizex = 160;
        gba_mapview_sizey = 128;
        GUI_ConsoleModePrintf(&gba_mapview_con, 0, 0,
                              "Size: 160x128 (Bitmap)\n"
                              "Colors: 16bit\n"
                              "Priority: %d\n"
                              "Wrap: 0\n"
                              "Char Base: ----------\n"
                              "Scrn Base: 0x%08X\n"
                              "Mosaic: %d",
                              prio, SBB, mosaic);
        GUI_ConsoleModePrintf(&gba_mapview_tileinfo_con, 0, 0,
                              "Tile: --- (--)\n"
                              "Pos: ---------\n"
                              "Pal: --\n"
                              "Ad: ----------\n");
    }

    GBA_Debug_PrintBackgroundAlpha(gba_complete_map_buffer, 1024, 1024, control,
                                   gba_mapview_bgmode,
                                   gba_mapview_selected_bitmap_page);

    //--------------------------------------------------

    gba_mapview_scrollx_max = (((int)gba_mapview_sizex) - 256) / 8;
    gba_mapview_scrolly_max = (((int)gba_mapview_sizey) - 256) / 8;

    if (gba_mapview_scrollx_max < 0)
        gba_mapview_scrollx_max = 0;
    if (gba_mapview_scrolly_max < 0)
        gba_mapview_scrolly_max = 0;

    if (gba_mapview_scrollx > gba_mapview_scrollx_max)
        gba_mapview_scrollx = gba_mapview_scrollx_max;
    if (gba_mapview_scrolly > gba_mapview_scrolly_max)
        gba_mapview_scrolly = gba_mapview_scrolly_max;

    GUI_SetScrollBar(&gba_mapview_scrollx_scrollbar,
                     76 + 15 * FONT_WIDTH + 6, 262,
                     256, 12, 0, gba_mapview_scrollx_max, gba_mapview_scrollx,
                     _win_gba_mapviewer_scrollbar_x_callback);
    GUI_SetScrollBar(&gba_mapview_scrolly_scrollbar,
                     76 + 15 * FONT_WIDTH + 6 + GBA_MAP_BUFFER_WIDTH, 6,
                     12, 256, 0, gba_mapview_scrolly_max, gba_mapview_scrolly,
                     _win_gba_mapviewer_scrollbar_y_callback);

    _win_gba_mapviewer_update_mapbuffer_from_temp_buffer();

    if (gba_mapview_selected_tilex >= (gba_mapview_sizex / 8))
        gba_mapview_selected_tilex = 0;
    if (gba_mapview_selected_tiley >= (gba_mapview_sizey / 8))
        gba_mapview_selected_tiley = 0;

    //--------------------------------------------------

    int tiletempbuffer[8 * 8], tiletempvis[8 * 8];
    memset(tiletempvis, 0, sizeof(tiletempvis));

    if (gba_mapview_bgmode == 1)
    {
        uint8_t *charbaseblockptr =
                (uint8_t *)MEM_VRAM + ((control >> 2) & 3) * (16 * 1024);
        uint16_t *scrbaseblockptr =
                (uint16_t *)((uint8_t *)MEM_VRAM + ((control >> 8) & 0x1F) * (2 * 1024));

        uint32_t TileSEIndex = se_index(gba_mapview_selected_tilex,
                                        gba_mapview_selected_tiley,
                                        gba_mapview_sizex / 8);

        uint16_t SE = scrbaseblockptr[TileSEIndex];

        if (control & BIT(7)) // 256 Colors
        {
            uint8_t *data = (uint8_t *)&(charbaseblockptr[(SE & 0x3FF) * 64]);

            for (int i = 0; i < 8; i++)
            {
                for (int j = 0; j < 8; j++)
                {
                    uint8_t dat_ = data[j * 8 + i];
                    uint32_t color = rgb16to32(MEM_PALETTE_BG[dat_]);
                    tiletempbuffer[j * 8 + i] = color;
                    tiletempvis[j * 8 + i] = dat_;
                }
            }

            uint32_t address = ((control >> 8) & 0x1F) * (2 * 1024) + TileSEIndex
                             + 0x06000000;

            GUI_ConsoleModePrintf(&gba_mapview_tileinfo_con, 0, 0,
                                  "Tile: %d (%s%s)\n"
                                  "Pos: %d,%d\n"
                                  "Pal: --\n"
                                  "Ad: 0x%08X\n",
                                  SE & 0x3FF, (SE & BIT(10)) ? "H" : "-",
                                  (SE & BIT(11)) ? "V" : "-",
                                  gba_mapview_selected_tilex,
                                  gba_mapview_selected_tiley, address);
        }
        else // 16 Colors
        {
            uint8_t *data = (uint8_t *)&(charbaseblockptr[(SE & 0x3FF) * 32]);
            uint16_t *palptr = (uint16_t *)&MEM_PALETTE_BG[(SE >> 12) * 16];

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

            uint32_t address = ((control >> 8) & 0x1F) * (2 * 1024) + TileSEIndex
                          + 0x06000000;

            GUI_ConsoleModePrintf(&gba_mapview_tileinfo_con, 0, 0,
                                  "Tile: %d (%s%s)\n"
                                  "Pos: %d,%d\n"
                                  "Pal: %d\n"
                                  "Ad: 0x%08X\n",
                                  SE & 0x3FF, (SE & BIT(10)) ? "H" : "-",
                                  (SE & BIT(11)) ? "V" : "-",
                                  gba_mapview_selected_tilex,
                                  gba_mapview_selected_tiley,
                                  (SE >> 12) & 0xF, address);
        }
    }
    else if (gba_mapview_bgmode == 2)
    {
        uint8_t *charbaseblockptr =
                (uint8_t *)MEM_VRAM + ((control >> 2) & 3) * (16 * 1024);
        uint8_t *scrbaseblockptr =
                (uint8_t *)MEM_VRAM + ((control >> 8) & 0x1F) * (2 * 1024);

        uint32_t TileSEIndex = se_index_affine(gba_mapview_selected_tilex,
                                          gba_mapview_selected_tiley,
                                          gba_mapview_sizex / 8);

        uint16_t SE = scrbaseblockptr[TileSEIndex];
        uint8_t *data = (uint8_t *)&(charbaseblockptr[SE * 64]);

        // 256 colors always
        for (int i = 0; i < 8; i++)
        {
            for (int j = 0; j < 8; j++)
            {
                uint8_t dat_ = data[j * 8 + i];
                uint32_t color = rgb16to32(MEM_PALETTE_BG[dat_]);
                tiletempbuffer[j * 8 + i] = color;
                tiletempvis[j * 8 + i] = dat_;
            }
        }

        uint32_t address = ((control >> 8) & 0x1F) * (2 * 1024)
                         + TileSEIndex + 0x06000000;

        GUI_ConsoleModePrintf(&gba_mapview_tileinfo_con, 0, 0,
                              "Tile: %d (--)\n"
                              "Pos: %d,%d\n"
                              "Pal: --\n"
                              "Ad: 0x%08X\n",
                              SE & 0x3FF, gba_mapview_selected_tilex,
                              gba_mapview_selected_tiley, address);
    }
    else
    {
        GUI_ConsoleModePrintf(&gba_mapview_tileinfo_con, 0, 0,
                              "Tile: --- (--)\n"
                              "Pos: ---------\n"
                              "Pal: --\n"
                              "Ad: ----------\n");

        memset(tiletempvis, 0, sizeof(tiletempvis));
    }

    // Expand to 64x64
    for (int i = 0; i < 64; i++)
    {
        for (int j = 0; j < 64; j++)
        {
            unsigned char v = ((i & 16) ^ (j & 16)) ? 0x80 : 0xB0;
            gba_map_zoomed_tile_buffer[(j * 64 + i) * 3 + 0] = v;
            gba_map_zoomed_tile_buffer[(j * 64 + i) * 3 + 1] = v;
            gba_map_zoomed_tile_buffer[(j * 64 + i) * 3 + 2] = v;
        }
    }

    for (int i = 0; i < 64; i++)
    {
        for (int j = 0; j < 64; j++)
        {
            if (tiletempvis[(j / 8) * 8 + (i / 8)])
            {
                uint32_t color = tiletempbuffer[(j / 8) * 8 + (i / 8)];
                int index = (j * 64 + i) * 3;
                gba_map_zoomed_tile_buffer[index + 0] = color & 0xFF;
                gba_map_zoomed_tile_buffer[index + 1] = (color >> 8) & 0xFF;
                gba_map_zoomed_tile_buffer[index + 2] = (color >> 16) & 0xFF;
            }
        }
    }

    //--------------------------------------------------

    if ((gba_mapview_bgmode == 1) || (gba_mapview_bgmode == 2))
    {
        GUI_Draw_SetDrawingColor(255, 0, 0);

        int l = (gba_mapview_selected_tilex - gba_mapview_scrollx) * 8; // Left
        int t = (gba_mapview_selected_tiley - gba_mapview_scrolly) * 8; // Top
        int r = l + 7; // Right
        int b = t + 7; // Bottom

        if ((l >= 0) && (r <= 255) && (t >= 0) && (b <= 255))
        {
            GUI_Draw_Rect(gba_map_buffer,
                          GBA_MAP_BUFFER_WIDTH, GBA_MAP_BUFFER_HEIGHT,
                          l, r, t, b);
        }
    }
}

//----------------------------------------------------------------

static void _win_gba_map_viewer_render(void)
{
    if (GBAMapViewerCreated == 0)
        return;

    GUI_Draw(&gba_mapviewer_window_gui, gba_mapviewer_buffer,
             WIN_GBA_MAPVIEWER_WIDTH, WIN_GBA_MAPVIEWER_HEIGHT, 1);

    WH_Render(WinIDGBAMapViewer, gba_mapviewer_buffer);
}

static int _win_gba_map_viewer_callback(SDL_Event *e)
{
    if (GBAMapViewerCreated == 0)
        return 1;

    int redraw = GUI_SendEvent(&gba_mapviewer_window_gui, e);

    int close_this = 0;

    if (e->type == SDL_WINDOWEVENT)
    {
        if (e->window.event == SDL_WINDOWEVENT_FOCUS_GAINED)
        {
            redraw = 1;
        }
        else if (e->window.event == SDL_WINDOWEVENT_EXPOSED)
        {
            redraw = 1;
        }
        else if (e->window.event == SDL_WINDOWEVENT_CLOSE)
        {
            close_this = 1;
        }
    }
    else if (e->type == SDL_KEYDOWN)
    {
        if (e->key.keysym.sym == SDLK_ESCAPE)
        {
            close_this = 1;
        }
    }

    if (close_this)
    {
        GBAMapViewerCreated = 0;
        WH_Close(WinIDGBAMapViewer);
        return 1;
    }

    if (redraw)
    {
        Win_GBAMapViewerUpdate();
        _win_gba_map_viewer_render();
        return 1;
    }

    return 0;
}

//----------------------------------------------------------------

static void _win_gba_mapviewer_dump_btn_callback(void)
{
    unsigned char *buf = malloc(gba_mapview_sizex * gba_mapview_sizey * 4);
    if (buf == NULL)
    {
        Debug_Log("%s(): Not enough memory.");
        return;
    }

    for (uint32_t j = 0; j < gba_mapview_sizey; j++)
    {
        for (uint32_t i = 0; i < gba_mapview_sizex; i++)
        {
            int dst_index = (j * gba_mapview_sizex + i) * 4;
            int src_index = (j * 1024 + i) * 4;

            buf[dst_index + 0] = gba_complete_map_buffer[src_index + 0];
            buf[dst_index + 1] = gba_complete_map_buffer[src_index + 1];
            buf[dst_index + 2] = gba_complete_map_buffer[src_index + 2];
            buf[dst_index + 3] = gba_complete_map_buffer[src_index + 3];
        }
    }

    char *name = FU_GetNewTimestampFilename("gba_map");
    Save_PNG(name, buf, gba_mapview_sizex, gba_mapview_sizey, 1);

    free(buf);

    Win_GBAMapViewerUpdate();
}

//----------------------------------------------------------------

int Win_GBAMapViewerCreate(void)
{
    if (GBAMapViewerCreated == 1)
    {
        WH_Focus(WinIDGBAMapViewer);
        return 0;
    }

    gba_mapviewer_buffer = calloc(1, WIN_GBA_MAPVIEWER_WIDTH *
                                  WIN_GBA_MAPVIEWER_HEIGHT * 3);
    if (gba_mapviewer_buffer == NULL)
    {
        Debug_Log("%s(): Not enough memory.");
        return 0;
    }

    GUI_SetLabel(&gba_mapview_bg_label,
                 6, 6, 10 * FONT_WIDTH, FONT_HEIGHT,
                 "Background");
    GUI_SetRadioButton(&gba_mapview_bg0_radbtn, 6, 24, 15 * FONT_WIDTH, 18,
                       "Background 0", 0, 0, 1,
                       _win_gba_mapviewer_bgnum_radbtn_callback);
    GUI_SetRadioButton(&gba_mapview_bg1_radbtn, 6, 44, 15 * FONT_WIDTH, 18,
                       "Background 1", 0, 1, 0,
                       _win_gba_mapviewer_bgnum_radbtn_callback);
    GUI_SetRadioButton(&gba_mapview_bg2_radbtn, 6, 64, 15 * FONT_WIDTH, 18,
                      "Background 2", 0, 2, 0,
                      _win_gba_mapviewer_bgnum_radbtn_callback);
    GUI_SetRadioButton(&gba_mapview_bg3_radbtn, 6, 84, 15 * FONT_WIDTH, 18,
                      "Background 3", 0, 3, 0,
                      _win_gba_mapviewer_bgnum_radbtn_callback);

    GUI_SetLabel(&gba_mapview_page_label,
                 12 + 15 * FONT_WIDTH, 6, 4 * FONT_WIDTH, FONT_HEIGHT,
                 "Page");
    GUI_SetRadioButton(&gba_mapview_page0_radbtn,
                       12 + 15 * FONT_WIDTH, 24, 8 * FONT_WIDTH, 18,
                       "Page 0", 1, 0, 1,
                       _win_gba_mapviewer_pagenum_radbtn_callback);
    GUI_SetRadioButton(&gba_mapview_page1_radbtn,
                       12 + 15 * FONT_WIDTH, 44, 8 * FONT_WIDTH, 18,
                       "Page 1", 1, 1, 0,
                       _win_gba_mapviewer_pagenum_radbtn_callback);

    GUI_SetBitmap(&gba_mapview_zoomed_tile_bmp,
                  6, 198, 64, 64, gba_map_zoomed_tile_buffer, NULL);

    GUI_SetButton(&gba_mapview_dumpbtn,
                  19 + 15 * FONT_WIDTH, 71, FONT_WIDTH * 6, FONT_HEIGHT * 2,
                  "Dump", _win_gba_mapviewer_dump_btn_callback);

    GUI_SetTextBox(&gba_mapview_textbox, &gba_mapview_con,
                   6, 108, 25 * FONT_WIDTH, 7 * FONT_HEIGHT, NULL);

    GUI_SetTextBox(&gba_mapview_tileinfo_textbox, &gba_mapview_tileinfo_con,
                   76, 198, 15 * FONT_WIDTH, 4 * FONT_HEIGHT, NULL);

    GUI_SetBitmap(&gba_mapview_tiles_bmp, 76 + 15 * FONT_WIDTH + 6, 6,
                  GBA_MAP_BUFFER_WIDTH, GBA_MAP_BUFFER_HEIGHT, gba_map_buffer,
                  _win_gba_mapviewer_tiles_bmp_callback);
    GUI_SetScrollBar(&gba_mapview_scrollx_scrollbar,
                     76 + 15 * FONT_WIDTH + 6, 262, 256, 12,
                     0, 0, 0, _win_gba_mapviewer_scrollbar_x_callback);
    GUI_SetScrollBar(&gba_mapview_scrolly_scrollbar,
                     76 + 15 * FONT_WIDTH + 6 + GBA_MAP_BUFFER_WIDTH, 6,
                     12, 256, 0, 0, 0, _win_gba_mapviewer_scrollbar_y_callback);

    gba_mapview_selected_bg = 0;
    gba_mapview_selected_tilex = 0;
    gba_mapview_selected_tiley = 0;
    gba_mapview_selected_bitmap_page = 0;

    gba_mapview_sizex = 0;
    gba_mapview_sizey = 0;
    gba_mapview_bgmode = 0;
    gba_mapview_bgcontrolreg = 0;

    gba_mapview_scrollx = 0;
    gba_mapview_scrolly = 0;
    gba_mapview_scrollx_max = 0;
    gba_mapview_scrolly_max = 0;

    GBAMapViewerCreated = 1;

    WinIDGBAMapViewer = WH_Create(WIN_GBA_MAPVIEWER_WIDTH,
                                  WIN_GBA_MAPVIEWER_HEIGHT, 0, 0, 0);
    WH_SetCaption(WinIDGBAMapViewer, "GBA Map Viewer");

    WH_SetEventCallback(WinIDGBAMapViewer, _win_gba_map_viewer_callback);

    Win_GBAMapViewerUpdate();
    _win_gba_map_viewer_render();

    return 1;
}

void Win_GBAMapViewerClose(void)
{
    if (GBAMapViewerCreated == 0)
        return;

    free(gba_mapviewer_buffer);
    gba_mapviewer_buffer = NULL;

    GBAMapViewerCreated = 0;
    WH_Close(WinIDGBAMapViewer);
}
