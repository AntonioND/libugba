// SPDX-License-Identifier: MIT
//
// Copyright (c) 2020 Antonio Niño Díaz

#ifndef DMA_H__
#define DMA_H__

#include "definitions.h"

// Important note: The only DMA channel that can read from ROM is 3.

// Generic DMA transfer function. It is needed to start repeating transfers or
// anything other than a simple copy, as there are only helpers for simple
// copy scenarios.
EXPORT_API
void DMA_Transfer(int channel, const void *src, void *dst, size_t size,
                  uint16_t flags);

// Stop DMA transfers that repeat.
EXPORT_API
void DMA_Stop(int channel);

// Copy from source to destination in 16-bit chunks.
EXPORT_API
void DMA_Copy16(int channel, const void *src, void *dst, size_t size);

// Copy from source to destination in 32-bit chunks.
EXPORT_API
void DMA_Copy32(int channel, const void *src, void *dst, size_t size);

// During the next HBL do a copy, in 16-bit chunks.
EXPORT_API
void DMA_HBLCopy16(int channel, const void *src, void *dst, size_t size);

// During the next HBL do a copy, in 32-bit chunks.
EXPORT_API
void DMA_HBLCopy32(int channel, const void *src, void *dst, size_t size);

// During the next VBL do a copy, in 16-bit chunks.
EXPORT_API
void DMA_VBLCopy16(int channel, const void *src, void *dst, size_t size);

// During the next VBL do a copy, in 32-bit chunks.
EXPORT_API
void DMA_VBLCopy32(int channel, const void *src, void *dst, size_t size);

EXPORT_API
void DMA_StreamAudio(int channel, const void *src, void *dst);

#endif // DMA_H__
