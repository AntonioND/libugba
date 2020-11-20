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

#pragma pack(push, 1)
typedef struct {
    int16_t sx;     // 8.8 fixed point
    int16_t sy;     // 8.8 fixed point
    uint16_t angle; // 8.8 fixed point. Range: 0 - 0xFFFF
    uint16_t padding;
} obj_affine_src_t;
#pragma pack(pop)

EXPORT_API
void SWI_ObjAffineSet(obj_affine_src_t *src, void *dst,
                      uint32_t count, uint32_t increment);
EXPORT_API
void SWI_ObjAffineSet_OAM(obj_affine_src_t *src, oam_matrix_entry_t *dst,
                          uint32_t count);

#endif // BIOS_H__
