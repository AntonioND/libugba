// SPDX-License-Identifier: LGPL-3.0-only
//
// Copyright (c) 2021, Antonio Niño Díaz

#ifndef SDL2_GUI_WIN_CONFIG_H__
#define SDL2_GUI_WIN_CONFIG_H__

#include <SDL2/SDL.h>

int Win_ConfigEventCallback(SDL_Event *e);
void Win_ConfigDrawOverlay(unsigned char *dest_buffer);
int Win_ConfigCreate(void);

#endif // SDL2_GUI_WIN_CONFIG_H__
