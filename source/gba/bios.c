// SPDX-License-Identifier: MIT
//
// Copyright (c) 2020-2021 Antonio Niño Díaz

#include <ugba/ugba.h>

// This directive helps with unit testing. Every build system deals differently
// with relative vs absolute paths. This makes asserts always print the same
// error message.
#line 11 "source/gba/bios.c"

// The functions here access the BIOS services directly. Some of the calls have
// awkward arguments, so the library has  wrappers for some of them.

#ifdef __thumb__
# define SWI_NUMBER(n) "swi "#n "\n"
#else
# define SWI_NUMBER(n) "swi "#n " << 16\n"
#endif

NORETURN void SWI_SoftReset(void)
{
    __asm__ volatile(
        SWI_NUMBER(0x00) :::
        "r0", "r1", "r2", "r3", "memory"
    );

    while (1); // Trap execution just in case
}

void SWI_RegisterRamReset(uint32_t flags)
{
    register uint32_t flags_ __asm__("r0") = flags;

    __asm__ volatile(
        SWI_NUMBER(0x01) ::
        "r"(flags_) :
        "r1", "r2", "r3", "memory"
    );
}

void SWI_Halt(void)
{
    __asm__ volatile(
        SWI_NUMBER(0x02) :::
        "r0", "r1", "r2", "r3", "memory"
    );
}

#if 0
SWI_Stop 0x03 // TODO
#endif

void SWI_IntrWait(uint32_t discard_old_flags, uint16_t wait_flags)
{
    UGBA_Assert(wait_flags != 0);

    register uint32_t discard_ __asm__("r0") = discard_old_flags;
    register uint32_t flags_ __asm__("r1") = wait_flags;

    __asm__ volatile(
        SWI_NUMBER(0x04) ::
        "r"(discard_), "r"(flags_) :
        "r2", "r3", "memory"
    );
}

void SWI_VBlankIntrWait(void)
{
    __asm__ volatile(
        SWI_NUMBER(0x05) :::
        "r0", "r1", "r2", "r3", "memory"
    );
}

int32_t SWI_Div(int32_t num, int32_t div)
{
    UGBA_Assert(div != 0);

    if (div == 0)
        return 0;

    register int32_t num_ __asm__("r0") = num;
    register int32_t div_ __asm__("r1") = div;
    register int32_t result __asm__("r0");

    __asm__ volatile(
        SWI_NUMBER(0x06) :
        "=r"(result) :
        "r"(num_), "r"(div_) :
        "r2", "r3", "memory"
    );

    return result;
}

int32_t SWI_DivMod(int32_t num, int32_t div)
{
    UGBA_Assert(div != 0);

    if (div == 0)
        return 0;

    register int32_t num_ __asm__("r0") = num;
    register int32_t div_ __asm__("r1") = div;
    register int32_t result __asm__("r1");

    __asm__ volatile(
        SWI_NUMBER(0x06) :
        "=r"(result) :
        "r"(num_), "r"(div_) :
        "r2", "r3", "memory"
    );

    return result;
}

uint16_t SWI_Sqrt(uint32_t value)
{
    register uint32_t value_ __asm__("r0") = value;
    register uint32_t result __asm__("r0");

    __asm__ volatile(
        SWI_NUMBER(0x08) :
        "=r"(result) :
        "r"(value_) :
        "r1", "r2", "r3", "memory"
    );

    return (uint16_t)result;
}

int16_t SWI_ArcTan(int16_t tan)
{
    register uint32_t tan_ __asm__("r0") = tan;
    register uint32_t result __asm__("r0");

    __asm__ volatile(
        SWI_NUMBER(0x09) :
        "=r"(result) :
        "r"(tan_) :
        "r1", "r2", "r3", "memory"
    );

    return (int16_t)result;
}

int16_t SWI_ArcTan2(int16_t x, int16_t y)
{
    register uint32_t x_ __asm__("r0") = x;
    register uint32_t y_ __asm__("r1") = y;
    register uint32_t result __asm__("r0");

    __asm__ volatile(
        SWI_NUMBER(0x0A) :
        "=r"(result) :
        "r"(x_), "r"(y_) :
        "r2", "r3", "memory"
    );

    return (int16_t)result;
}

