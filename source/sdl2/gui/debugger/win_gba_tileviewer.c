// SPDX-License-Identifier: LGPL-3.0-only
//
// Copyright (c) 2011-2015, 2019-2021, Antonio Niño Díaz

#include <string.h>

#include <SDL2/SDL.h>

#include "../../debug_utils.h"
#include "../../png_utils.h"

#include "../font_utils.h"
#include "../win_utils.h"
#include "../window_handler.h"

#include "file_utils.h"
#include "gba_debug_video.h"
#include "win_gba_debugger.h"

//------------------------------------------------------------------------------

static int WinIDGBATileViewer;

#define WIN_GBA_TILEVIEWER_WIDTH  449
#define WIN_GBA_TILEVIEWER_HEIGHT 268

static int GBATileViewerCreated = 0;

static unsigned char *gba_tileviewer_buffer = NULL;

//------------------------------------------------------------------------------

static uint32_t gba_tileview_selected_pal = 0;
static uint32_t gba_tileview_selected_cbb = 0;
static uint32_t gba_tileview_selected_colors = 0;
static uint32_t gba_tileview_selected_index = 0;

#define GBA_TILE_BUFFER_WIDTH  (32 * 8)
#define GBA_TILE_BUFFER_HEIGHT (32 * 8)

static unsigned char
    gba_tile_buffer[GBA_TILE_BUFFER_WIDTH * GBA_TILE_BUFFER_HEIGHT * 3];

static unsigned char gba_tile_zoomed_tile_buffer[64 * 64 * 3];

//------------------------------------------------------------------------------

static gui_console gba_tileview_con;
static gui_element gba_tileview_textbox;

static gui_element gba_tileview_cbb_label;
static gui_element gba_tileview_cbb_06000000_radbtn,
                   gba_tileview_cbb_06004000_radbtn,
                   gba_tileview_cbb_06008000_radbtn,
                   gba_tileview_cbb_0600C000_radbtn,
                   gba_tileview_cbb_06010000_radbtn,
                   gba_tileview_cbb_06014000_radbtn;

static gui_element gba_tileview_colornum_label;
static gui_element gba_tileview_colornum_16_radbtn,
                   gba_tileview_colornum_256_radbtn;

static gui_element gba_tileview_zoomed_tile_bmp;

static gui_element gba_tileview_dumpbtn;

static gui_element gba_tileview_tiles_bmp;

static gui_element gba_tileview_pal_scrollbar;

static gui_element *gba_tileviwer_window_gui_elements[] = {
    &gba_tileview_cbb_label,
    &gba_tileview_cbb_06000000_radbtn, &gba_tileview_cbb_06004000_radbtn,
    &gba_tileview_cbb_06008000_radbtn, &gba_tileview_cbb_0600C000_radbtn,
    &gba_tileview_cbb_06010000_radbtn, &gba_tileview_cbb_06014000_radbtn,
    &gba_tileview_colornum_label,
    &gba_tileview_colornum_16_radbtn, &gba_tileview_colornum_256_radbtn,
    &gba_tileview_zoomed_tile_bmp,
    &gba_tileview_dumpbtn,
    &gba_tileview_textbox,
    &gba_tileview_tiles_bmp,
    &gba_tileview_pal_scrollbar,
    NULL
};

static gui_global gba_tileviewer_window_gui = {
    gba_tileviwer_window_gui_elements,
    NULL,
    NULL
};

//----------------------------------------------------------------

static int _win_gba_tileviewer_tiles_bmp_callback(int x, int y)
{
    gba_tileview_selected_index = (x / 8) + ((y / 8) * 32);
    return 1;
}

static void _win_gba_tileviewer_pal_scrollbar_callback(int value)
{
    gba_tileview_selected_pal = value;
}

static void _win_gba_tileviewer_cbb_radbtn_callback(int num)
{
    switch (num)
    {
        case 0:
            gba_tileview_selected_cbb = 0x06000000;
            break;
        case 1:
            gba_tileview_selected_cbb = 0x06004000;
            break;
        case 2:
            gba_tileview_selected_cbb = 0x06008000;
            break;
        case 3:
            gba_tileview_selected_cbb = 0x0600C000;
            break;
        case 4:
            gba_tileview_selected_cbb = 0x06010000;
            break;
        case 5:
            gba_tileview_selected_cbb = 0x06014000;
            break;
        default:
            gba_tileview_selected_cbb = 0x06000000;
            break;
    }
}

