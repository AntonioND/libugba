// SPDX-License-Identifier: MIT
//
// Copyright (c) 2020-2021 Antonio Niño Díaz

#include <ugba/ugba.h>

// This directive helps with unit testing. Every build system deals differently
// with relative vs absolute paths. This makes asserts always print the same
// error message.
#line 11 "source/obj.c"

static void OBJ_TileSet_internal(oam_entry *e, oam_color_mode colors,
                                 int tile)
{
    e->attr2 &= ~ATTR2_TILE_MASK;

    if (colors == OBJ_256_COLORS)
        e->attr2 |= ATTR2_256_COLOR_TILE(tile);
    else
        e->attr2 |= ATTR2_16_COLOR_TILE(tile);
}

static int OBJ_IsRegular(oam_entry *e)
{
    if ((e->attr0 & ATTR0_TYPE_MASK) == ATTR0_REGULAR)
        return 1;

    return 0;
}

static int OBJ_IsAffine(oam_entry *e)
{
    if ((e->attr0 & ATTR0_TYPE_MASK) == ATTR0_AFFINE)
        return 1;

    return 0;
}

// Regular objects functions
// -------------------------

void OBJ_RegularInit(int index, int x, int y, oam_entry_size size,
                     oam_color_mode colors, int pal, int tile)
{
    if (index >= MEM_OAM_NUMBER_ENTRIES)
        return;

    uint16_t attr0_shape, attr1_size;
    OBJ_GetShapeSize(size, &attr0_shape, &attr1_size);

    oam_entry *e = &MEM_OAM_ENTRIES[index];

    e->attr0 = ATTR0_Y(y) | ATTR0_REGULAR | ATTR0_MODE_NORMAL |
               (colors == OBJ_256_COLORS ? ATTR0_256_COLORS : ATTR0_16_COLORS) |
               attr0_shape;

    e->attr1 = ATTR1_X(x) | attr1_size;

    e->attr2 = ATTR2_PRIORITY(0) | ATTR2_PALETTE(pal);

    OBJ_TileSet_internal(e, colors, tile);
}

void OBJ_RegularEnableSet(int index, int enable)
{
    if (index >= MEM_OAM_NUMBER_ENTRIES)
        return;

    oam_entry *e = &MEM_OAM_ENTRIES[index];

    UGBA_Assert(OBJ_IsRegular(e));

    if (enable)
        e->attr0 &= ~ATTR0_DISABLE;
    else
        e->attr0 |= ATTR0_DISABLE;
}

void OBJ_RegularHFlipSet(int index, int enable)
{
    if (index >= MEM_OAM_NUMBER_ENTRIES)
        return;

    oam_entry *e = &MEM_OAM_ENTRIES[index];

    UGBA_Assert(OBJ_IsRegular(e));

    if (enable)
        e->attr1 |= ATTR1_REGULAR_HFLIP;
    else
        e->attr1 &= ~ATTR1_REGULAR_HFLIP;
}

void OBJ_RegularVFlipSet(int index, int enable)
{
    if (index >= MEM_OAM_NUMBER_ENTRIES)
        return;

    oam_entry *e = &MEM_OAM_ENTRIES[index];

    UGBA_Assert(OBJ_IsRegular(e));

    if (enable)
        e->attr1 |= ATTR1_REGULAR_VFLIP;
    else
        e->attr1 &= ~ATTR1_REGULAR_VFLIP;
}

// Affine objects functions
// ------------------------