void SWI_CpuSet(const void *src, void *dst, uint32_t len_mode)
{
    if (len_mode & SWI_MODE_32BIT)
    {
        UGBA_Assert(((uint32_t)src & 3) == 0);
        UGBA_Assert(((uint32_t)dst & 3) == 0);
    }
    else
    {
        UGBA_Assert(((uint32_t)src & 1) == 0);
        UGBA_Assert(((uint32_t)dst & 1) == 0);
    }

    register uint32_t src_ __asm__("r0") = (uint32_t)src;
    register uint32_t dst_ __asm__("r1") = (uint32_t)dst;
    register uint32_t len_mode_ __asm__("r2") = len_mode;

    __asm__ volatile(
        SWI_NUMBER(0x0B) ::
        "r"(src_), "r"(dst_), "r"(len_mode_) :
        "r3", "memory"
    );
}

void SWI_CpuFastSet(const void *src, void *dst, uint32_t len_mode)
{
    UGBA_Assert(((uint32_t)src & 3) == 0);
    UGBA_Assert(((uint32_t)dst & 3) == 0);
    UGBA_Assert((len_mode & 7) == 0);

    register uint32_t src_ __asm__("r0") = (uint32_t)src;
    register uint32_t dst_ __asm__("r1") = (uint32_t)dst;
    register uint32_t len_mode_ __asm__("r2") = len_mode;

    __asm__ volatile(
        SWI_NUMBER(0x0C) ::
        "r"(src_), "r"(dst_), "r"(len_mode_) :
        "r3", "memory"
    );
}

uint32_t SWI_GetBiosChecksum(void)
{
    register uint32_t sum __asm__("r0");

    __asm__ volatile(
        SWI_NUMBER(0x0D) :
        "=r"(sum) ::
        "r1", "r2", "r3", "memory"
    );

    return sum;
}

void SWI_BgAffineSet(const bg_affine_src *src, bg_affine_dst *dst,
                     uint32_t count)
{
    UGBA_Assert(((uint32_t)src & 3) == 0);
    UGBA_Assert(((uint32_t)dst & 3) == 0);

    register uint32_t src_ __asm__("r0") = (uint32_t)src;
    register uint32_t dst_ __asm__("r1") = (uint32_t)dst;
    register uint32_t count_ __asm__("r2") = count;

    __asm__ volatile(
        SWI_NUMBER(0x0E) ::
        "r"(src_), "r"(dst_), "r"(count_) :
        "r3", "memory"
    );
}

void SWI_ObjAffineSet(const obj_affine_src *src, void *dst,
                      uint32_t count, uint32_t increment)
{
    UGBA_Assert(((uint32_t)src & 1) == 0);
    UGBA_Assert(((uint32_t)dst & 1) == 0);
    UGBA_Assert((increment & 1) == 0);

    register uint32_t src_ __asm__("r0") = (uint32_t)src;
    register uint32_t dst_ __asm__("r1") = (uint32_t)dst;
    register uint32_t count_ __asm__("r2") = count;
    register uint32_t increment_ __asm__("r3") = increment;

    __asm__ volatile(
        SWI_NUMBER(0x0F) ::
        "r"(src_), "r"(dst_), "r"(count_), "r"(increment_) :
        "memory"
    );
}

void SWI_BitUnPack(const void *source, void *dest, const bit_unpack_info *info)
{
    UGBA_Assert(((uint32_t)dest & 3) == 0);

    register uint32_t source_ __asm__("r0") = (uint32_t)source;
    register uint32_t dest_ __asm__("r1") = (uint32_t)dest;
    register uint32_t info_ __asm__("r2") = (uint32_t)info;

    __asm__ volatile(
        SWI_NUMBER(0x10) ::
        "r"(source_), "r"(dest_), "r"(info_) :
        "r3", "memory"
    );
}

void SWI_LZ77UnCompReadNormalWrite8bit(const void *source, void *dest)
{
    UGBA_Assert(((uint32_t)source & 3) == 0);

    register uint32_t source_ __asm__("r0") = (uint32_t)source;
    register uint32_t dest_ __asm__("r1") = (uint32_t)dest;

    __asm__ volatile(
        SWI_NUMBER(0x11) ::
        "r"(source_), "r"(dest_) :
        "r2", "r3", "memory"
    );
}