static void _win_gba_tileviewer_colors_radbtn_callback(int num)
{
    if (num == 1)
        gba_tileview_selected_colors = 256;
    else
        gba_tileview_selected_colors = 16;
}

//----------------------------------------------------------------

void Win_GBATileViewerUpdate(void)
{
    if (GBATileViewerCreated == 0)
        return;

    GUI_ConsoleClear(&gba_tileview_con);

    uint32_t address = gba_tileview_selected_cbb
                     + ((gba_tileview_selected_colors == 16) ?
                        (gba_tileview_selected_index & 0x3FF) * 32 :
                        (gba_tileview_selected_index & 0x3FF) * 64);

    GUI_ConsoleModePrintf(&gba_tileview_con, 0, 0,
                          "Tile: %d\n"
                          "Addr: %08X\n"
                          "Pal: %d",
                          gba_tileview_selected_index, address,
                          gba_tileview_selected_pal);

    GBA_Debug_PrintTiles(gba_tile_buffer,
                         GBA_TILE_BUFFER_WIDTH, GBA_TILE_BUFFER_HEIGHT,
                         gba_tileview_selected_cbb,
                         gba_tileview_selected_colors,
                         gba_tileview_selected_pal);

    GUI_Draw_SetDrawingColor(255, 0, 0);
    int l = (gba_tileview_selected_index % 32) * 8; // Left
    int t = (gba_tileview_selected_index / 32) * 8; // Top
    int r = l + 7;                                  // Right
    int b = t + 7;                                  // Bottom
    GUI_Draw_Rect(gba_tile_buffer,
                  GBA_TILE_BUFFER_WIDTH, GBA_TILE_BUFFER_HEIGHT,
                  l, r, t, b);

    GBA_Debug_TilePrint64x64(gba_tile_zoomed_tile_buffer, 64, 64,
                             gba_tileview_selected_cbb,
                             gba_tileview_selected_index,
                             gba_tileview_selected_colors,
                             gba_tileview_selected_pal);
}

//----------------------------------------------------------------

static void _win_gba_tile_viewer_render(void)
{
    if (GBATileViewerCreated == 0)
        return;

    GUI_Draw(&gba_tileviewer_window_gui, gba_tileviewer_buffer,
             WIN_GBA_TILEVIEWER_WIDTH, WIN_GBA_TILEVIEWER_HEIGHT, 1);

    WH_Render(WinIDGBATileViewer, gba_tileviewer_buffer);
}

static int _win_gba_tile_viewer_callback(SDL_Event *e)
{
    if (GBATileViewerCreated == 0)
        return 1;

    int redraw = GUI_SendEvent(&gba_tileviewer_window_gui, e);

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
        GBATileViewerCreated = 0;
        WH_Close(WinIDGBATileViewer);
        return 1;
    }

    if (redraw)
    {
        Win_GBATileViewerUpdate();
        _win_gba_tile_viewer_render();
        return 1;
    }

    return 0;
}

//----------------------------------------------------------------

static void _win_gba_tileviewer_dump_btn_callback(void)
{
    unsigned char *buf =
        malloc(GBA_TILE_BUFFER_WIDTH * GBA_TILE_BUFFER_HEIGHT * 4);
    if (buf == NULL)
    {
        Debug_Log("%s(): Not enough memory.");
        return;
    }

    GBA_Debug_PrintTilesAlpha(buf,
                              GBA_TILE_BUFFER_WIDTH, GBA_TILE_BUFFER_HEIGHT,
                              gba_tileview_selected_cbb,
                              gba_tileview_selected_colors,
                              gba_tileview_selected_pal);

    char *name = FU_GetNewTimestampFilename("gba_tiles");
    Save_PNG(name, buf, GBA_TILE_BUFFER_WIDTH, GBA_TILE_BUFFER_HEIGHT, 1);

    free(buf);

    Win_GBATileViewerUpdate();
}

//----------------------------------------------------------------

