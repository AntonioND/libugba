// SPDX-License-Identifier: MIT
//
// Copyright (c) 2020-2021 Antonio Niño Díaz

#include <ugba/ugba.h>

// This directive helps with unit testing. Every build system deals differently
// with relative vs absolute paths. This makes asserts always print the same
// error message.
#line 11 "source/dma.c"

int DMA_Transfer(int channel, const void *src, void *dst, size_t size,
                 uint16_t flags)
{
    uintptr_t source = (uintptr_t)src;
    uintptr_t dest = (uintptr_t)dst;
    uint16_t nelem = flags & DMACNT_TRANSFER_32_BITS ? (size / 4) : (size / 2);
    uint16_t newflags = flags | DMACNT_DMA_ENABLE;

#ifdef __GBA__
    // On GBA, DMA channels 0, 1 and 2 don't work when the source address is in
    // the external cartridge. Unfortunately, this error check can't work on the
    // SDL2 port, as all code, data and heap don't need to follow any placement
    // rules like in GBA.
    if (channel < 3)
    {
        UMOD_Assert(source < MEM_ROM_ADDR_WS0);

        // If asserts are disabled, return an error code at least.
        if (source >= MEM_ROM_ADDR_WS0)
            return -1;
    }
#endif // __GBA__

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
    else
    {
        return -1;
    }

    return 0;
}

int DMA_Stop(int channel)
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
    else
    {
        return -1;
    }

    return 0;
}

int DMA_Copy16(int channel, const void *src, void *dst, size_t size)
{
    return DMA_Transfer(channel, src, dst, size,
                        DMACNT_DST_INCREMENT | DMACNT_SRC_INCREMENT |
                        DMACNT_TRANSFER_16_BITS | DMACNT_START_NOW);
}

int DMA_Copy32(int channel, const void *src, void *dst, size_t size)
{
    return DMA_Transfer(channel, src, dst, size,
                        DMACNT_DST_INCREMENT | DMACNT_SRC_INCREMENT |
                        DMACNT_TRANSFER_32_BITS | DMACNT_START_NOW);
}

int DMA_HBLCopy16(int channel, const void *src, void *dst, size_t size)
{
    return DMA_Transfer(channel, src, dst, size,
                        DMACNT_DST_INCREMENT | DMACNT_SRC_INCREMENT |
                        DMACNT_TRANSFER_16_BITS | DMACNT_START_HBLANK);
}

int DMA_HBLCopy32(int channel, const void *src, void *dst, size_t size)
{
    return DMA_Transfer(channel, src, dst, size,
                        DMACNT_DST_INCREMENT | DMACNT_SRC_INCREMENT |
                        DMACNT_TRANSFER_32_BITS | DMACNT_START_HBLANK);
}

int DMA_VBLCopy16(int channel, const void *src, void *dst, size_t size)
{
    return DMA_Transfer(channel, src, dst, size,
                        DMACNT_DST_INCREMENT | DMACNT_SRC_INCREMENT |
                        DMACNT_TRANSFER_16_BITS | DMACNT_START_VBLANK);
}

int DMA_VBLCopy32(int channel, const void *src, void *dst, size_t size)
{
    return DMA_Transfer(channel, src, dst, size,
                        DMACNT_DST_INCREMENT | DMACNT_SRC_INCREMENT |
                        DMACNT_TRANSFER_32_BITS | DMACNT_START_VBLANK);
}
