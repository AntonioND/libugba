// SPDX-License-Identifier: MIT
//
// Copyright (c) 2020 Antonio Niño Díaz

#ifndef HARDWARE_H__
#define HARDWARE_H__

#include <stdint.h>

#include "definitions.h"

// Device definitions
// ------------------

#define GBA_SCREEN_W    240
#define GBA_SCREEN_H    160

static inline uint16_t RGB15(uint16_t r, uint16_t g, uint16_t b)
{
    return (r & 0x1F) | ((g & 0x1F) << 5) | ((b & 0x1F) << 10);
}

// Memory Regions Definitions
// ==========================

// BIOS (16 KiB)
#define MEM_BIOS_SIZE           (0x00004000)
#define MEM_BIOS                ((void *)MEM_BIOS_ADDR)

// EWRAM (256 KiB)
#define MEM_EWRAM_SIZE          (0x00040000)
#define MEM_EWRAM               ((void *)MEM_EWRAM_ADDR)

// IWRAM (32 KiB)
#define MEM_IWRAM_SIZE          (0x00008000)
#define MEM_IWRAM               ((void *)MEM_IWRAM_ADDR)

// I/O Registers
#define MEM_IO_SIZE             (0x00000400)
#define MEM_IO                  ((void *)MEM_IO_ADDR)

// BG/OBJ Palette RAM (1 KiB)
#define MEM_PALETTE_SIZE        (0x00000400)
#define MEM_PALETTE             ((void *)MEM_PALETTE_ADDR)

#define MEM_PALETTE_BG          ((uint16_t *)MEM_PALETTE)
#define MEM_PALETTE_OBJ         ((uint16_t *)(MEM_PALETTE_ADDR + 0x200))

#define MEM_BACKDROP_COLOR      (MEM_PALETTE_BG[0])

// VRAM - Video RAM (96 KiB)
#define MEM_VRAM_SIZE           (0x00018000)
#define MEM_VRAM                ((void *)MEM_VRAM_ADDR)

#define MEM_VRAM_BG             MEM_VRAM
#define MEM_VRAM_BG_SIZE        (0x00010000)
#define MEM_VRAM_OBJ            ((void *)(MEM_VRAM_ADDR + MEM_VRAM_BG_SIZE))
#define MEM_VRAM_OBJ_SIZE       (0x00008000)

#define MEM_BG_TILES_BLOCK_SIZE (0x4000)
#define MEM_BG_TILES_BLOCK_ADDR(n) \
                (((n) * MEM_BG_TILES_BLOCK_SIZE) + MEM_VRAM_ADDR)

#define MEM_BG_MAP_BLOCK_SIZE   (0x800)
#define MEM_BG_MAP_BLOCK_ADDR(n) \
                (((n) * MEM_BG_MAP_BLOCK_SIZE) + MEM_VRAM_ADDR)

#define MEM_VRAM_MODE3_FB       ((uint16_t *)MEM_VRAM)

#define MEM_VRAM_MODE4_FRAME0   ((uint8_t *)MEM_VRAM_ADDR)
#define MEM_VRAM_MODE4_FRAME1   ((uint8_t *)(MEM_VRAM_ADDR + 40 * 1024))

#define MEM_VRAM_MODE5_FRAME0   ((uint16_t *)MEM_VRAM_ADDR)
#define MEM_VRAM_MODE5_FRAME1   ((uint16_t *)(MEM_VRAM_ADDR + 40 * 1024))

// OAM - OBJ Attributes (1 KiB)
#define MEM_OAM_SIZE            (0x00000400)
#define MEM_OAM                 ((void *)MEM_OAM_ADDR)

// Game Pak ROM/FlashROM (max 32MiB)
#define MEM_ROM_SIZE            (0x02000000)
#define MEM_ROM_WS0             ((void *)MEM_ROM_ADDR_WS0)
#define MEM_ROM_WS1             ((void *)MEM_ROM_ADDR_WS1)
#define MEM_ROM_WS2             ((void *)MEM_ROM_ADDR_WS2)

// Game Pak SRAM (max 64 KiB)
#define MEM_SRAM_SIZE           (0x00010000)
#define MEM_SRAM                ((void *)MEM_SRAM_ADDR)

