// SPDX-License-Identifier: LGPL-3.0-only
//
// Copyright (c) 2011-2015, 2019-2021, Antonio Niño Díaz

#ifndef SDL2_GUI_DEBUGGER_WIN_UTILS_EVENTS_H__
#define SDL2_GUI_DEBUGGER_WIN_UTILS_EVENTS_H__

#include "win_utils.h"

int GUI_SendEvent(gui_global *gui, SDL_Event *e); // Returns 1 if needed to redraw

#endif // SDL2_GUI_DEBUGGER_WIN_UTILS_EVENTS_H__
