// SPDX-License-Identifier: MIT
//
// Copyright (c) 2020 Antonio Niño Díaz

#ifndef SDL2_SYS_DEFS_H__
#define SDL2_SYS_DEFS_H__

#include "../definitions.h"

EXPORT_API uintptr_t UGBA_MemBIOS(void);
EXPORT_API uintptr_t UGBA_MemEWRAM(void);
EXPORT_API uintptr_t UGBA_MemIWRAM(void);
EXPORT_API uintptr_t UGBA_MemIO(void);
EXPORT_API uintptr_t UGBA_MemPalette(void);
EXPORT_API uintptr_t UGBA_MemVRAM(void);
EXPORT_API uintptr_t UGBA_MemOAM(void);
EXPORT_API uintptr_t UGBA_MemROM(void);
EXPORT_API uintptr_t UGBA_MemSRAM(void);

#define MEM_BIOS_ADDR       (UGBA_MemBIOS())
#define MEM_EWRAM_ADDR      (UGBA_MemEWRAM())
#define MEM_IWRAM_ADDR      (UGBA_MemIWRAM())
#define MEM_IO_ADDR         (UGBA_MemIO())
#define MEM_PALETTE_ADDR    (UGBA_MemPalette())
#define MEM_VRAM_ADDR       (UGBA_MemVRAM())
#define MEM_OAM_ADDR        (UGBA_MemOAM())
#define MEM_ROM_ADDR_WS0    (UGBA_MemROM())
#define MEM_ROM_ADDR_WS1    MEM_ROM_ADDR_WS0
#define MEM_ROM_ADDR_WS2    MEM_ROM_ADDR_WS0
#define MEM_SRAM_ADDR       (UGBA_MemSRAM())

#endif // SDL2_SYS_DEFS_H__
