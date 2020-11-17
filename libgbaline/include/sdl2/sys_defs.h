// SPDX-License-Identifier: MIT
//
// Copyright (c) 2020 Antonio Niño Díaz

#ifndef SDL2_SYS_DEFS_H__
#define SDL2_SYS_DEFS_H__

#include "../definitions.h"

EXPORT_API uintptr_t GBALINE_MemBIOS(void);
EXPORT_API uintptr_t GBALINE_MemEWRAM(void);
EXPORT_API uintptr_t GBALINE_MemIWRAM(void);
EXPORT_API uintptr_t GBALINE_MemIO(void);
EXPORT_API uintptr_t GBALINE_MemPalette(void);
EXPORT_API uintptr_t GBALINE_MemVRAM(void);
EXPORT_API uintptr_t GBALINE_MemOAM(void);
EXPORT_API uintptr_t GBALINE_MemROM(void);
EXPORT_API uintptr_t GBALINE_MemSRAM(void);

#define MEM_BIOS_ADDR       (GBALINE_MemBIOS())
#define MEM_EWRAM_ADDR      (GBALINE_MemEWRAM())
#define MEM_IWRAM_ADDR      (GBALINE_MemIWRAM())
#define MEM_IO_ADDR         (GBALINE_MemIO())
#define MEM_PALETTE_ADDR    (GBALINE_MemPalette())
#define MEM_VRAM_ADDR       (GBALINE_MemVRAM())
#define MEM_OAM_ADDR        (GBALINE_MemOAM())
#define MEM_ROM_ADDR_WS0    (GBALINE_MemROM())
#define MEM_ROM_ADDR_WS1    MEM_ROM_ADDR_WS0
#define MEM_ROM_ADDR_WS2    MEM_ROM_ADDR_WS0
#define MEM_SRAM_ADDR       (GBALINE_MemSRAM())

#endif // SDL2_SYS_DEFS_H__
