// SPDX-License-Identifier: MIT
//
// Copyright (c) 2020 Antonio Niño Díaz

#ifndef GBA_DEFS_H__
#define GBA_DEFS_H__

#include <stdint.h>

// Memory Regions Definitions
// --------------------------

// BIOS (16 KiB)
#define MEM_BIOS_SIZE       (0x00004000)
#define MEM_BIOS            ((void *)MEM_BIOS_ADDR)

// EWRAM (256 KiB)
#define MEM_EWRAM_SIZE      (0x00040000)
#define MEM_EWRAM           ((void *)MEM_EWRAM_ADDR)

// IWRAM (32 KiB)
#define MEM_IWRAM_SIZE      (0x00008000)
#define MEM_IWRAM           ((void *)MEM_IWRAM_ADDR)

// I/O Registers
#define MEM_IO_SIZE         (0x00000400)
#define MEM_IO              ((void *)MEM_IO_ADDR)

// BG/OBJ Palette RAM (1 KiB)
#define MEM_PALETTE_SIZE    (0x00000400)
#define MEM_PALETTE         ((void *)MEM_PALETTE_ADDR)

// VRAM - Video RAM (96 KiB)
#define MEM_VRAM_SIZE       (0x00018000)
#define MEM_VRAM            ((void *)MEM_VRAM_ADDR)

// OAM - OBJ Attributes (1 KiB)
#define MEM_OAM_SIZE        (0x00000400)
#define MEM_OAM             ((void *)MEM_OAM_ADDR)

// Game Pak ROM/FlashROM (max 32MiB)
#define MEM_ROM_SIZE        (0x02000000)
#define MEM_ROM_WS0         ((void *)MEM_ROM_ADDR_WS0)
#define MEM_ROM_WS1         ((void *)MEM_ROM_ADDR_WS1)
#define MEM_ROM_WS2         ((void *)MEM_ROM_ADDR_WS2)

// Game Pak SRAM (max 64 KiB)
#define MEM_SRAM_SIZE       (0x00010000)
#define MEM_SRAM            ((void *)MEM_SRAM_ADDR)

// Base addresses of all regions
#ifdef __GBA__
# include "gba/sys_defs.h"
#else
# include "sdl2/sys_defs.h"
#endif

// I/O Registers Definitions
// -------------------------

// LCD I/O Registers
#define OFFSET_DISPCNT      (0x000) // R/W
#define OFFSET_GREENSWAP    (0x002) // R/W
#define OFFSET_DISPSTAT     (0x004) // R/W
#define OFFSET_VCOUNT       (0x006) // R
#define OFFSET_BG0CNT       (0x008) // R/W
#define OFFSET_BG1CNT       (0x00A) // R/W
#define OFFSET_BG2CNT       (0x00C) // R/W
#define OFFSET_BG3CNT       (0x00E) // R/W
#define OFFSET_BG0HOFS      (0x010) // W
#define OFFSET_BG0VOFS      (0x012) // W
#define OFFSET_BG1HOFS      (0x014) // W
#define OFFSET_BG1VOFS      (0x016) // W
#define OFFSET_BG2HOFS      (0x018) // W
#define OFFSET_BG2VOFS      (0x01A) // W
#define OFFSET_BG3HOFS      (0x01C) // W
#define OFFSET_BG3VOFS      (0x01E) // W
#define OFFSET_BG2PA        (0x020) // W
#define OFFSET_BG2PB        (0x022) // W
#define OFFSET_BG2PC        (0x024) // W
#define OFFSET_BG2PD        (0x026) // W
#define OFFSET_BG2X_L       (0x028) // W
#define OFFSET_BG2X_H       (0x02A) // W
#define OFFSET_BG2Y_L       (0x02C) // W
#define OFFSET_BG2Y_H       (0x02E) // W
#define OFFSET_BG3PA        (0x030) // W
#define OFFSET_BG3PB        (0x032) // W
#define OFFSET_BG3PC        (0x034) // W
#define OFFSET_BG3PD        (0x036) // W
#define OFFSET_BG3X_L       (0x038) // W
#define OFFSET_BG3X_H       (0x03A) // W
#define OFFSET_BG3Y_L       (0x03C) // W
#define OFFSET_BG3Y_H       (0x03E) // W
#define OFFSET_WIN0H        (0x040) // W
#define OFFSET_WIN1H        (0x042) // W
#define OFFSET_WIN0V        (0x044) // W
#define OFFSET_WIN1V        (0x046) // W
#define OFFSET_WININ        (0x048) // R/W
#define OFFSET_WINOUT       (0x04A) // R/W
#define OFFSET_MOSAIC       (0x04C) // W
#define OFFSET_BLDCNT       (0x050) // R/W
#define OFFSET_BLDALPHA     (0x052) // W
#define OFFSET_BLDY         (0x054) // W

