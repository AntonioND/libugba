// SPDX-License-Identifier: MIT
//
// Copyright (c) 2020 Antonio Niño Díaz

#include <gbaline.h>

#ifdef __thumb__
# define SWI_NUMBER(n) "swi "#n "\n"
#else
# define SWI_NUMBER(n) "swi "#n " << 16\n"
#endif

void SWI_Halt(void)
{
    asm volatile(SWI_NUMBER(0x02) :::
                 "r0", "r1", "r2", "r3", "memory");
}

#if 0
void SWI_IntrWait(uint32_t discard_old_flags, uint32_t wait_flags)
{
    asm volatile(SWI_NUMBER(0x04) :::
                 "r0", "r1", "r2", "r3", "memory");
}
#endif

void SWI_VBlankIntrWait(void)
{
    asm volatile(SWI_NUMBER(0x05) :::
                 "r0", "r1", "r2", "r3", "memory");
}

#if 0
SWI_SoftReset 0x00
SWI_RegisterRamReset 0x01
SWI_Stop 0x03
SWI_Div
SWI_DivArm
SWI_Sqrt
SWI_ArcTan
SWI_ArcTan2
SWI_CpuSet
SWI_CpuFastSet
SWI_GetBiosChecksum
SWI_BgAffineSet
#endif

void SWI_ObjAffineSet(obj_affine_src_t *src, void *dst,
                      uint32_t count, uint32_t increment)
{
    asm volatile("mov r0, %0\n"
                 "mov r1, %1\n"
                 "mov r2, %2\n"
                 "mov r3, %3\n"
                 SWI_NUMBER(0x0F) ::
                 "r" (src), "r" (dst), "r" (count), "r" (increment) :
                 "r0", "r1", "r2", "r3", "memory");
}

#if 0
SWI_BitUnPack
SWI_LZ77UnCompWram8 // 8 bit version
SWI_LZ77UnCompVram16 // 16 bit version
SWI_HuffUnComp
SWI_RLUnCompWram8 // 8 bit version
SWI_RLUnCompVram16 // 16 bit version
SWI_Diff8bitUnFilterWram
SWI_Diff8bitUnFilterVram
SWI_Diff16bitUnFilter
SWI_MidiKey2Freq
SWI_MultiBoot
SWI_HardReset
SWI_CustomHalt
#endif
