// SPDX-License-Identifier: MIT
//
// Copyright (c) 2020 Antonio Niño Díaz

#ifndef GBA_SYS_DEFS_H__
#define GBA_SYS_DEFS_H__

#define MEM_BIOS_ADDR      (0x00000000)
#define MEM_EWRAM_ADDR     (0x02000000)
#define MEM_IWRAM_ADDR     (0x03000000)
#define MEM_IO_ADDR        (0x04000000)
#define MEM_PALETTE_ADDR   (0x05000000)
#define MEM_VRAM_ADDR      (0x06000000)
#define MEM_OAM_ADDR       (0x07000000)
#define MEM_ROM_ADDR_WS0   (0x08000000) // Wait State 0
#define MEM_ROM_ADDR_WS1   (0x0A000000) // Wait State 1
#define MEM_ROM_ADDR_WS2   (0x0C000000) // Wait State 2
#define MEM_SRAM_ADDR      (0x0E000000)

#endif // GBA_SYS_DEFS_H__