int Win_GBATileViewerCreate(void)
{
    if (GBATileViewerCreated == 1)
    {
        WH_Focus(WinIDGBATileViewer);
        return 0;
    }

    gba_tileviewer_buffer = calloc(1, WIN_GBA_TILEVIEWER_WIDTH *
                                   WIN_GBA_TILEVIEWER_HEIGHT * 3);
    if (gba_tileviewer_buffer == NULL)
    {
        Debug_Log("%s(): Not enough memory.");
        return 0;
    }

    GUI_SetLabel(&gba_tileview_cbb_label, 6, 6, 20 * FONT_WIDTH, FONT_HEIGHT,
                 "Character Base Block");
    GUI_SetRadioButton(&gba_tileview_cbb_06000000_radbtn, 6, 24,
                       25 * FONT_WIDTH, 18,
                       "0x06000000 - BG         ", 0, 0, 1,
                       _win_gba_tileviewer_cbb_radbtn_callback);
    GUI_SetRadioButton(&gba_tileview_cbb_06004000_radbtn, 6, 44,
                       25 * FONT_WIDTH, 18,
                       "0x06004000 - BG         ", 0, 1, 0,
                       _win_gba_tileviewer_cbb_radbtn_callback);
    GUI_SetRadioButton(&gba_tileview_cbb_06008000_radbtn, 6, 64,
                       25 * FONT_WIDTH, 18,
                       "0x06008000 - BG         ", 0, 2, 0,
                       _win_gba_tileviewer_cbb_radbtn_callback);
    GUI_SetRadioButton(&gba_tileview_cbb_0600C000_radbtn, 6, 84,
                       25 * FONT_WIDTH, 18,
                       "0x0600C000 - BG         ", 0, 3, 0,
                       _win_gba_tileviewer_cbb_radbtn_callback);
    GUI_SetRadioButton(&gba_tileview_cbb_06010000_radbtn, 6, 104,
                       25 * FONT_WIDTH, 18,
                       "0x06010000 - OBJ/BG     ", 0, 4, 0,
                       _win_gba_tileviewer_cbb_radbtn_callback);
    GUI_SetRadioButton(&gba_tileview_cbb_06014000_radbtn, 6, 124,
                       25 * FONT_WIDTH, 18,
                       "0x06014000 - (OBJ)      ", 0, 5, 0,
                       _win_gba_tileviewer_cbb_radbtn_callback);

    GUI_SetLabel(&gba_tileview_colornum_label,
                 6, 148, 6 * FONT_WIDTH, FONT_HEIGHT, "Colors");
    GUI_SetRadioButton(&gba_tileview_colornum_16_radbtn,
                       6, 166, 8 * FONT_WIDTH, 18,
                       "16", 1, 0, 1,
                       _win_gba_tileviewer_colors_radbtn_callback);
    GUI_SetRadioButton(&gba_tileview_colornum_256_radbtn,
                       12 + 8 * FONT_WIDTH, 166, 8 * FONT_WIDTH, 18,
                       "256", 1, 1, 0, _win_gba_tileviewer_colors_radbtn_callback);

    GUI_SetBitmap(&gba_tileview_zoomed_tile_bmp, 6, 198, 64, 64,
                  gba_tile_zoomed_tile_buffer, NULL);

    GUI_SetButton(&gba_tileview_dumpbtn,
                  139, 160, FONT_WIDTH * 6, FONT_HEIGHT * 2, "Dump",
                  _win_gba_tileviewer_dump_btn_callback);

    GUI_SetTextBox(&gba_tileview_textbox, &gba_tileview_con,
                   76, 198, 15 * FONT_WIDTH, 3 * FONT_HEIGHT, NULL);

    GUI_SetScrollBar(&gba_tileview_pal_scrollbar,
                     75, 250, 2 + 15 * FONT_WIDTH, 12,
                     0, 15, 0, _win_gba_tileviewer_pal_scrollbar_callback);

    GUI_SetBitmap(&gba_tileview_tiles_bmp, 76 + 15 * FONT_WIDTH + 6, 6,
                  GBA_TILE_BUFFER_WIDTH, GBA_TILE_BUFFER_HEIGHT,
                  gba_tile_buffer, _win_gba_tileviewer_tiles_bmp_callback);

    GBATileViewerCreated = 1;

    gba_tileview_selected_pal = 0;
    gba_tileview_selected_cbb = 0x06000000;
    gba_tileview_selected_colors = 16;
    gba_tileview_selected_index = 0;

    WinIDGBATileViewer = WH_Create(WIN_GBA_TILEVIEWER_WIDTH,
                                   WIN_GBA_TILEVIEWER_HEIGHT, 0, 0, 0);
    WH_SetCaption(WinIDGBATileViewer, "GBA Tile Viewer");

    WH_SetEventCallback(WinIDGBATileViewer, _win_gba_tile_viewer_callback);

    Win_GBATileViewerUpdate();
    _win_gba_tile_viewer_render();

    return 1;
}

void Win_GBATileViewerClose(void)
{
    if (GBATileViewerCreated == 0)
        return;

    free(gba_tileviewer_buffer);
    gba_tileviewer_buffer = NULL;

    GBATileViewerCreated = 0;
    WH_Close(WinIDGBATileViewer);
}
