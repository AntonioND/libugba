// SPDX-License-Identifier: LGPL-3.0-only
//
// Copyright (c) 2011-2015, 2019-2020 Antonio Niño Díaz

#ifndef SDL2_CORE_VIDEO__
#define SDL2_CORE_VIDEO__

void GBA_SkipFrame(int skip);
int GBA_HasToSkipFrame(void);

// Must be called to fill the look up tables used for blending effects.
void GBA_FillFadeTables(void);

// Note: The correct way of emulating is drawing a pixel every 4 clocks. This is
// an optimization that makes pretty much all games show as expected.
void GBA_UpdateDrawScanlineFn(void);

void GBA_VideoUpdateRegister(unsigned int address);

void GBA_DrawScanline(int y);
void GBA_DrawScanlineWhite(int y);

// 24-bit RGB
void GBA_ConvertScreenBufferTo24RGB(void *dst);
// 32-bit RGB (with alpha set to 255 in all pixels)
void GBA_ConvertScreenBufferTo32RGB(void *dst);

#endif // SDL2_CORE_VIDEO__
