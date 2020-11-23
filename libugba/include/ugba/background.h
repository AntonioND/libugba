// SPDX-License-Identifier: MIT
//
// Copyright (c) 2020 Antonio Niño Díaz

#ifndef BACKGROUND_H__
#define BACKGROUND_H__

#include "hardware.h"

typedef enum {
    BG_REGULAR_256x256,
    BG_REGULAR_512x256,
    BG_REGULAR_256x512,
    BG_REGULAR_512x512,

    BG_REGULAR_NUMBER,

    BG_REGULAR_INVALID
} bg_regular_size;

typedef enum {
    BG_AFFINE_128x128,
    BG_AFFINE_256x256,
    BG_AFFINE_512x512,
    BG_AFFINE_1024x1024,

    BG_AFFINE_NUMBER,

    BG_AFFINE_INVALID
} bg_affine_size;

typedef enum {
    BG_16_COLORS,
    BG_256_COLORS
} bg_color_mode;

EXPORT_API
void BG_RegularInit(int index, bg_regular_size size, bg_color_mode colors,
                    uintptr_t tile_base_addr, uintptr_t map_base_addr);

// The scroll hardware registers are read-only. The functions below keep a copy
// of the scroll value so that it can be recovered later.
EXPORT_API void BG_RegularScrollSet(int index, int x, int y);
EXPORT_API void BG_RegularScrollGet(int index, int *x, int *y);

EXPORT_API
void BG_AffineInit(int index, bg_affine_size size, bg_color_mode colors,
                   uintptr_t tile_base_addr, uintptr_t map_base_addr,
                   int wrap);

EXPORT_API
void BG_PrioritySet(int index, int priority);

EXPORT_API
void BG_MosaicEnable(int index, int enabled);

EXPORT_API uint16_t *BG_Mode3FramebufferGet(void);

EXPORT_API uint8_t *BG_Mode4FramebufferActiveGet(void);
EXPORT_API uint8_t *BG_Mode4FramebufferBackGet(void);
EXPORT_API uint16_t *BG_Mode5FramebufferActiveGet(void);
EXPORT_API uint16_t *BG_Mode5FramebufferBackGet(void);

EXPORT_API void BG_FramebufferSet(int backbuffer);
EXPORT_API void BG_FramebufferSwap(void);

EXPORT_API void BG_BackdropColorSet(uint16_t color);

#endif // BACKGROUND_H__