// Base addresses of all regions

#ifdef __GBA__

# define MEM_BIOS_ADDR      (0x00000000)
# define MEM_EWRAM_ADDR     (0x02000000)
# define MEM_IWRAM_ADDR     (0x03000000)
# define MEM_IO_ADDR        (0x04000000)
# define MEM_PALETTE_ADDR   (0x05000000)
# define MEM_VRAM_ADDR      (0x06000000)
# define MEM_OAM_ADDR       (0x07000000)
# define MEM_ROM_ADDR_WS0   (0x08000000) // Wait State 0
# define MEM_ROM_ADDR_WS1   (0x0A000000) // Wait State 1
# define MEM_ROM_ADDR_WS2   (0x0C000000) // Wait State 2
# define MEM_SRAM_ADDR      (0x0E000000)

#else

EXPORT_API uintptr_t UGBA_MemBIOS(void);
EXPORT_API uintptr_t UGBA_MemEWRAM(void);
EXPORT_API uintptr_t UGBA_MemIWRAM(void);
EXPORT_API uintptr_t UGBA_MemIO(void);
EXPORT_API uintptr_t UGBA_MemPalette(void);
EXPORT_API uintptr_t UGBA_MemVRAM(void);
EXPORT_API uintptr_t UGBA_MemOAM(void);
EXPORT_API uintptr_t UGBA_MemROM(void);
EXPORT_API uintptr_t UGBA_MemSRAM(void);

# define MEM_BIOS_ADDR      (UGBA_MemBIOS())
# define MEM_EWRAM_ADDR     (UGBA_MemEWRAM())
# define MEM_IWRAM_ADDR     (UGBA_MemIWRAM())
# define MEM_IO_ADDR        (UGBA_MemIO())
# define MEM_PALETTE_ADDR   (UGBA_MemPalette())
# define MEM_VRAM_ADDR      (UGBA_MemVRAM())
# define MEM_OAM_ADDR       (UGBA_MemOAM())
# define MEM_ROM_ADDR_WS0   (UGBA_MemROM())
# define MEM_ROM_ADDR_WS1   MEM_ROM_ADDR_WS0
# define MEM_ROM_ADDR_WS2   MEM_ROM_ADDR_WS0
# define MEM_SRAM_ADDR      (UGBA_MemSRAM())

#endif // __GBA__

// Tilemap definitions
// ===================

// 16 bits per entry
#define MAP_REGULAR_TILE(v)         ((v) & 0x3FF)
#define MAP_REGULAR_HFLIP           (1 << 10)
#define MAP_REGULAR_VFLIP           (1 << 11)
#define MAP_REGULAR_PALETTE(v)      (((v) & 0xF) << 12) // 16 color backgrounds

// 8 bits per entry
#define MAP_AFFINE_TILE(v)          ((v) & 0xFF)

// OAM Definitions
// ===============

#pragma pack(push, 1)
typedef struct {
    uint16_t attr0;
    uint16_t attr1;
    uint16_t attr2;
    uint16_t padding;
} oam_entry_t;
#pragma pack(pop)

#define MEM_OAM_NUMBER_ENTRIES      (128)
#define MEM_OAM_ENTRIES             ((oam_entry_t *)MEM_OAM)

// Attribute 0

#define ATTR0_Y(v)                  ((v) & 0xFF)
#define ATTR0_Y_MASK                (0xFF)

#define ATTR0_REGULAR               (0 << 8)
#define ATTR0_AFFINE                (1 << 8)

#define ATTR0_DOUBLE_SIZE           (1 << 9) // Affine objects
#define ATTR0_DISABLE               (1 << 9) // Regular objects

#define ATTR0_MODE_MASK             (3 << 10)
#define ATTR0_MODE_NORMAL           (0 << 10)
#define ATTR0_MODE_TRANSPARENT      (1 << 10)
#define ATTR0_MODE_WINDOW           (2 << 10)

#define ATTR0_MOSAIC                (1 << 12)

#define ATTR0_16_COLORS             (0 << 13)
#define ATTR0_256_COLORS            (1 << 13)

#define ATTR0_SHAPE_MASK            (3 << 14)
#define ATTR0_SHAPE_SQUARE          (0 << 14)
#define ATTR0_SHAPE_HORIZONTAL      (1 << 14)
#define ATTR0_SHAPE_VERTICAL        (2 << 14)

