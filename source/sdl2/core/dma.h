// SPDX-License-Identifier: LGPL-3.0-only
//
// Copyright (c) 2020 Antonio Niño Díaz

#ifndef SDL2_CORE_DMA_H__
#define SDL2_CORE_DMA_H__

void GBA_DMAUpdateRegister(uint32_t offset);
void GBA_DMAHandleHBL(void);
void GBA_DMAHandleVBL(void);

uint32_t UGBA_DMA_SoundGetDataFifoA(void);
uint32_t UGBA_DMA_SoundGetDataFifoB(void);

#endif // SDL2_CORE_DMA_H__
