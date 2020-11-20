// SPDX-License-Identifier: MIT
//
// Copyright (c) 2020 Antonio Niño Díaz

#include <gbaline.h>

void SWI_ObjAffineSet_OAM(obj_affine_src_t *src, oam_matrix_entry_t *dst,
                          uint32_t count)
{
    // Start writing at the right offset in the OAM entry
    uintptr_t real_dst = (uintptr_t)dst + 3 * sizeof(uint16_t);

    SWI_ObjAffineSet(src, (void *)real_dst, count, 8);
}