// Attribute 1

#define ATTR1_X(v)                  ((v) & 0xFF)
#define ATTR1_X_MASK                (0xFF)

#define ATTR1_AFFINE_MATRIX(v)      (((v) & 0x1F) << 9) // Affine objects
#define ATTR1_AFFINE_MATRIX_MASK    (0x1F << 9) // Affine objects

#define ATTR1_REGULAR_HFLIP         (1 << 12) // Regular objects
#define ATTR1_REGULAR_VFLIP         (1 << 13) // Regular objects

#define ATTR1_SIZE_MASK             (3 << 14)
#define ATTR1_SIZE_0                (0 << 14)
#define ATTR1_SIZE_1                (1 << 14)
#define ATTR1_SIZE_2                (2 << 14)
#define ATTR1_SIZE_3                (3 << 14)

// Attribute 2

#define ATTR2_16_COLOR_TILE(v)      ((v) & 0x3FF)
#define ATTR2_256_COLOR_TILE(v)     (((v) & 0x1FF) << 1)
#define ATTR2_TILE_MASK             (0x3FF)

#define ATTR2_PRIORITY(v)           (((v) & 0x3) << 10)
#define ATTR2_PRIORITY_MASK         (0x3 << 10)

#define ATTR2_PALETTE(v)            (((v) & 0xF) << 12) // 16 color objects
#define ATTR2_PALETTE_MASK          (0xf << 12)

#pragma pack(push, 1)
typedef struct {
    uint16_t padding0[3];
    int16_t pa;
    uint16_t padding1[3];
    int16_t pb;
    uint16_t padding2[3];
    int16_t pc;
    uint16_t padding3[3];
    int16_t pd;
} oam_matrix_entry_t;
#pragma pack(pop)

#define MEM_OAM_NUMBER_MATRICES     (32)
#define MEM_OAM_MATRICES            ((oam_matrix_entry_t *)MEM_OAM)

// I/O Registers Definitions
// =========================

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
#define OFFSET_IME          (0x208) // R/W

//------------------------------------------

#define REG_8(r)            *((volatile uint8_t *)(MEM_IO_ADDR + (r)))
#define REG_16(r)           *((volatile uint16_t *)(MEM_IO_ADDR + (r)))
#define REG_32(r)           *((volatile uint32_t *)(MEM_IO_ADDR + (r)))

#define PTR_REG_16(r)       ((volatile uint16_t *)(MEM_IO_ADDR + (r)))

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

#define REG_DMA0CNT_L       REG_16(OFFSET_DMA0CNT_L)
#define REG_DMA0CNT_H       REG_16(OFFSET_DMA0CNT_H)
#define REG_DMA0CNT         REG_32(OFFSET_DMA0CNT_L)
#define REG_DMA1CNT_L       REG_16(OFFSET_DMA1CNT_L)
#define REG_DMA1CNT_H       REG_16(OFFSET_DMA1CNT_H)
#define REG_DMA1CNT         REG_32(OFFSET_DMA1CNT_L)
#define REG_DMA2CNT_L       REG_16(OFFSET_DMA2CNT_L)
#define REG_DMA2CNT_H       REG_16(OFFSET_DMA2CNT_H)
#define REG_DMA2CNT         REG_32(OFFSET_DMA2CNT_L)
#define REG_DMA3CNT_L       REG_16(OFFSET_DMA3CNT_L)
#define REG_DMA3CNT_H       REG_16(OFFSET_DMA3CNT_H)
#define REG_DMA3CNT         REG_32(OFFSET_DMA3CNT_L)

#ifdef __GBA__

# define REG_DMA0SAD        REG_32(OFFSET_DMA0SAD)
# define REG_DMA0DAD        REG_32(OFFSET_DMA0DAD)
# define REG_DMA1SAD        REG_32(OFFSET_DMA1SAD)
# define REG_DMA1DAD        REG_32(OFFSET_DMA1DAD)
# define REG_DMA2SAD        REG_32(OFFSET_DMA2SAD)
# define REG_DMA2DAD        REG_32(OFFSET_DMA2DAD)
# define REG_DMA3SAD        REG_32(OFFSET_DMA3SAD)
# define REG_DMA3DAD        REG_32(OFFSET_DMA3DAD)

