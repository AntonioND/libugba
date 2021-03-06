// SPDX-License-Identifier: LGPL-3.0-only
//
// Copyright (c) 2011-2015, 2019-2021, Antonio Niño Díaz

#ifndef SDL2_GUI_DEBUGGER_GBA_DEBUG_VIDEO_H__
#define SDL2_GUI_DEBUGGER_GBA_DEBUG_VIDEO_H__

void GBA_Debug_PrintZoomedSpriteAt(int spritenum, int buf_has_alpha_channel,
                                   unsigned char *buffer, int bufw, int bufh,
                                   int posx, int posy, int sizex, int sizey);

// starting sprite number = page * 64
void GBA_Debug_PrintSpritesPage(int page, int buf_has_alpha_channel,
                                unsigned char *buffer, int bufw, int bufh);

void GBA_Debug_PrintTiles(unsigned char *buffer, int bufw, int bufh, int cbb,
                          int colors, int palette);
void GBA_Debug_PrintTilesAlpha(unsigned char *buffer,
                               int bufw, int bufh, int cbb,
                               int colors, int palette);

void GBA_Debug_TilePrint64x64(unsigned char *buffer,
                              int bufw, int bufh, int cbb,
                              int tile, int palcolors, int selected_pal);

void GBA_Debug_PrintBackgroundAlpha(unsigned char *buffer, int bufw, int bufh,
                                    uint16_t control, int bgmode, int page);

#endif // SDL2_GUI_DEBUGGER_GBA_DEBUG_VIDEO_H__