void OBJ_AffineInit(int index, int x, int y, oam_entry_size size, int matrix,
                    oam_color_mode colors, int pal, int tile, int doublesize)
{
    if (index >= MEM_OAM_NUMBER_ENTRIES)
        return;

    uint16_t attr0_shape, attr1_size;
    OBJ_GetShapeSize(size, &attr0_shape, &attr1_size);

    oam_entry *e = &MEM_OAM_ENTRIES[index];

    e->attr0 = ATTR0_Y(y) | ATTR0_AFFINE | ATTR0_MODE_NORMAL |
               (colors == OBJ_256_COLORS ? ATTR0_256_COLORS : ATTR0_16_COLORS) |
               (doublesize ? ATTR0_DOUBLE_SIZE : 0) |
               attr0_shape;

    e->attr1 = ATTR1_X(x) | ATTR1_AFFINE_MATRIX(matrix) | attr1_size;

    e->attr2 = ATTR2_PRIORITY(0) | ATTR2_PALETTE(pal);

    OBJ_TileSet_internal(e, colors, tile);
}

void OBJ_AffineMatrixSet(int index, int matrix_index)
{
    if (index >= MEM_OAM_NUMBER_ENTRIES)
        return;

    oam_entry *e = &MEM_OAM_ENTRIES[index];

    UGBA_Assert(OBJ_IsAffine(e));

    e->attr1 &= ~ATTR1_AFFINE_MATRIX_MASK;
    e->attr1 |= ~ATTR1_AFFINE_MATRIX(matrix_index);
}

void OBJ_AffineDoubleSizeSet(int index, int enable)
{
    if (index >= MEM_OAM_NUMBER_ENTRIES)
        return;

    oam_entry *e = &MEM_OAM_ENTRIES[index];

    UGBA_Assert(OBJ_IsAffine(e));

    if (enable)
        e->attr0 &= ~ATTR0_DOUBLE_SIZE;
    else
        e->attr0 |= ATTR0_DOUBLE_SIZE;
}

// Common objects functions
// ------------------------

void OBJ_PositionSet(int index, int x, int y)
{
    if (index >= MEM_OAM_NUMBER_ENTRIES)
        return;

    oam_entry *e = &MEM_OAM_ENTRIES[index];

    e->attr0 &= ~ATTR0_Y_MASK;
    e->attr0 |= ATTR0_Y(y);

    e->attr1 &= ~ATTR1_X_MASK;
    e->attr1 |= ATTR1_X(x);
}

void OBJ_PositionGet(int index, int *x, int *y)
{
    if (index >= MEM_OAM_NUMBER_ENTRIES)
    {
        *x = 0;
        *y = 0;
        return;
    }

    oam_entry *e = &MEM_OAM_ENTRIES[index];

    *y = e->attr0 & ATTR0_Y_MASK;
    *x = e->attr1 & ATTR1_X_MASK;
}

void OBJ_ModeSet(int index, oam_entry_mode mode)
{
     if (index >= MEM_OAM_NUMBER_ENTRIES)
        return;

    oam_entry *e = &MEM_OAM_ENTRIES[index];

    e->attr0 &= ~ATTR0_MODE_MASK;

    if (mode == OBJ_MODE_NORMAL)
        e->attr0 |= ATTR0_MODE_NORMAL;
    else if (mode == OBJ_MODE_TRANSPARENT)
        e->attr0 |= ATTR0_MODE_TRANSPARENT;
    else if (mode == OBJ_MODE_WINDOW)
        e->attr0 |= ATTR0_MODE_WINDOW;
}

