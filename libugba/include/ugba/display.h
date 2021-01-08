// SPDX-License-Identifier: MIT
//
// Copyright (c) 2020-2021 Antonio Niño Díaz

#ifndef DISPLAY_H__
#define DISPLAY_H__

#include "hardware.h"

// Set background display mode and set the other fields of the register to 0.
EXPORT_API void DISP_ModeSet(int mode);

// Enable or disable screen layers.
EXPORT_API void DISP_LayersEnable(int bg0, int bg1, int bg2, int bg3, int obj);

// Enable or disable windows.
EXPORT_API void DISP_WindowsEnable(int win0, int win1, int obj);

// Enable or disable the HBL Interval Free mode.
EXPORT_API void DISP_HBLIntervalFreeEnable(int enable);

// Enable or disable forced blank.
EXPORT_API void DISP_ForcedBlankEnable(int enable);

// Enable 1D mapping or disable it (and use 2D mapping).
EXPORT_API void DISP_Object1DMappingEnable(int enable);

// Setup window 0.
EXPORT_API void WIN_SetupWin0(uint32_t left, uint32_t right,
                              uint32_t top, uint32_t bottom);

// Setup window 1.
EXPORT_API void WIN_SetupWin1(uint32_t left, uint32_t right,
                              uint32_t top, uint32_t bottom);

// Setup mosaic for background and objects.
EXPORT_API void DISP_MosaicSet(uint32_t bgh, uint32_t bgw,
                               uint32_t objh, uint32_t objw);

#endif // DISPLAY_H__
