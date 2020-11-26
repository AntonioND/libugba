// SPDX-License-Identifier: LGPL-3.0-only
//
// Copyright (c) 2020 Antonio Niño Díaz

#include <stdint.h>

#include <ugba/ugba.h>

#include "core/video.h"
#include "core/dma.h"

uint64_t internal_bios[MEM_BIOS_SIZE / sizeof(uint64_t)];
uint64_t internal_ewram[MEM_EWRAM_SIZE / sizeof(uint64_t)];
uint64_t internal_iwram[MEM_IWRAM_SIZE / sizeof(uint64_t)];
uint64_t internal_io[MEM_IO_SIZE / sizeof(uint64_t)];
uint64_t internal_palette[MEM_PALETTE_SIZE / sizeof(uint64_t)];
uint64_t internal_vram[MEM_VRAM_SIZE / sizeof(uint64_t)];
uint64_t internal_oam[MEM_OAM_SIZE / sizeof(uint64_t)];
uint64_t internal_rom[MEM_ROM_SIZE / sizeof(uint64_t)];
uint64_t internal_sram[MEM_SRAM_SIZE / sizeof(uint64_t)];

uintptr_t UGBA_MemBIOS(void)
{
    return (uintptr_t)(&internal_bios[0]);
}

uintptr_t UGBA_MemEWRAM(void)
{
    return (uintptr_t)(&internal_ewram[0]);
}

uintptr_t UGBA_MemIWRAM(void)
{
    return (uintptr_t)(&internal_iwram[0]);
}

uintptr_t UGBA_MemIO(void)
{
    return (uintptr_t)(&internal_io[0]);
}

uintptr_t UGBA_MemPalette(void)
{
    return (uintptr_t)(&internal_palette[0]);
}

uintptr_t UGBA_MemVRAM(void)
{
    return (uintptr_t)(&internal_vram[0]);
}

uintptr_t UGBA_MemOAM(void)
{
    return (uintptr_t)(&internal_oam[0]);
}

uintptr_t UGBA_MemROM(void)
{
    return (uintptr_t)(&internal_rom[0]);
}

uintptr_t UGBA_MemSRAM(void)
{
    return (uintptr_t)(&internal_sram[0]);
}

void UGBA_RegisterUpdatedOffset(uint32_t offset)
{
    switch (offset)
    {
        case OFFSET_DMA0CNT_H:
        case OFFSET_DMA1CNT_H:
        case OFFSET_DMA2CNT_H:
        case OFFSET_DMA3CNT_H:
            GBA_DMAUpdateRegister(offset);
            break;

        case OFFSET_BG2X_L:
        case OFFSET_BG2X_H:
        case OFFSET_BG2Y_L:
        case OFFSET_BG2Y_H:
        case OFFSET_BG3X_L:
        case OFFSET_BG3X_H:
        case OFFSET_BG3Y_L:
        case OFFSET_BG3Y_H:
            GBA_VideoUpdateRegister(offset);
            break;

        default:
            break;
    }
}

static uintptr_t DMASAD[4];
static uintptr_t DMADAD[4];

uintptr_t *UGBA_RegDMA0SAD(void)
{
    return &(DMASAD[0]);
}

uintptr_t *UGBA_RegDMA0DAD(void)
{
    return &(DMADAD[0]);
}

uintptr_t *UGBA_RegDMA1SAD(void)
{
    return &(DMASAD[1]);
}

uintptr_t *UGBA_RegDMA1DAD(void)
{
    return &(DMADAD[1]);
}

uintptr_t *UGBA_RegDMA2SAD(void)
{
    return &(DMASAD[2]);
}

uintptr_t *UGBA_RegDMA2DAD(void)
{
    return &(DMADAD[2]);
}

uintptr_t *UGBA_RegDMA3SAD(void)
{
    return &(DMASAD[3]);
}

uintptr_t *UGBA_RegDMA3DAD(void)
{
    return &(DMADAD[3]);
}