// Sound Registers
#define OFFSET_SOUND1CNT_L  (0x060) // R/W
#define OFFSET_SOUND1CNT_H  (0x062) // R/W
#define OFFSET_SOUND1CNT_X  (0x064) // R/W
#define OFFSET_SOUND2CNT_L  (0x068) // R/W
#define OFFSET_SOUND2CNT_H  (0x06C) // R/W
#define OFFSET_SOUND3CNT_L  (0x070) // R/W
#define OFFSET_SOUND3CNT_H  (0x072) // R/W
#define OFFSET_SOUND3CNT_X  (0x074) // R/W
#define OFFSET_SOUND4CNT_L  (0x078) // R/W
#define OFFSET_SOUND4CNT_H  (0x07C) // R/W
#define OFFSET_SOUNDCNT_L   (0x080) // R/W
#define OFFSET_SOUNDCNT_H   (0x082) // R/W
#define OFFSET_SOUNDCNT_X   (0x084) // R/W
#define OFFSET_SOUNDBIAS    (0x088) // BIOS
#define OFFSET_WAVE_RAM     (0x090) // R/W -- 2x10h bytes
#define OFFSET_FIFO_A       (0x0A0) // W
#define OFFSET_FIFO_B       (0x0A4) // W

// DMA Transfer Channels
#define OFFSET_DMA0SAD      (0x0B0) // W
#define OFFSET_DMA0DAD      (0x0B4) // W
#define OFFSET_DMA0CNT_L    (0x0B8) // W
#define OFFSET_DMA0CNT_H    (0x0BA) // R/W
#define OFFSET_DMA1SAD      (0x0BC) // W
#define OFFSET_DMA1DAD      (0x0C0) // W
#define OFFSET_DMA1CNT_L    (0x0C4) // W
#define OFFSET_DMA1CNT_H    (0x0C6) // R/W
#define OFFSET_DMA2SAD      (0x0C8) // W
#define OFFSET_DMA2DAD      (0x0CC) // W
#define OFFSET_DMA2CNT_L    (0x0D0) // W
#define OFFSET_DMA2CNT_H    (0x0D2) // R/W
#define OFFSET_DMA3SAD      (0x0D4) // W
#define OFFSET_DMA3DAD      (0x0D8) // W
#define OFFSET_DMA3CNT_L    (0x0DC) // W
#define OFFSET_DMA3CNT_H    (0x0DE) // R/W

// Timer Registers
#define OFFSET_TM0CNT_L     (0x100) // R/W
#define OFFSET_TM0CNT_H     (0x102) // R/W
#define OFFSET_TM1CNT_L     (0x104) // R/W
#define OFFSET_TM1CNT_H     (0x106) // R/W
#define OFFSET_TM2CNT_L     (0x108) // R/W
#define OFFSET_TM2CNT_H     (0x10A) // R/W
#define OFFSET_TM3CNT_L     (0x10C) // R/W
#define OFFSET_TM3CNT_H     (0x10E) // R/W

// Serial Communication (1)
#define OFFSET_SIODATA32    (0x120) // R/W
#define OFFSET_SIOMULTI0    (0x120) // R/W
#define OFFSET_SIOMULTI1    (0x122) // R/W
#define OFFSET_SIOMULTI2    (0x124) // R/W
#define OFFSET_SIOMULTI3    (0x126) // R/W
#define OFFSET_SIOCNT       (0x128) // R/W
#define OFFSET_SIOMLT_SEND  (0x12A) // R/W
#define OFFSET_SIODATA8     (0x12A) // R/W

