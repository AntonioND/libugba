// SPDX-License-Identifier: LGPL-3.0-only
//
// Copyright (c) 2011-2015, 2019-2021, Antonio Niño Díaz

#ifndef SDL2_GUI_DEBUGGER_WIN_UTILS_DRAW_H__
#define SDL2_GUI_DEBUGGER_WIN_UTILS_DRAW_H__

#include "win_utils.h"

#define GUI_BACKGROUND_GREY      (216)
#define GUI_BACKGROUND_GREY_RGBA ((0xFF << 24) | (GUI_BACKGROUND_GREY << 16) | \
                                  (GUI_BACKGROUND_GREY << 8) | \
                                  (GUI_BACKGROUND_GREY))

#define GUI_WINDOWBAR_GREY      (232)
#define GUI_WINDOWBAR_GREY_RGBA ((0xFF << 24) | (GUI_WINDOWBAR_GREY << 16) | \
                                 (GUI_WINDOWBAR_GREY << 8) | \
                                 (GUI_WINDOWBAR_GREY))

void GUI_Draw_SetDrawingColor(int r, int g, int b);
void GUI_Draw_HorizontalLine(unsigned char *buffer, int w, int h,
                             int x1, int x2, int y);
void GUI_Draw_VerticalLine(unsigned char *buffer, int w, int h,
                           int x, int y1, int y2);
void GUI_Draw_Rect(unsigned char *buffer, int w, int h,
                   int x1, int x2, int y1, int y2);
void GUI_Draw_FillRect(unsigned char *buffer, int w, int h,
                       int x1, int x2, int y1, int y2);

void GUI_Draw(gui_global *gui, unsigned char *buffer, int w, int h, int clean);

#endif // SDL2_GUI_DEBUGGER_WIN_UTILS_DRAW_H__
