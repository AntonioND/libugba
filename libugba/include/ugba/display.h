// SPDX-License-Identifier: MIT
//
// Copyright (c) 2020 Antonio Niño Díaz

#ifndef DISPLAY_H__
#define DISPLAY_H__

#include "hardware.h"

// Set background display mode and set the other fields of the register to 0.
EXPORT_API void DISPLAY_ModeSet(int mode);

// Enable or disable screen layers.
EXPORT_API void DISPLAY_LayersEnable(int bg0, int bg1, int bg2, int bg3, int obj);

// Enable or disable windows.
EXPORT_API void DISPLAY_WindowsEnable(int win0, int win1, int obj);

// Enable or disable the HBL Interval Free mode.
EXPORT_API void DISPLAY_HBLIntervalFreeEnable(int enable);

// Enable or disable forced blank.
EXPORT_API void DISPLAY_ForcedBlankEnable(int enable);

// Enable 1D mapping or disable it (and use 2D mapping).
EXPORT_API void DISPLAY_Object1DMappingEnable(int enable);

#endif // DISPLAY_H__
