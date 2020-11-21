// SPDX-License-Identifier: MIT
//
// Copyright (c) 2020 Antonio Niño Díaz

#ifndef BIOS_WRAPPERS_H__
#define BIOS_WRAPPERS_H__

#include <stddef.h>

#include "bios.h"

EXPORT_API void SWI_CpuSet_Copy16(const void *src, void *dst, size_t len);
EXPORT_API void SWI_CpuSet_Fill16(const void *src, void *dst, size_t len);
EXPORT_API void SWI_CpuSet_Copy32(const void *src, void *dst, size_t len);
EXPORT_API void SWI_CpuSet_Fill32(const void *src, void *dst, size_t len);

EXPORT_API void SWI_CpuFastSet_Copy32(const void *src, void *dst, size_t len);
EXPORT_API void SWI_CpuFastSet_Fill32(const void *src, void *dst, size_t len);

EXPORT_API void SWI_ObjAffineSet_OAM(const obj_affine_src_t *src,
                                     oam_matrix_entry_t *dst, uint32_t count);

#endif // BIOS_WRAPPERS_H__
