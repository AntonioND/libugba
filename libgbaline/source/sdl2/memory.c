// SPDX-License-Identifier: LGPL-3.0-only
//
// Copyright (c) 2020 Antonio Niño Díaz

#include <stdint.h>

#include <gbaline.h>

#include "core/video.h"

uint64_t internal_bios[MEM_BIOS_SIZE / sizeof(uint64_t)];
uint64_t internal_ewram[MEM_EWRAM_SIZE / sizeof(uint64_t)];
uint64_t internal_iwram[MEM_IWRAM_SIZE / sizeof(uint64_t)];
uint64_t internal_io[MEM_IO_SIZE / sizeof(uint64_t)];
uint64_t internal_palette[MEM_PALETTE_SIZE / sizeof(uint64_t)];
uint64_t internal_vram[MEM_VRAM_SIZE / sizeof(uint64_t)];
uint64_t internal_oam[MEM_OAM_SIZE / sizeof(uint64_t)];
uint64_t internal_rom[MEM_ROM_SIZE / sizeof(uint64_t)];
uint64_t internal_sram[MEM_SRAM_SIZE / sizeof(uint64_t)];

uintptr_t GBALINE_MemBIOS(void)
{
    return (uintptr_t)(&internal_bios[0]);
}

uintptr_t GBALINE_MemEWRAM(void)
{
    return (uintptr_t)(&internal_ewram[0]);
}

uintptr_t GBALINE_MemIWRAM(void)
{
    return (uintptr_t)(&internal_iwram[0]);
}

uintptr_t GBALINE_MemIO(void)
{
    return (uintptr_t)(&internal_io[0]);
}

uintptr_t GBALINE_MemPalette(void)
{
    return (uintptr_t)(&internal_palette[0]);
}

uintptr_t GBALINE_MemVRAM(void)
{
    return (uintptr_t)(&internal_vram[0]);
}

uintptr_t GBALINE_MemOAM(void)
{
    return (uintptr_t)(&internal_oam[0]);
}

uintptr_t GBALINE_MemROM(void)
{
    return (uintptr_t)(&internal_rom[0]);
}

uintptr_t GBALINE_MemSRAM(void)
{
    return (uintptr_t)(&internal_sram[0]);
}

void GBA_RegisterUpdatedOffset(uint32_t offset)
{
    switch (offset)
    {
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
