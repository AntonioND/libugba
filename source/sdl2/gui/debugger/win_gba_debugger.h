// SPDX-License-Identifier: LGPL-3.0-only
//
// Copyright (c) 2011-2015, 2019-2021, Antonio Niño Díaz

#ifndef SDL2_GUI_DEBUGGER_WIN_GBA_DEBUGGER_H__
#define SDL2_GUI_DEBUGGER_WIN_GBA_DEBUGGER_H__

// I/O viewer
// ----------

int Win_GBAIOViewerCreate(void); // Returns 1 on error
void Win_GBAIOViewerUpdate(void);
void Win_GBAIOViewerRender(void);
void Win_GBAIOViewerClose(void);

// Map viewer
// ----------

int Win_GBAMapViewerCreate(void); // Returns 1 on error
void Win_GBAMapViewerUpdate(void);
void Win_GBAMapViewerClose(void);

// Tile viewer
// -----------

int Win_GBATileViewerCreate(void); // Returns 1 on error
void Win_GBATileViewerUpdate(void);
void Win_GBATileViewerClose(void);

// Sprite viewer
// -------------

int Win_GBASprViewerCreate(void); // Returns 1 on error
void Win_GBASprViewerUpdate(void);
void Win_GBASprViewerClose(void);

// Palette viewer
// --------------

int Win_GBAPalViewerCreate(void); // Returns 1 on error
void Win_GBAPalViewerUpdate(void);
void Win_GBAPalViewerClose(void);

#endif // SDL2_GUI_DEBUGGER_WIN_GBA_DEBUGGER_H__