#else // __GBA__

EXPORT_API uintptr_t *UGBA_RegDMA0SAD(void);
EXPORT_API uintptr_t *UGBA_RegDMA0DAD(void);
EXPORT_API uintptr_t *UGBA_RegDMA1SAD(void);
EXPORT_API uintptr_t *UGBA_RegDMA1DAD(void);
EXPORT_API uintptr_t *UGBA_RegDMA2SAD(void);
EXPORT_API uintptr_t *UGBA_RegDMA2DAD(void);
EXPORT_API uintptr_t *UGBA_RegDMA3SAD(void);
EXPORT_API uintptr_t *UGBA_RegDMA3DAD(void);

# define REG_DMA0SAD        (*UGBA_RegDMA0SAD())
# define REG_DMA0DAD        (*UGBA_RegDMA0DAD())
# define REG_DMA1SAD        (*UGBA_RegDMA1SAD())
# define REG_DMA1DAD        (*UGBA_RegDMA1DAD())
# define REG_DMA2SAD        (*UGBA_RegDMA2SAD())
# define REG_DMA2DAD        (*UGBA_RegDMA2DAD())
# define REG_DMA3SAD        (*UGBA_RegDMA3SAD())
# define REG_DMA3DAD        (*UGBA_RegDMA3DAD())

#endif // __GBA__

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
#define REG_IME             REG_16(OFFSET_IME)

// The UGBA_RegisterUpdatedOffset() function must be called in some
// circumstances for the values of the registers to be reflected immediately in
// the hardware emulation.
//
// 1) When modifying the following registers dynamically during the screen
//    redraw (in the HBL interrupt handler, for example). It isn't needed if the
//    values are static during the whole frame.
//
//        REG_BG2X_L, REG_BG2X_H, REG_BG2X
//        REG_BG2Y_L, REG_BG2Y_H, REG_BG2Y
//        REG_BG3X_L, REG_BG3X_H, REG_BG3X
//        REG_BG3Y_L, REG_BG3Y_H, REG_BG3Y
//
// 2) When starting a DMA transfer by writing to:
//
//        REG_DMA0CNT_H, REG_DMA1CNT_H, REG_DMA2CNT_H, REG_DMA3CNT_H
//
// 3) When starting a timer by writing to:
//
//        REG_TM0CNT_H, REG_TM1CNT_H, REG_TM2CNT_H, REG_TM3CNT_H

#ifdef __GBA__
# define UGBA_RegisterUpdatedOffset(offset) do { (void)(offset); } while (0)
#else
EXPORT_API void UGBA_RegisterUpdatedOffset(uint32_t offset);
#endif

// Per-register fields definitions
// ===============================

// LCD I/O Registers
// -----------------

// DISPCNT

#define DISPCNT_BG_MODE_MASK            (0x7)
#define DISPCNT_BG_MODE(n)              ((n) & DISPCNT_BG_MODE_MASK) // 0 to 5

#define DISPCNT_USE_FRONTBUFFER         (0 << 4)
#define DISPCNT_USE_BACKBUFFER          (1 << 4)

#define DISPCNT_HBL_INTERVAL_FREE       (1 << 5)

#define DISPCNT_OBJ_2D_MAPPING          (0 << 6)
#define DISPCNT_OBJ_1D_MAPPING          (1 << 6)

#define DISPCNT_FORCED_BLANK            (1 << 7)

#define DISPCNT_BG0_ENABLE              (1 << 8)
#define DISPCNT_BG1_ENABLE              (1 << 9)
#define DISPCNT_BG2_ENABLE              (1 << 10)
#define DISPCNT_BG3_ENABLE              (1 << 11)
#define DISPCNT_OBJ_ENABLE              (1 << 12)

#define DISPCNT_WIN0_ENABLE             (1 << 13)
#define DISPCNT_WIN1_ENABLE             (1 << 14)
#define DISPCNT_OBJ_WIN_ENABLE          (1 << 15)

// GREENSWAP

#define GREENSWAP_ENABLE                (1 << 0)

// DISPSTAT

