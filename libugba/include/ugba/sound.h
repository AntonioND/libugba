// SPDX-License-Identifier: MIT
//
// Copyright (c) 2020 Antonio Niño Díaz

#ifndef SOUND_H__
#define SOUND_H__

#include "definitions.h"

EXPORT_API
void SOUND_MasterEnable(int enable);

EXPORT_API
void SOUND_DMA_Volume(int dma_a_max, int dma_b_max);

EXPORT_API
void SOUND_DMA_Pan(int dma_a_left, int dma_a_right,
                   int dma_b_left, int dma_b_right);

EXPORT_API
void SOUND_DMA_Stream_A(const void *source);

EXPORT_API
void SOUND_DMA_Stream_B(const void *source);

EXPORT_API
void SOUND_DMA_TimerSetup(int dma_a_timer, int dma_b_timer);

#endif // SOUND_H__
