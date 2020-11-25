// SPDX-License-Identifier: MIT
//
// Copyright (c) 2020 Antonio Niño Díaz

#ifndef DMA_H__
#define DMA_H__

#include "definitions.h"

// Important note: They only DMA channel that can read from ROM is 3.

EXPORT_API
void DMA_Transfer(int channel, const void *src, void *dst, size_t size,
                  uint16_t flags);

EXPORT_API
void DMA_Stop(int channel);

EXPORT_API
void DMA_Copy16(int channel, const void *src, void *dst, size_t size);

EXPORT_API
void DMA_Copy32(int channel, const void *src, void *dst, size_t size);

EXPORT_API
void DMA_HBLCopy16(int channel, const void *src, void *dst, size_t size);

EXPORT_API
void DMA_HBLCopy32(int channel, const void *src, void *dst, size_t size);

EXPORT_API
void DMA_RepeatedHBLCopy16(int channel, const void *src, void *dst, size_t size);

EXPORT_API
void DMA_RepeatedHBLCopy32(int channel, const void *src, void *dst, size_t size);

EXPORT_API
void DMA_VBLCopy16(int channel, const void *src, void *dst, size_t size);

EXPORT_API
void DMA_VBLCopy32(int channel, const void *src, void *dst, size_t size);

#endif // DMA_H__
