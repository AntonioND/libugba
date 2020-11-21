// SPDX-License-Identifier: MIT
//
// Copyright (c) 2020 Antonio Niño Díaz

#include <gbaline.h>

void SWI_CpuSet_Copy16(const void *src, void *dst, size_t len)
{
    SWI_CpuSet(src, dst, SWI_MODE_COPY | SWI_MODE_16BIT | (len >> 1));
}

void SWI_CpuSet_Fill16(const void *src, void *dst, size_t len)
{
    SWI_CpuSet(src, dst, SWI_MODE_FILL | SWI_MODE_16BIT | (len >> 1));
}

void SWI_CpuSet_Copy32(const void *src, void *dst, size_t len)
{
    SWI_CpuSet(src, dst, SWI_MODE_COPY | SWI_MODE_32BIT | (len >> 2));
}

void SWI_CpuSet_Fill32(const void *src, void *dst, size_t len)
{
    SWI_CpuSet(src, dst, SWI_MODE_FILL | SWI_MODE_32BIT | (len >> 2));
}

void SWI_CpuFastSet_Copy32(const void *src, void *dst, size_t len)
{
    SWI_CpuFastSet(src, dst, SWI_MODE_COPY | (len >> 2));
}

void SWI_CpuFastSet_Fill32(const void *src, void *dst, size_t len)
{
    SWI_CpuFastSet(src, dst, SWI_MODE_FILL | (len >> 2));
}

void SWI_ObjAffineSet_OAM(const obj_affine_src_t *src, oam_matrix_entry_t *dst,
                          uint32_t count)
{
    // Start writing at the right offset in the OAM entry
    uintptr_t real_dst = (uintptr_t)dst + 3 * sizeof(uint16_t);

    SWI_ObjAffineSet(src, (void *)real_dst, count, 8);
}