// Keypad Input
#define OFFSET_KEYINPUT     (0x130) // R
#define OFFSET_KEYCNT       (0x132) // R/W

// Serial Communication (2)
#define OFFSET_RCNT         (0x134) // R/W
#define OFFSET_JOYCNT       (0x140) // R/W
#define OFFSET_JOY_RECV     (0x150) // R/W
#define OFFSET_JOY_TRANS    (0x154) // R/W
#define OFFSET_JOYSTAT      (0x158) // R/?

// Interrupt, Waitstate, and Power-Down Control
#define OFFSET_IE           (0x200) // R/W
#define OFFSET_IF           (0x202) // R/W
#define OFFSET_WAITCNT      (0x204) // R/W
#define OFFSET_IME          (0x208) // R/W
#define OFFSET_POSTFLG      (0x300) // R/W
#define OFFSET_HALTCNT      (0x301) // W

//------------------------------------------

#define REG_8(r)    *((volatile uint8_t *)(MEM_IO_ADDR + (r)))
#define REG_16(r)   *((volatile uint16_t *)(MEM_IO_ADDR + (r)))
#define REG_32(r)   *((volatile uint32_t *)(MEM_IO_ADDR + (r)))

//------------------------------------------

#define REG_DISPCNT         REG_16(OFFSET_DISPCNT)
#define REG_GREENSWAP       REG_16(OFFSET_GREENSWAP)
#define REG_DISPSTAT        REG_16(OFFSET_DISPSTAT)
#define REG_VCOUNT          REG_16(OFFSET_VCOUNT)
#define REG_BG0CNT          REG_16(OFFSET_BG0CNT)
#define REG_BG1CNT          REG_16(OFFSET_BG1CNT)
#define REG_BG2CNT          REG_16(OFFSET_BG2CNT)
#define REG_BG3CNT          REG_16(OFFSET_BG3CNT)
#define REG_BG0HOFS         REG_16(OFFSET_BG0HOFS)
#define REG_BG0VOFS         REG_16(OFFSET_BG0VOFS)
#define REG_BG1HOFS         REG_16(OFFSET_BG1HOFS)
#define REG_BG1VOFS         REG_16(OFFSET_BG1VOFS)
#define REG_BG2HOFS         REG_16(OFFSET_BG2HOFS)
#define REG_BG2VOFS         REG_16(OFFSET_BG2VOFS)
#define REG_BG3HOFS         REG_16(OFFSET_BG3HOFS)
#define REG_BG3VOFS         REG_16(OFFSET_BG3VOFS)
#define REG_BG2PA           REG_16(OFFSET_BG2PA)
#define REG_BG2PB           REG_16(OFFSET_BG2PB)
#define REG_BG2PC           REG_16(OFFSET_BG2PC)
#define REG_BG2PD           REG_16(OFFSET_BG2PD)
#define REG_BG2X_L          REG_16(OFFSET_BG2X_L)
#define REG_BG2X_H          REG_16(OFFSET_BG2X_H)
#define REG_BG2X            REG_32(OFFSET_BG2X_L)
#define REG_BG2Y_L          REG_16(OFFSET_BG2Y_L)
#define REG_BG2Y_H          REG_16(OFFSET_BG2Y_H)
#define REG_BG2Y            REG_32(OFFSET_BG2Y_L)
#define REG_BG3PA           REG_16(OFFSET_BG3PA)
#define REG_BG3PB           REG_16(OFFSET_BG3PB)
#define REG_BG3PC           REG_16(OFFSET_BG3PC)
#define REG_BG3PD           REG_16(OFFSET_BG3PD)
#define REG_BG3X_L          REG_16(OFFSET_BG3X_L)
#define REG_BG3X_H          REG_16(OFFSET_BG3X_H)
#define REG_BG3X            REG_32(OFFSET_BG3X_L)
#define REG_BG3Y_L          REG_16(OFFSET_BG3Y_L)
#define REG_BG3Y_H          REG_16(OFFSET_BG3Y_H)
#define REG_BG3Y            REG_32(OFFSET_BG3Y_L)
#define REG_WIN0H           REG_16(OFFSET_WIN0H)
#define REG_WIN1H           REG_16(OFFSET_WIN1H)
#define REG_WIN0V           REG_16(OFFSET_WIN0V)
#define REG_WIN1V           REG_16(OFFSET_WIN1V)
#define REG_WININ           REG_16(OFFSET_WININ)
#define REG_WINOUT          REG_16(OFFSET_WINOUT)
#define REG_MOSAIC          REG_16(OFFSET_MOSAIC)
#define REG_BLDCNT          REG_16(OFFSET_BLDCNT)
#define REG_BLDALPHA        REG_16(OFFSET_BLDALPHA)
#define REG_BLDY            REG_16(OFFSET_BLDY)
#define REG_SOUND1CNT_L     REG_16(OFFSET_SOUND1CNT_L)
#define REG_SOUND1CNT_H     REG_16(OFFSET_SOUND1CNT_H)
#define REG_SOUND1CNT_X     REG_16(OFFSET_SOUND1CNT_X)
#define REG_SOUND2CNT_L     REG_16(OFFSET_SOUND2CNT_L)
#define REG_SOUND2CNT_H     REG_16(OFFSET_SOUND2CNT_H)
#define REG_SOUND3CNT_L     REG_16(OFFSET_SOUND3CNT_L)
#define REG_SOUND3CNT_H     REG_16(OFFSET_SOUND3CNT_H)
#define REG_SOUND3CNT_X     REG_16(OFFSET_SOUND3CNT_X)
#define REG_SOUND4CNT_L     REG_16(OFFSET_SOUND4CNT_L)
#define REG_SOUND4CNT_H     REG_16(OFFSET_SOUND4CNT_H)
#define REG_SOUNDCNT_L      REG_16(OFFSET_SOUNDCNT_L)
#define REG_SOUNDCNT_H      REG_16(OFFSET_SOUNDCNT_H)
#define REG_SOUNDCNT_X      REG_16(OFFSET_SOUNDCNT_X)
#define REG_SOUNDBIAS       REG_16(OFFSET_SOUNDBIAS)
#define REG_WAVE_RAM        ((uint8_t *)(OFFSET_WAVE_RAM))
#define REG_FIFO_A          REG_32(OFFSET_FIFO_A)
#define REG_FIFO_B          REG_32(OFFSET_FIFO_B)
#define REG_DMA0SAD         REG_32(OFFSET_DMA0SAD)
#define REG_DMA0DAD         REG_32(OFFSET_DMA0DAD)
#define REG_DMA0CNT_L       REG_16(OFFSET_DMA0CNT_L)
#define REG_DMA0CNT_H       REG_16(OFFSET_DMA0CNT_H)
#define REG_DMA0CNT         REG_32(OFFSET_DMA0CNT_L)
#define REG_DMA1SAD         REG_32(OFFSET_DMA1SAD)
#define REG_DMA1DAD         REG_32(OFFSET_DMA1DAD)
#define REG_DMA1CNT_L       REG_16(OFFSET_DMA1CNT_L)
#define REG_DMA1CNT_H       REG_16(OFFSET_DMA1CNT_H)
#define REG_DMA1CNT         REG_32(OFFSET_DMA1CNT_L)
#define REG_DMA2SAD         REG_32(OFFSET_DMA2SAD)
#define REG_DMA2DAD         REG_32(OFFSET_DMA2DAD)
#define REG_DMA2CNT_L       REG_16(OFFSET_DMA2CNT_L)
#define REG_DMA2CNT_H       REG_16(OFFSET_DMA2CNT_H)
#define REG_DMA2CNT         REG_32(OFFSET_DMA2CNT_L)
#define REG_DMA3SAD         REG_32(OFFSET_DMA3SAD)
#define REG_DMA3DAD         REG_32(OFFSET_DMA3DAD)
#define REG_DMA3CNT_L       REG_16(OFFSET_DMA3CNT_L)
#define REG_DMA3CNT_H       REG_16(OFFSET_DMA3CNT_H)
#define REG_DMA3CNT         REG_32(OFFSET_DMA3CNT_L)
#define REG_TM0CNT_L        REG_16(OFFSET_TM0CNT_L)
#define REG_TM0CNT_H        REG_16(OFFSET_TM0CNT_H)
#define REG_TM1CNT_L        REG_16(OFFSET_TM1CNT_L)
#define REG_TM1CNT_H        REG_16(OFFSET_TM1CNT_H)
#define REG_TM2CNT_L        REG_16(OFFSET_TM2CNT_L)
#define REG_TM2CNT_H        REG_16(OFFSET_TM2CNT_H)
#define REG_TM3CNT_L        REG_16(OFFSET_TM3CNT_L)
#define REG_TM3CNT_H        REG_16(OFFSET_TM3CNT_H)
#define REG_SIODATA32       REG_32(OFFSET_SIODATA32)
#define REG_SIOMULTI0       REG_16(OFFSET_SIOMULTI0)
#define REG_SIOMULTI1       REG_16(OFFSET_SIOMULTI1)
#define REG_SIOMULTI2       REG_16(OFFSET_SIOMULTI2)
#define REG_SIOMULTI3       REG_16(OFFSET_SIOMULTI3)
#define REG_SIOCNT          REG_16(OFFSET_SIOCNT)
#define REG_SIOMLT_SEND     REG_16(OFFSET_SIOMLT_SEND)
#define REG_SIODATA8        REG_16(OFFSET_SIODATA8)
#define REG_KEYINPUT        REG_16(OFFSET_KEYINPUT)
#define REG_KEYCNT          REG_16(OFFSET_KEYCNT)
#define REG_RCNT            REG_16(OFFSET_RCNT)

