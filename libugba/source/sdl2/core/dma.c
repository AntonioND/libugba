// SPDX-License-Identifier: LGPL-3.0-only
//
// Copyright (c) 2011-2015, 2019-2020 Antonio Niño Díaz

#include <string.h>

#include <ugba/ugba.h>

#include "../debug_utils.h"

typedef struct {
    int enabled;

    uintptr_t srcaddr, dstaddr;

    int copywords;
    size_t num_chunks;

    int32_t srcadd, dstadd;

    int repeat;

    uint32_t start_mode;
} dma_channel;

static dma_channel DMA[4];

static void GBA_DMACopyNow(dma_channel *dma)
{
    // TODO: DMA 3 is the only one that can read from the ROM!

    if (dma->copywords)
    {
        for (size_t i = 0; i < dma->num_chunks; i++)
        {
            uint32_t *src = (uint32_t *)dma->srcaddr;
            uint32_t *dst = (uint32_t *)dma->dstaddr;

            *dst = *src;

            dma->srcaddr += dma->srcadd;
            dma->dstaddr += dma->dstadd;
        }
    }
    else
    {
        for (size_t i = 0; i < dma->num_chunks; i++)
        {
            uint16_t *src = (uint16_t *)dma->srcaddr;
            uint16_t *dst = (uint16_t *)dma->dstaddr;

            *dst = *src;

            dma->srcaddr += dma->srcadd;
            dma->dstaddr += dma->dstadd;
        }
    }
}

static int UGBA_DMA_SoundGetChannelFifoA(void)
{
    if (DMA[1].dstaddr == (uintptr_t)REG_FIFO_A)
    {
        if (DMA[1].start_mode == DMACNT_START_SPECIAL)
        {
            return 1;
        }
    }

    if (DMA[2].dstaddr == (uintptr_t)REG_FIFO_A)
    {
        if (DMA[2].start_mode == DMACNT_START_SPECIAL)
        {
            return 2;
        }
    }

    return -1;
}

static int UGBA_DMA_SoundGetChannelFifoB(void)
{
    if (DMA[1].dstaddr == (uintptr_t)REG_FIFO_B)
    {
        if (DMA[1].start_mode == DMACNT_START_SPECIAL)
        {
            return 1;
        }
    }

    if (DMA[2].dstaddr == (uintptr_t)REG_FIFO_B)
    {
        if (DMA[2].start_mode == DMACNT_START_SPECIAL)
        {
            return 2;
        }
    }

    return -1;
}

uint32_t UGBA_DMA_SoundGetDataFifoA(void)
{
    int channel = UGBA_DMA_SoundGetChannelFifoA();
    if (channel == -1)
        return 0;

    uint32_t *src = (uint32_t *)DMA[channel].srcaddr;
    DMA[channel].srcaddr += 4;

    return *src;
}

uint32_t UGBA_DMA_SoundGetDataFifoB(void)
{
    int channel = UGBA_DMA_SoundGetChannelFifoB();
    if (channel == -1)
        return 0;

    uint32_t *src = (uint32_t *)DMA[channel].srcaddr;
    DMA[channel].srcaddr += 4;

    return *src;
}

static void GBA_DMAUpdateState(int channel)
{
    uint16_t dmacnt, dmasize;
    uintptr_t dmasad, dmadad;

    if (channel == 0)
    {
        dmasad = REG_DMA0SAD;
        dmadad = REG_DMA0DAD;
        dmasize = REG_DMA0CNT_L;
        dmacnt = REG_DMA0CNT_H;
    }
    else if (channel == 1)
    {
        dmasad = REG_DMA1SAD;
        dmadad = REG_DMA1DAD;
        dmasize = REG_DMA1CNT_L;
        dmacnt = REG_DMA1CNT_H;
    }
    else if (channel == 2)
    {
        dmasad = REG_DMA2SAD;
        dmadad = REG_DMA2DAD;
        dmasize = REG_DMA2CNT_L;
        dmacnt= REG_DMA2CNT_H;
    }
    else if (channel == 3)
    {
        dmasad = REG_DMA3SAD;
        dmadad = REG_DMA3DAD;
        dmasize = REG_DMA3CNT_L;
        dmacnt = REG_DMA3CNT_H;
    }
    else
    {
        return;
    }

    dma_channel *dma = &DMA[channel];

    if (!(dmacnt & DMACNT_DMA_ENABLE))
    {
        dma->enabled = 0;
        dma->start_mode = DMACNT_START_NOW;
        return;
    }

    uint16_t dst_inc_mode = dmacnt & (3 << 5);
    if (dst_inc_mode == DMACNT_DST_INCREMENT)
        dma->dstadd = 2;
    else if (dst_inc_mode == DMACNT_DST_DECREMENT)
        dma->dstadd = -2;
    else
        dma->dstadd = 0;

    uint16_t src_inc_mode = dmacnt & (3 << 7);
    if (src_inc_mode == DMACNT_SRC_INCREMENT)
        dma->srcadd = 2;
    else if (src_inc_mode == DMACNT_SRC_DECREMENT)
        dma->srcadd = -2;
    else
        dma->srcadd = 0;

    dma->repeat = dmacnt & DMACNT_REPEAT_ON;

    if (dmacnt & DMACNT_TRANSFER_32_BITS)
    {
        dma->copywords = 1;
        dma->srcadd *= 2;
        dma->dstadd *= 2;
    }
    else
    {
        dma->copywords = 0;
    }

    dma->srcaddr = dmasad;
    dma->dstaddr = dmadad;

    dma->num_chunks = ((size_t)dmasize) & 0x3FFF;
    if (dma->num_chunks == 0)
    {
        if (channel == 3)
            dma->num_chunks = 0x10000;
        else
            dma->num_chunks = 0x4000;
    }

    dma->start_mode = dmacnt & (3 << 12);

    if (dma->start_mode == DMACNT_START_NOW)
    {
        // Do the copy now
        GBA_DMACopyNow(dma);
        dma->enabled = 0;
    }
}

void GBA_DMAUpdateRegister(uint32_t offset)
{
    switch (offset)
    {
        case OFFSET_DMA0CNT_H:
            GBA_DMAUpdateState(0);
            break;
        case OFFSET_DMA1CNT_H:
            GBA_DMAUpdateState(1);
            break;
        case OFFSET_DMA2CNT_H:
            GBA_DMAUpdateState(2);
            break;
        case OFFSET_DMA3CNT_H:
            GBA_DMAUpdateState(3);
            break;

        default:
            break;
    }
}

void GBA_DMAHandleHBL(void)
{
    for (int i = 0; i < 4; i++)
    {
        dma_channel *dma = &DMA[i];

        if (dma->start_mode == DMACNT_START_HBLANK)
        {
            GBA_DMACopyNow(dma);

            if (dma->repeat)
                continue;

            dma->enabled = 0;
        }
    }
}

void GBA_DMAHandleVBL(void)
{
    for (int i = 0; i < 4; i++)
    {
        dma_channel *dma = &DMA[i];

        if (dma->start_mode == DMACNT_START_VBLANK)
        {
            GBA_DMACopyNow(dma);

            if (dma->repeat)
                continue;

            dma->enabled = 0;
        }
    }
}
