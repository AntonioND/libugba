// SPDX-License-Identifier: MIT
//
// Copyright (c) 2020-2021 Antonio Niño Díaz

#ifndef SOUND_H__
#define SOUND_H__

#include "definitions.h"

// Enable or disable sound hardware. It must be enabled before doing anything
// else with sound hardware.
EXPORT_API void SOUND_MasterEnable(int enable);

// Set volume of DMA channels. Valid values are 100 and 50. Any other value will
// be treated as 50%.
EXPORT_API void SOUND_DMA_Volume(int dma_a_max, int dma_b_max);

// Specify which DMA channels are heard from which speakers.
EXPORT_API
void SOUND_DMA_Pan(int dma_a_left, int dma_a_right,
                   int dma_b_left, int dma_b_right);

// Set buffer that will be streamed to DMA channel A. The buffer needs to be
// aligned to 32 bits.
EXPORT_API void SOUND_DMA_Stream_A(const void *source);

// Set buffer that will be streamed to DMA channel B. The buffer needs to be
// aligned to 32 bits.
EXPORT_API void SOUND_DMA_Stream_B(const void *source);

// Specify which timer is assigned to each one of the DMA channels. Only timers
// 0 and 1 are valid.
EXPORT_API void SOUND_DMA_TimerSetup(int dma_a_timer, int dma_b_timer);

#endif // SOUND_H__
