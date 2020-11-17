// SPDX-License-Identifier: MIT
//
// Copyright (c) 2020 Antonio Niño Díaz

#ifndef OBJ_H__
#define OBJ_H__

typedef enum {
    OBJ_MODE_NORMAL,
    OBJ_MODE_TRANSPARENT,
    OBJ_MODE_WINDOW
} oam_entry_mode;

typedef enum {
    // Square
    OBJ_SIZE_8x8,
    OBJ_SIZE_16x16,
    OBJ_SIZE_32x32,
    OBJ_SIZE_64x64,

    // Horizontal
    OBJ_SIZE_16x8,
    OBJ_SIZE_32x8,
    OBJ_SIZE_32x16,
    OBJ_SIZE_64x32,

    // Vertical
    OBJ_SIZE_8x16,
    OBJ_SIZE_8x32,
    OBJ_SIZE_16x32,
    OBJ_SIZE_32x64,

    OBJ_SIZE_NUMBER,

    OBJ_SIZE_INVALID

} oam_entry_size;

// Regular objects functions
// -------------------------

void OBJ_RegularInit(int index, int x, int y, oam_entry_size size,
                     int colors256, int pal, int tile);
void OBJ_RegularEnableSet(int index, int enable);
void OBJ_RegularHFlipSet(int index, int enable);
void OBJ_RegularVFlipSet(int index, int enable);

// Affine objects functions
// ------------------------

void OBJ_AffineInit(int index, int x, int y, oam_entry_size size, int matrix,
                    int colors256, int pal, int tile, int doublesize);
void OBJ_AffineMatrixSet(int index, int matrix_index);
void OBJ_AffineDoubleSizeSet(int index, int enable);

// Common objects functions
// ------------------------

void OBJ_PositionSet(int index, int x, int y);
void OBJ_PositionGet(int index, int *x, int *y);

void OBJ_ModeSet(int index, oam_entry_mode mode);

void OBJ_GetShapeSize(oam_entry_size size,
                      uint16_t *attr0_shape, uint16_t *attr1_size);
oam_entry_size OBJ_GetSizeFromDimensions(int width, int height);
void OBJ_SizeSet(int index, oam_entry_size size);

void OBJ_MosaicSet(int index, int enable);

// Only for 16 color sprites
void OBJ_Palette16Set(int index, int palette);

void OBJ_PrioritySet(int index, int priority);

// This function detects if the sprite is in 16 or 256 color mode
void OBJ_TileSet(int index, int tile);

#endif // OBJ_H__
