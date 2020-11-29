// SPDX-License-Identifier: MIT
//
// Copyright (c) 2020 Antonio Niño Díaz

#ifndef VRAM_H__
#define VRAM_H__

#include <stddef.h>
#include <stdint.h>

#include "definitions.h"

EXPORT_API
void VRAM_OBJTiles16Copy(const void *src, size_t size, uint32_t tile_index);

EXPORT_API
void VRAM_OBJTiles256Copy(const void *src, size_t size, uint32_t tile_index);

EXPORT_API
void VRAM_OBJPalette16Copy(const void *src, size_t size, uint32_t pal_index);

EXPORT_API
void VRAM_OBJPalette256Copy(const void *src, size_t size);

EXPORT_API
void VRAM_BGTiles16Copy(const void *src, size_t size, uint32_t tile_index);

EXPORT_API
void VRAM_BGTiles256Copy(const void *src, size_t size, uint32_t tile_index);

EXPORT_API
void VRAM_BGPalette16Copy(const void *src, size_t size, uint32_t pal_index);

EXPORT_API
void VRAM_BGPalette256Copy(const void *src, size_t size);

#endif // VRAM_H__
