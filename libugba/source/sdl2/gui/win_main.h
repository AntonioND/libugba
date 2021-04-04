// SPDX-License-Identifier: LGPL-3.0-only
//
// Copyright (c) 2011-2015, 2019-2021 Antonio Niño Díaz

#ifndef SDL2_GUI_WIN_MAIN_H__
#define SDL2_GUI_WIN_MAIN_H__

int Win_MainCreate(void); // returns 1 if error
void Win_MainRender(void);
void Win_MainSetZoom(int factor);
int Win_MainIsConfigEnabled(void);
void Win_MainLoopHandle(void);
void Win_MainExit(void);

#endif // SDL2_GUI_WIN_MAIN_H__
