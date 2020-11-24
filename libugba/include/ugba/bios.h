// SPDX-License-Identifier: MIT
//
// Copyright (c) 2020 Antonio Niño Díaz

#ifndef BIOS_H__
#define BIOS_H__

#include "definitions.h"
#include "hardware.h"

// Names taken from GBATEK

EXPORT_API void SWI_Halt(void);
EXPORT_API void SWI_VBlankIntrWait(void);

// For SWI_CpuSet() and SWI_CpuFastSet()
#define SWI_MODE_COPY   (0 << 24)
#define SWI_MODE_FILL   (1 << 24)
// For SWI_CpuSet() only
#define SWI_MODE_16BIT  (0 << 26)
#define SWI_MODE_32BIT  (1 << 26)

// Source and destination must be aligned to 4 bytes for 32-bit copies, or 2
// bytes for 16-bit copies. The length is expressed either in 32-bit or 16-bit
// chunks (words or halfwords).
EXPORT_API void SWI_CpuSet(const void *src, void *dst, uint32_t len_mode);

// Source and destination must be aligned to 4 bytes. The length must be a
// multiple of 8 bytes.
EXPORT_API void SWI_CpuFastSet(const void *src, void *dst, uint32_t len_mode);

#pragma pack(push, 1)
typedef struct {
    int32_t bgx;        // 24.8 fixed point
    int32_t bgy;        // 24.8 fixed point
    int16_t scrx;
    int16_t scry;
    int16_t scalex;     // 8.8 fixed point
    int16_t scaley;     // 8.8 fixed point
    uint16_t angle;     // 8.8 fixed point. Range: 0 - 0xFFFF
    uint16_t padding;
} bg_affine_src_t;
#pragma pack(pop)

#pragma pack(push, 1)
typedef struct {
    int16_t pa;
    int16_t pb;
    int16_t pc;
    int16_t pd;
    int32_t xoff;
    int32_t yoff;
} bg_affine_dst_t;
#pragma pack(pop)

EXPORT_API void SWI_BgAffineSet(const bg_affine_src_t *src,
                                bg_affine_dst_t *dst, uint32_t count);

#pragma pack(push, 1)
typedef struct {
    int16_t sx;         // 8.8 fixed point
    int16_t sy;         // 8.8 fixed point
    uint16_t angle;     // 8.8 fixed point. Range: 0 - 0xFFFF
    uint16_t padding;
} obj_affine_src_t;
#pragma pack(pop)

EXPORT_API
void SWI_ObjAffineSet(const obj_affine_src_t *src, void *dst,
                      uint32_t count, uint32_t increment);

EXPORT_API
void SWI_LZ77UnCompReadNormalWrite8bit(const void *source, void *dest);

EXPORT_API
void SWI_LZ77UnCompReadNormalWrite16bit(const void *source, void *dest);

#endif // BIOS_H__
