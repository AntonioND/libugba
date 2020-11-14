// SPDX-License-Identifier: MIT
//
// Copyright (c) 2020 Antonio Niño Díaz

#ifndef SDL2_SYS_DEFS_H__
#define SDL2_SYS_DEFS_H__

extern uint64_t internal_bios[MEM_BIOS_SIZE / sizeof(uint64_t)];
extern uint64_t internal_ewram[MEM_EWRAM_SIZE / sizeof(uint64_t)];
extern uint64_t internal_iwram[MEM_IWRAM_SIZE / sizeof(uint64_t)];
extern uint64_t internal_io[MEM_IO_SIZE / sizeof(uint64_t)];
extern uint64_t internal_palette[MEM_PALETTE_SIZE / sizeof(uint64_t)];
extern uint64_t internal_vram[MEM_VRAM_SIZE / sizeof(uint64_t)];
extern uint64_t internal_oam[MEM_OAM_SIZE / sizeof(uint64_t)];
extern uint64_t internal_rom[MEM_ROM_SIZE / sizeof(uint64_t)];
extern uint64_t internal_sram[MEM_SRAM_SIZE / sizeof(uint64_t)];
# define MEM_BIOS_ADDR      ((uintptr_t)(&internal_bios[0]))
# define MEM_EWRAM_ADDR     ((uintptr_t)(&internal_ewram[0]))
# define MEM_IWRAM_ADDR     ((uintptr_t)(&internal_iwram[0]))
# define MEM_IO_ADDR        ((uintptr_t)(&internal_io[0]))
# define MEM_PALETTE_ADDR   ((uintptr_t)(&internal_palette[0]))
# define MEM_VRAM_ADDR      ((uintptr_t)(&internal_vram[0]))
# define MEM_OAM_ADDR       ((uintptr_t)(&internal_oam[0]))
# define MEM_ROM_ADDR_WS0   ((uintptr_t)(&internal_rom[0]))
# define MEM_ROM_ADDR_WS1   MEM_ROM_ADDR_WS0
# define MEM_ROM_ADDR_WS2   MEM_ROM_ADDR_WS0
# define MEM_SRAM_ADDR      ((uintptr_t)(&internal_sram[0]))

#endif // SDL2_SYS_DEFS_H__