#define DISPSTAT_IN_VBLANK              (1 << 0)
#define DISPSTAT_IN_HBLANK              (1 << 1)
#define DISPSTAT_VCOUNT_MATCH           (1 << 2)

#define DISPSTAT_VBLANK_IRQ_ENABLE      (1 << 3)
#define DISPSTAT_HBLANK_IRQ_ENABLE      (1 << 4)
#define DISPSTAT_VCOUNT_IRQ_ENABLE      (1 << 5)

#define DISPSTAT_VCOUNT_SHIFT           (8)
#define DISPSTAT_VCOUNT(n)              ((n) << DISPSTAT_VCOUNT_SHIFT)
#define DISPSTAT_VCOUNT_MASK            (0xFF << DISPSTAT_VCOUNT_SHIFT)

// VCOUNT

#define VCOUNT_MASK                     (0xFF)

// BG0CNT, BG1CNT, BG2CNT, BG3CNT

#define BGCNT_BG_PRIORITY_MASK          (0x3)
#define BGCNT_BG_PRIORITY(n)            ((n) & BGCNT_BG_PRIORITY_MASK)

// The base address is 0x06000000 + 0x4000 * x (0x06000000 to 0x0600C000)
#define BGCNT_TILES_BLOCK_BASE(n) \
                (((((n) - MEM_VRAM_ADDR) / MEM_BG_TILES_BLOCK_SIZE) & 0x3) << 2)

#define BGCNT_MOSAIC_ENABLE             (1 << 6)

#define BGCNT_16_COLORS                 (0 << 7)
#define BGCNT_256_COLORS                (1 << 7)

// The base address is 0x06000000 + 0x800 * x (0x06000000 to 0x0600F800)
#define BGCNT_MAP_BLOCK_BASE(n) \
                (((((n) - MEM_VRAM_ADDR) / MEM_BG_MAP_BLOCK_SIZE) & 0x1F) << 8)

#define BGCNT_BG2_BG3_WRAP              (1 << 13) // For affine backgrounds

#define BGCNT_REGULAR_256x256           (0 << 14)
#define BGCNT_REGULAR_512x256           (1 << 14)
#define BGCNT_REGULAR_256x512           (2 << 14)
#define BGCNT_REGULAR_512x512           (3 << 14)

#define BGCNT_AFFINE_128x128            (0 << 14)
#define BGCNT_AFFINE_256x256            (1 << 14)
#define BGCNT_AFFINE_512x512            (2 << 14)
#define BGCNT_AFFINE_1024x1024          (3 << 14)

// WIN0H, WIN1H

#define WINH_SET(left, right)   ((((left) & 0xFF) << 8) | ((right) & 0xFF))

// WIN0V, WIN1V

#define WINV_SET(top, bottom)   ((((top) & 0xFF) << 8) | ((bottom) & 0xFF))

// WININ, WINOUT

#define WIN0_BG0_ENABLE                 (1 << 0)
#define WIN0_BG1_ENABLE                 (1 << 1)
#define WIN0_BG2_ENABLE                 (1 << 2)
#define WIN0_BG3_ENABLE                 (1 << 3)
#define WIN0_OBJ_ENABLE                 (1 << 4)
#define WIN0_EFFECT_ENABLE              (1 << 5)

#define WIN1_BG0_ENABLE                 (1 << 8)
#define WIN1_BG1_ENABLE                 (1 << 9)
#define WIN1_BG2_ENABLE                 (1 << 10)
#define WIN1_BG3_ENABLE                 (1 << 11)
#define WIN1_OBJ_ENABLE                 (1 << 12)
#define WIN1_EFFECT_ENABLE              (1 << 13)

// MOSAIC

#define MOSAIC_BG_SIZE(h, v)    (((h) & 0xF) | (((v) & 0xF) << 4))
#define MOSAIC_OBJ_SIZE(h, v)   ((((h) & 0xF) << 8) | (((v) & 0xF) << 12))

// BLDCNT

#define BLDCNT_1ST_BG0                  (1 << 0)
#define BLDCNT_1ST_BG1                  (1 << 1)
#define BLDCNT_1ST_BG2                  (1 << 2)
#define BLDCNT_1ST_BG3                  (1 << 3)
#define BLDCNT_1ST_OBJ                  (1 << 4)
#define BLDCNT_1ST_BD                   (1 << 5)

