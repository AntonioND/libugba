// SPDX-License-Identifier: MIT
//
// Copyright (c) 2020 Antonio Niño Díaz

#ifdef __thumb__
# define SWI(n) asm volatile("swi "#n "\n" :::  "r0", "r1", "r2", "r3")
#else
# define SWI(n) asm volatile("swi "#n " << 16\n" :::  "r0", "r1", "r2", "r3")
#endif

void SWI_Halt(void)
{
    SWI(0x02);
}

#if 0
void SWI_IntrWait(uint32_t discard_old_flags, uint32_t wait_flags)
{
    SWI(0x04);
}
#endif

void SWI_VBlankIntrWait(void)
{
    SWI(0x05);
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
SWI_ObjAffineSet
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