void SWI_LZ77UnCompReadNormalWrite16bit(const void *source, void *dest)
{
    UGBA_Assert(((uint32_t)source & 3) == 0);
    UGBA_Assert(((uint32_t)dest & 1) == 0);

    register uint32_t source_ __asm__("r0") = (uint32_t)source;
    register uint32_t dest_ __asm__("r1") = (uint32_t)dest;

    __asm__ volatile(
        SWI_NUMBER(0x12) ::
        "r"(source_), "r"(dest_) :
        "r2", "r3", "memory"
    );
}

void SWI_HuffUnComp(const void *source, void *dest)
{
    UGBA_Assert(((uint32_t)source & 3) == 0);
    UGBA_Assert(((uint32_t)dest & 3) == 0);

    register uint32_t source_ __asm__("r0") = (uint32_t)source;
    register uint32_t dest_ __asm__("r1") = (uint32_t)dest;

    __asm__ volatile(
        SWI_NUMBER(0x13) ::
        "r"(source_), "r"(dest_) :
        "r2", "r3", "memory"
    );
}

void SWI_RLUnCompWram(const void *source, void *dest)
{
    UGBA_Assert(((uint32_t)source & 3) == 0);

    register uint32_t source_ __asm__("r0") = (uint32_t)source;
    register uint32_t dest_ __asm__("r1") = (uint32_t)dest;

    __asm__ volatile(
        SWI_NUMBER(0x14) ::
        "r"(source_), "r"(dest_) :
        "r2", "r3", "memory"
    );
}

void SWI_RLUnCompVram(const void *source, void *dest)
{
    UGBA_Assert(((uint32_t)source & 3) == 0);
    UGBA_Assert(((uint32_t)dest & 1) == 0);

    register uint32_t source_ __asm__("r0") = (uint32_t)source;
    register uint32_t dest_ __asm__("r1") = (uint32_t)dest;

    __asm__ volatile(
        SWI_NUMBER(0x15) ::
        "r"(source_), "r"(dest_) :
        "r2", "r3", "memory"
    );
}

void SWI_Diff8bitUnFilterWram(const void *source, void *dest)
{
    UGBA_Assert(((uint32_t)source & 3) == 0);

    register uint32_t source_ __asm__("r0") = (uint32_t)source;
    register uint32_t dest_ __asm__("r1") = (uint32_t)dest;

    __asm__ volatile(
        SWI_NUMBER(0x16) ::
        "r"(source_), "r"(dest_) :
        "r2", "r3", "memory"
    );
}

void SWI_Diff8bitUnFilterVram(const void *source, void *dest)
{
    UGBA_Assert(((uint32_t)source & 3) == 0);
    UGBA_Assert(((uint32_t)dest & 1) == 0);

    register uint32_t source_ __asm__("r0") = (uint32_t)source;
    register uint32_t dest_ __asm__("r1") = (uint32_t)dest;

    __asm__ volatile(
        SWI_NUMBER(0x17) ::
        "r"(source_), "r"(dest_) :
        "r2", "r3", "memory"
    );
}

void SWI_Diff16bitUnFilter(const void *source, void *dest)
{
    UGBA_Assert(((uint32_t)source & 3) == 0);
    UGBA_Assert(((uint32_t)dest & 1) == 0);

    register uint32_t source_ __asm__("r0") = (uint32_t)source;
    register uint32_t dest_ __asm__("r1") = (uint32_t)dest;

    __asm__ volatile(
        SWI_NUMBER(0x18) ::
        "r"(source_), "r"(dest_) :
        "r2", "r3", "memory"
    );
}

void SWI_SoundBias(uint32_t level)
{
    register uint32_t level_ __asm__("r0") = level;

    __asm__ volatile(
        SWI_NUMBER(0x19) ::
        "r"(level_) :
        "r1", "r2", "r3", "memory"
    );
}

NORETURN void SWI_HardReset(void)
{
    __asm__ volatile(
        SWI_NUMBER(0x26) :::
        "r0", "r1", "r2", "r3", "memory"
    );

    while (1); // Trap execution just in case
}
