// SPDX-License-Identifier: MIT
//
// Copyright (c) 2020 Antonio Niño Díaz

#include <gbaline.h>

void VRAM_SpriteTiles16Copy(const void *src, size_t size, uint32_t tile_index)
{
    // 4 bits (16 colors), 8x8, transform from bits to bytes
    const size_t tile_size = (4 * 8 * 8) / 8;

    uint8_t *dst = (uint8_t *)MEM_VRAM_OBJ + (tile_index * tile_size);
    SWI_CpuFastSet_Copy32(src, dst, size);
}

void VRAM_SpriteTiles256Copy(const void *src, size_t size, uint32_t tile_index)
{
    // 8 bits (16 colors), 8x8, transform from bits to bytes
    const size_t tile_size = (8 * 8 * 8) / 8;

    uint8_t *dst = (uint8_t *)MEM_VRAM_OBJ + (tile_index * tile_size);
    SWI_CpuFastSet_Copy32(src, dst, size);
}

void VRAM_SpritePalette16Copy(const void *src, size_t size, uint32_t pal_index)
{
    uint16_t *dst = MEM_PALETTE_OBJ + (pal_index * 16);
    SWI_CpuSet_Copy16(src, dst, size);
}

void VRAM_SpritePalette256Copy(const void *src, size_t size)
{
    SWI_CpuSet_Copy16(src, MEM_PALETTE_OBJ, size);
}