#define BLDCNT_DISABLE                  (0 << 6)
#define BLDCNT_ALPHA_BLENDING           (1 << 6) // 1ST + 2ND
#define BLDCNT_BRIGTHNESS_INCRESE       (2 << 6) // 1ST
#define BLDCNT_BRIGTHNESS_DECREASE      (3 << 6) // 1ST

#define BLDCNT_2ND_BG0                  (1 << 8)
#define BLDCNT_2ND_BG1                  (1 << 9)
#define BLDCNT_2ND_BG2                  (1 << 10)
#define BLDCNT_2ND_BG3                  (1 << 11)
#define BLDCNT_2ND_OBJ                  (1 << 12)
#define BLDCNT_2ND_BD                   (1 << 13)

// BLDALPHA

#define BLDALPHA_EVA(v)                 ((v) & 0x1F)
#define BLDALPHA_EVB(v)                 (((v) & 0x1F) << 8)

// BLDY

#define BLDY_EVY(v)                     ((v) & 0x1F)

// Sound Registers
// ---------------

#if 0
  4000060h  2  R/W  SOUND1CNT_L Channel 1 Sweep register       (NR10)
  4000062h  2  R/W  SOUND1CNT_H Channel 1 Duty/Length/Envelope (NR11, NR12)
  4000064h  2  R/W  SOUND1CNT_X Channel 1 Frequency/Control    (NR13, NR14)
  4000068h  2  R/W  SOUND2CNT_L Channel 2 Duty/Length/Envelope (NR21, NR22)
  400006Ch  2  R/W  SOUND2CNT_H Channel 2 Frequency/Control    (NR23, NR24)
  4000070h  2  R/W  SOUND3CNT_L Channel 3 Stop/Wave RAM select (NR30)
  4000072h  2  R/W  SOUND3CNT_H Channel 3 Length/Volume        (NR31, NR32)
  4000074h  2  R/W  SOUND3CNT_X Channel 3 Frequency/Control    (NR33, NR34)
  4000078h  2  R/W  SOUND4CNT_L Channel 4 Length/Envelope      (NR41, NR42)
  400007Ch  2  R/W  SOUND4CNT_H Channel 4 Frequency/Control    (NR43, NR44)
  4000080h  2  R/W  SOUNDCNT_L  Control Stereo/Volume/Enable   (NR50, NR51)
  4000082h  2  R/W  SOUNDCNT_H  Control Mixing/DMA Control
  4000084h  2  R/W  SOUNDCNT_X  Control Sound on/off           (NR52)
  4000088h  2  BIOS SOUNDBIAS   Sound PWM Control
  4000090h 2x10h R/W  WAVE_RAM  Channel 3 Wave Pattern RAM (2 banks!!)
  40000A0h  4    W    FIFO_A    Channel A FIFO, Data 0-3
  40000A4h  4    W    FIFO_B    Channel B FIFO, Data 0-3
#endif

// DMA Transfer Channels
// ---------------------

// DMA0CNT_H, DMA1CNT_H, DMA2CNT_H, DMA3CNT_H

#define DMACNT_DST_INCREMENT            (0 << 5)
#define DMACNT_DST_DECREMENT            (1 << 5)
#define DMACNT_DST_FIXED                (2 << 5)
#define DMACNT_DST_RELOAD               (3 << 5)

#define DMACNT_SRC_INCREMENT            (0 << 7)
#define DMACNT_SRC_DECREMENT            (1 << 7)
#define DMACNT_SRC_FIXED                (2 << 7)

#define DMACNT_REPEAT_ON                (1 << 9)

#define DMACNT_TRANSFER_16_BITS         (0 << 10)
#define DMACNT_TRANSFER_32_BITS         (1 << 10)

#define DMACNT_GAME_PAK_NORMAL          (0 << 11) // DMA3 only
#define DMACNT_GAME_PAK_DRQ             (1 << 11) // DMA3 only

#define DMACNT_START_NOW                (0 << 12)
#define DMACNT_START_VBLANK             (1 << 12)
#define DMACNT_START_HBLANK             (2 << 12)
#define DMACNT_START_SPECIAL            (3 << 12) // DMA1, DMA2, DMA3