void OBJ_GetShapeSize(oam_entry_size size,
                      uint16_t *attr0_shape, uint16_t *attr1_size)
{
    if (size >= OBJ_SIZE_NUMBER)
    {
        *attr0_shape = 0;
        *attr1_size = 0;
        return;
    }

    const uint16_t table[OBJ_SIZE_NUMBER][2] = {
        [OBJ_SIZE_8x8] = {ATTR0_SHAPE_SQUARE, ATTR1_SIZE_0},
        [OBJ_SIZE_16x16] = {ATTR0_SHAPE_SQUARE, ATTR1_SIZE_1},
        [OBJ_SIZE_32x32] = {ATTR0_SHAPE_SQUARE, ATTR1_SIZE_2},
        [OBJ_SIZE_64x64] = {ATTR0_SHAPE_SQUARE, ATTR1_SIZE_3},

        [OBJ_SIZE_16x8] = {ATTR0_SHAPE_HORIZONTAL, ATTR1_SIZE_0},
        [OBJ_SIZE_32x8] = {ATTR0_SHAPE_HORIZONTAL, ATTR1_SIZE_1},
        [OBJ_SIZE_32x16] = {ATTR0_SHAPE_HORIZONTAL, ATTR1_SIZE_2},
        [OBJ_SIZE_64x32] = {ATTR0_SHAPE_HORIZONTAL, ATTR1_SIZE_3},

        [OBJ_SIZE_8x16] = {ATTR0_SHAPE_VERTICAL, ATTR1_SIZE_0},
        [OBJ_SIZE_8x32] = {ATTR0_SHAPE_VERTICAL, ATTR1_SIZE_1},
        [OBJ_SIZE_16x32] = {ATTR0_SHAPE_VERTICAL, ATTR1_SIZE_2},
        [OBJ_SIZE_32x64] = {ATTR0_SHAPE_VERTICAL, ATTR1_SIZE_3},
    };

    *attr0_shape = table[size][0];
    *attr1_size = table[size][1];
}

static const int obj_size_table[OBJ_SIZE_NUMBER][2] = {
    [OBJ_SIZE_8x8] = {8, 8},
    [OBJ_SIZE_16x16] = {16, 16},
    [OBJ_SIZE_32x32] = {32, 32},
    [OBJ_SIZE_64x64] = {64, 64},

    [OBJ_SIZE_16x8] = {16, 8},
    [OBJ_SIZE_32x8] = {32, 8},
    [OBJ_SIZE_32x16] = {32, 16},
    [OBJ_SIZE_64x32] = {64, 32},

    [OBJ_SIZE_8x16] = {8, 16},
    [OBJ_SIZE_8x32] = {8, 32},
    [OBJ_SIZE_16x32] = {16, 32},
    [OBJ_SIZE_32x64] = {32, 64},
};

oam_entry_size OBJ_GetSizeFromDimensions(int width, int height)
{
    for (int i = 0; i < OBJ_SIZE_NUMBER; i++)
    {
        if ((obj_size_table[i][0] == width) && (obj_size_table[i][1] == height))
            return i;
    }

    return OBJ_SIZE_INVALID;
}

EXPORT_API void OBJ_GetDimensionsFromSize(oam_entry_size size,
                                          int *width, int *height)
{
    *width = obj_size_table[size][0];
    *height = obj_size_table[size][1];
}

void OBJ_MosaicSet(int index, int enable)
{
    if (index >= MEM_OAM_NUMBER_ENTRIES)
        return;

    oam_entry *e = &MEM_OAM_ENTRIES[index];

    if (enable)
        e->attr0 |= ATTR0_MOSAIC;
    else
        e->attr0 &= ~ATTR0_MOSAIC;
}

void OBJ_Palette16Set(int index, int palette)
{
    if (index >= MEM_OAM_NUMBER_ENTRIES)
        return;

    oam_entry *e = &MEM_OAM_ENTRIES[index];

    e->attr2 &= ~ATTR2_PALETTE_MASK;
    e->attr2 |= ATTR2_PALETTE(palette);
}

void OBJ_PrioritySet(int index, int priority)
{
    if (index >= MEM_OAM_NUMBER_ENTRIES)
        return;

    oam_entry *e = &MEM_OAM_ENTRIES[index];

    e->attr2 &= ~ATTR2_PRIORITY_MASK;
    e->attr2 |= ATTR2_PRIORITY(priority);
}

void OBJ_TileSet(int index, int tile)
{
    if (index >= MEM_OAM_NUMBER_ENTRIES)
        return;

    // This function detects if the object is in 16 or 256 colors mode

    oam_entry *e = &MEM_OAM_ENTRIES[index];

    oam_color_mode colors =
        (e->attr0 & ATTR0_256_COLORS) ? OBJ_256_COLORS : OBJ_16_COLORS;

    OBJ_TileSet_internal(e, colors, tile);
}