#define REG_JOYCNT          REG_16(OFFSET_JOYCNT)
#define REG_JOY_RECV        REG_32(OFFSET_JOY_RECV)
#define REG_JOY_TRANS       REG_32(OFFSET_JOY_TRANS)
#define REG_JOYSTAT         REG_16(OFFSET_JOYSTAT)
#define REG_IE              REG_16(OFFSET_IE)
#define REG_IF              REG_16(OFFSET_IF)
#define REG_WAITCNT         REG_16(OFFSET_WAITCNT)
#define REG_IME             REG_16(OFFSET_IME)
#define REG_POSTFLG         REG_8(OFFSET_POSTFLG)
#define REG_HALTCNT         REG_8(OFFSET_HALTCNT)

// The GBA_RegisterUpdated() function must be called in some circumstances for
// the values of the registers to be reflected immediately in the hardware
// emulation.
//
// 1) When modifying the following registers dynamically during the screen
//    redraw (in the HBL interrupt handler, for example). It isn't needed if the
//    values are static during the whole frame.
//
//        REG_BG2X_L, REG_BG2X_H, REG_BG2X
//        REG_BG2Y_L, REG_BG2Y_H, REG_BG2Y
//        REG_BG3X_L, REG_BG3X_H, REG_BG3X
//        REG_BG3Y_L, REG_BG3Y_H, REG_BG3Y

#ifdef __GBA__
# define GBA_RegisterUpdated(reg) do { } while (0)
#else
# define GBA_RegisterUpdated(reg) \
         GBA_RegisterUpdatedOffset(((uintptr_t)&reg) - MEM_IO_ADDR)
void GBA_RegisterUpdatedOffset(uint32_t offset);
#endif

// Per-register definitions
// ------------------------

// DISPSTAT

#define DISPSTAT_VBLANK_IRQ_ENABLE      (1 << 3)
#define DISPSTAT_HBLANK_IRQ_ENABLE      (1 << 4)
#define DISPSTAT_VCOUNT_IRQ_ENABLE      (1 << 5)

#endif // GBA_DEFS_H__
