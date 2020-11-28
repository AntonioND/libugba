// SPDX-License-Identifier: MIT
//
// Copyright (c) 2020 Antonio Niño Díaz

#include <ugba/ugba.h>

void DMA_Transfer(int channel, const void *src, void *dst, size_t size,
                  uint16_t flags)
{
    uintptr_t source = (uintptr_t)src;
    uintptr_t dest = (uintptr_t)dst;
    uint16_t nelem = flags & DMACNT_TRANSFER_32_BITS ? (size / 4) : (size / 2);
    uint16_t newflags = flags | DMACNT_DMA_ENABLE;

    if (channel == 0)
    {
        REG_DMA0SAD = source;
        REG_DMA0DAD = dest;
        REG_DMA0CNT_L = nelem;
        REG_DMA0CNT_H = newflags;
        UGBA_RegisterUpdatedOffset(OFFSET_DMA0CNT_H);
    }
    else if (channel == 1)
    {
        REG_DMA1SAD = source;
        REG_DMA1DAD = dest;
        REG_DMA1CNT_L = nelem;
        REG_DMA1CNT_H = newflags;
        UGBA_RegisterUpdatedOffset(OFFSET_DMA1CNT_H);
    }
    else if (channel == 2)
    {
        REG_DMA2SAD = source;
        REG_DMA2DAD = dest;
        REG_DMA2CNT_L = nelem;
        REG_DMA2CNT_H = newflags;
        UGBA_RegisterUpdatedOffset(OFFSET_DMA2CNT_H);
    }
    else if (channel == 3)
    {
        REG_DMA3SAD = source;
        REG_DMA3DAD = dest;
        REG_DMA3CNT_L = nelem;
        REG_DMA3CNT_H = newflags;
        UGBA_RegisterUpdatedOffset(OFFSET_DMA3CNT_H);
    }
}

void DMA_Stop(int channel)
{
    if (channel == 0)
    {
        REG_DMA0CNT_H = 0;
        UGBA_RegisterUpdatedOffset(OFFSET_DMA0CNT_H);
    }
    else if (channel == 1)
    {
        REG_DMA1CNT_H = 0;
        UGBA_RegisterUpdatedOffset(OFFSET_DMA1CNT_H);
    }
    else if (channel == 2)
    {
        REG_DMA2CNT_H = 0;
        UGBA_RegisterUpdatedOffset(OFFSET_DMA2CNT_H);
    }
    else if (channel == 3)
    {
        REG_DMA3CNT_H = 0;
        UGBA_RegisterUpdatedOffset(OFFSET_DMA3CNT_H);
    }
}

void DMA_Copy16(int channel, const void *src, void *dst, size_t size)
{
    DMA_Transfer(channel, src, dst, size,
                 DMACNT_DST_INCREMENT | DMACNT_SRC_INCREMENT |
                 DMACNT_TRANSFER_16_BITS | DMACNT_START_NOW);
}

void DMA_Copy32(int channel, const void *src, void *dst, size_t size)
{
    DMA_Transfer(channel, src, dst, size,
                 DMACNT_DST_INCREMENT | DMACNT_SRC_INCREMENT |
                 DMACNT_TRANSFER_32_BITS | DMACNT_START_NOW);
}

void DMA_HBLCopy16(int channel, const void *src, void *dst, size_t size)
{
    DMA_Transfer(channel, src, dst, size,
                 DMACNT_DST_INCREMENT | DMACNT_SRC_INCREMENT |
                 DMACNT_TRANSFER_16_BITS | DMACNT_START_HBLANK);
}

void DMA_HBLCopy32(int channel, const void *src, void *dst, size_t size)
{
    DMA_Transfer(channel, src, dst, size,
                 DMACNT_DST_INCREMENT | DMACNT_SRC_INCREMENT |
                 DMACNT_TRANSFER_32_BITS | DMACNT_START_HBLANK);
}

void DMA_VBLCopy16(int channel, const void *src, void *dst, size_t size)
{
    DMA_Transfer(channel, src, dst, size,
                 DMACNT_DST_INCREMENT | DMACNT_SRC_INCREMENT |
                 DMACNT_TRANSFER_16_BITS | DMACNT_START_VBLANK);
}

void DMA_VBLCopy32(int channel, const void *src, void *dst, size_t size)
{
    DMA_Transfer(channel, src, dst, size,
                 DMACNT_DST_INCREMENT | DMACNT_SRC_INCREMENT |
                 DMACNT_TRANSFER_32_BITS | DMACNT_START_VBLANK);
}