#define DMACNT_IRQ_ENABLE               (1 << 14)

#define DMACNT_DMA_ENABLE               (1 << 15)

// Timer Registers
// ---------------

// TM0CNT_H, TM1CNT_H, TM2CNT_H, TM3CNT_H

#define TMCNT_PRESCALER_F_DIV_1         (0 << 0)
#define TMCNT_PRESCALER_F_DIV_64        (1 << 0)
#define TMCNT_PRESCALER_F_DIV_256       (2 << 0)
#define TMCNT_PRESCALER_F_DIV_1024      (3 << 0)

#define TMCNT_STANDALONE                (0 << 2)
#define TMCNT_CASCADE                   (1 << 2) // Not used in TM0CNT_H

#define TMCNT_IRQ_DISABLE               (0 << 6)
#define TMCNT_IRQ_ENABLE                (1 << 6)

#define TMCNT_STOP                      (0 << 7)
#define TMCNT_START                     (1 << 7)

// Serial Communication (1)
// ------------------------

#if 0
  4000120h  4    R/W  SIODATA32 SIO Data (Normal-32bit Mode; shared with below)
  4000120h  2    R/W  SIOMULTI0 SIO Data 0 (Parent)    (Multi-Player Mode)
  4000122h  2    R/W  SIOMULTI1 SIO Data 1 (1st Child) (Multi-Player Mode)
  4000124h  2    R/W  SIOMULTI2 SIO Data 2 (2nd Child) (Multi-Player Mode)
  4000126h  2    R/W  SIOMULTI3 SIO Data 3 (3rd Child) (Multi-Player Mode)
  4000128h  2    R/W  SIOCNT    SIO Control Register
  400012Ah  2    R/W  SIOMLT_SEND SIO Data (Local of MultiPlayer; shared below)
  400012Ah  2    R/W  SIODATA8  SIO Data (Normal-8bit and UART Mode)
#endif

// Keypad Input
// ------------

//  KEYINPUT

#define KEY_A                           (1 << 0)
#define KEY_B                           (1 << 1)
#define KEY_SELECT                      (1 << 2)
#define KEY_START                       (1 << 3)
#define KEY_RIGHT                       (1 << 4)
#define KEY_LEFT                        (1 << 5)
#define KEY_UP                          (1 << 6)
#define KEY_DOWN                        (1 << 7)
#define KEY_R                           (1 << 8)
#define KEY_L                           (1 << 9)

// KEYCNT (Use the definitions from KEYINPUT too)

#define KEYCNT_IRQ_ENABLE               (1 << 14)
#define KEYCNT_IRQ_CONDITION_OR         (0 << 15)
#define KEYCNT_IRQ_CONDITION_AND        (1 << 15)

// Serial Communication (2)
// ------------------------

#if 0
  4000134h  2    R/W  RCNT      SIO Mode Select/General Purpose Data
  4000140h  2    R/W  JOYCNT    SIO JOY Bus Control
  4000150h  4    R/W  JOY_RECV  SIO JOY Bus Receive Data
  4000154h  4    R/W  JOY_TRANS SIO JOY Bus Transmit Data
  4000158h  2    R/?  JOYSTAT   SIO JOY Bus Receive Status
#endif

// Interrupt Control
// -----------------

// IE, IF

#define IRQF_VBLANK                     (1 << 0)
#define IRQF_HBLANK                     (1 << 1)
#define IRQF_VCOUNT                     (1 << 2)
#define IRQF_TIMER0                     (1 << 3)
#define IRQF_TIMER1                     (1 << 4)
#define IRQF_TIMER2                     (1 << 5)
#define IRQF_TIMER3                     (1 << 6)
#define IRQF_SERIAL                     (1 << 7)
#define IRQF_DMA0                       (1 << 8)
#define IRQF_DMA1                       (1 << 9)
#define IRQF_DMA2                       (1 << 10)
#define IRQF_DMA3                       (1 << 11)
#define IRQF_KEYPAD                     (1 << 12)
#define IRQF_GAMEPAK                    (1 << 13)

// IME

#define IME_DISABLE                     (0 << 0)
#define IME_ENABLE                      (1 << 0)

#endif // HARDWARE_H__
