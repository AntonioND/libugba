// SPDX-License-Identifier: LGPL-3.0-only
//
// Copyright (c) 2011-2015, 2019-2020 Antonio Niño Díaz

#ifndef SDL2_GUI_WINDOW_HANDLER_H__
#define SDL2_GUI_WINDOW_HANDLER_H__

typedef int (*WH_CallbackFn)(SDL_Event *);

void WH_Init(void);

// if scale = 0 texture will be scaled to window size. If not, centered and
// scaled to this factor. Returns -1 on error.
int WH_Create(int width, int height, int texw, int texh, int scale);

// if scale = 0 texture will be scaled to window size. If not, centered and
// scaled to this factor.
void WH_SetSize(int index, int width, int height, int texw, int texh,
                int scale);

// The callback function won't ever receive SDL_QUIT event
int WH_SetEventCallback(int index, WH_CallbackFn fn);
int WH_SetEventMainWindow(int index);

void WH_HandleEvents(void);

void WH_SetCaption(int index, const char *caption);

void WH_Render(int index, const unsigned char *buffer);

void WH_Close(int index);
void WH_CloseAllBut(int index);
void WH_CloseAllButMain(void);
void WH_CloseAll(void);

int WH_AreAllWindowsClosed(void);

void WH_Focus(int index);

int WH_GetWidth(int index);
int WH_GetHeight(int index);

int WH_HasMouseFocus(int index);
int WH_HasKeyboardFocus(int index);

int WH_IsShown(int index);

#endif // SDL2_GUI_WINDOW_HANDLER_H__
