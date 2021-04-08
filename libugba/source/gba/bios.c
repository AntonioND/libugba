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
    asm volatile(
        SWI_NUMBER(0x00) :::
        "r0", "r1", "r2", "r3", "memory"
    );

    while (1); // Trap execution just in case
}

void SWI_RegisterRamReset(uint32_t flags)
{
    register uint32_t flags_ asm("r0") = flags;

    asm volatile(
        SWI_NUMBER(0x01) ::
        "r"(flags_) :
        "r1", "r2", "r3", "memory"
    );
}

void SWI_Halt(void)
{
    asm volatile(
        SWI_NUMBER(0x02) :::
        "r0", "r1", "r2", "r3", "memory"
    );
}

#if 0
SWI_Stop 0x03 // TODO
#endif

void SWI_IntrWait(uint32_t discard_old_flags, uint16_t wait_flags)
{
    UMOD_Assert(wait_flags != 0);

    register uint32_t discard_ asm("r0") = discard_old_flags;
    register uint32_t flags_ asm("r1") = wait_flags;

    asm volatile(
        SWI_NUMBER(0x04) ::
        "r"(discard_), "r"(flags_) :
        "r2", "r3", "memory"
    );
}

void SWI_VBlankIntrWait(void)
{
    asm volatile(
        SWI_NUMBER(0x05) :::
        "r0", "r1", "r2", "r3", "memory"
    );
}

int32_t SWI_Div(int32_t num, int32_t div)
{
    UMOD_Assert(div != 0);

    if (div == 0)
        return 0;

    register int32_t num_ asm("r0") = num;
    register int32_t div_ asm("r1") = div;
    register int32_t result asm("r0");

    asm volatile(
        SWI_NUMBER(0x06) :
        "=r"(result) :
        "r"(num_), "r"(div_) :
        "r2", "r3", "memory"
    );

    return result;
}

int32_t SWI_DivMod(int32_t num, int32_t div)
{
    UMOD_Assert(div != 0);

    if (div == 0)
        return 0;

    register int32_t num_ asm("r0") = num;
    register int32_t div_ asm("r1") = div;
    register int32_t result asm("r1");

    asm volatile(
        SWI_NUMBER(0x06) :
        "=r"(result) :
        "r"(num_), "r"(div_) :
        "r2", "r3", "memory"
    );

    return result;
}

uint16_t SWI_Sqrt(uint32_t value)
{
    register uint32_t value_ asm("r0") = value;
    register uint32_t result asm("r0");

    asm volatile(
        SWI_NUMBER(0x08) :
        "=r"(result) :
        "r"(value_) :
        "r1", "r2", "r3", "memory"
    );

    return (uint16_t)result;
}

int16_t SWI_ArcTan(int16_t tan)
{
    register uint32_t tan_ asm("r0") = tan;
    register uint32_t result asm("r0");

    asm volatile(
        SWI_NUMBER(0x09) :
        "=r"(result) :
        "r"(tan_) :
        "r1", "r2", "r3", "memory"
    );

    return (int16_t)result;
}

int16_t SWI_ArcTan2(int16_t x, int16_t y)
{
    register uint32_t x_ asm("r0") = x;
    register uint32_t y_ asm("r1") = y;
    register uint32_t result asm("r0");

    asm volatile(
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
        UMOD_Assert(((uint32_t)src & 3) == 0);
        UMOD_Assert(((uint32_t)dst & 3) == 0);
    }
    else
    {
        UMOD_Assert(((uint32_t)src & 1) == 0);
        UMOD_Assert(((uint32_t)dst & 1) == 0);
    }

    register uint32_t src_ asm("r0") = (uint32_t)src;
    register uint32_t dst_ asm("r1") = (uint32_t)dst;
    register uint32_t len_mode_ asm("r2") = len_mode;

    asm volatile(
        SWI_NUMBER(0x0B) ::
        "r"(src_), "r"(dst_), "r"(len_mode_) :
        "r3", "memory"
    );
}

void SWI_CpuFastSet(const void *src, void *dst, uint32_t len_mode)
{
    UMOD_Assert(((uint32_t)src & 3) == 0);
    UMOD_Assert(((uint32_t)dst & 3) == 0);
    UMOD_Assert((len_mode & 7) == 0);

    register uint32_t src_ asm("r0") = (uint32_t)src;
    register uint32_t dst_ asm("r1") = (uint32_t)dst;
    register uint32_t len_mode_ asm("r2") = len_mode;

    asm volatile(
        SWI_NUMBER(0x0C) ::
        "r"(src_), "r"(dst_), "r"(len_mode_) :
        "r3", "memory"
    );
}

uint32_t SWI_GetBiosChecksum(void)
{
    register uint32_t sum asm("r0");

    asm volatile(
        SWI_NUMBER(0x0D) :
        "=r"(sum) ::
        "r1", "r2", "r3", "memory"
    );

    return sum;
}

void SWI_BgAffineSet(const bg_affine_src *src, bg_affine_dst *dst,
                     uint32_t count)
{
    UMOD_Assert(((uint32_t)src & 3) == 0);
    UMOD_Assert(((uint32_t)dst & 3) == 0);

    register uint32_t src_ asm("r0") = (uint32_t)src;
    register uint32_t dst_ asm("r1") = (uint32_t)dst;
    register uint32_t count_ asm("r2") = count;

    asm volatile(
        SWI_NUMBER(0x0E) ::
        "r"(src_), "r"(dst_), "r"(count_) :
        "r3", "memory"
    );
}

void SWI_ObjAffineSet(const obj_affine_src *src, void *dst,
                      uint32_t count, uint32_t increment)
{
    UMOD_Assert(((uint32_t)src & 1) == 0);
    UMOD_Assert(((uint32_t)dst & 1) == 0);
    UMOD_Assert((increment & 1) == 0);

    register uint32_t src_ asm("r0") = (uint32_t)src;
    register uint32_t dst_ asm("r1") = (uint32_t)dst;
    register uint32_t count_ asm("r2") = count;
    register uint32_t increment_ asm("r3") = increment;

    asm volatile(
        SWI_NUMBER(0x0F) ::
        "r"(src_), "r"(dst_), "r"(count_), "r"(increment_) :
        "memory"
    );
}

void SWI_BitUnPack(const void *source, void *dest, const bit_unpack_info *info)
{
    UMOD_Assert(((uint32_t)dest & 3) == 0);

    register uint32_t source_ asm("r0") = (uint32_t)source;
    register uint32_t dest_ asm("r1") = (uint32_t)dest;
    register uint32_t info_ asm("r2") = (uint32_t)info;

    asm volatile(
        SWI_NUMBER(0x10) ::
        "r"(source_), "r"(dest_), "r"(info_) :
        "r3", "memory"
    );
}

void SWI_LZ77UnCompReadNormalWrite8bit(const void *source, void *dest)
{
    UMOD_Assert(((uint32_t)source & 3) == 0);

    register uint32_t source_ asm("r0") = (uint32_t)source;
    register uint32_t dest_ asm("r1") = (uint32_t)dest;

    asm volatile(
        SWI_NUMBER(0x11) ::
        "r"(source_), "r"(dest_) :
        "r2", "r3", "memory"
    );
}

void SWI_LZ77UnCompReadNormalWrite16bit(const void *source, void *dest)
{
    UMOD_Assert(((uint32_t)source & 3) == 0);
    UMOD_Assert(((uint32_t)dest & 1) == 0);

    register uint32_t source_ asm("r0") = (uint32_t)source;
    register uint32_t dest_ asm("r1") = (uint32_t)dest;

    asm volatile(
        SWI_NUMBER(0x12) ::
        "r"(source_), "r"(dest_) :
        "r2", "r3", "memory"
    );
}

void SWI_HuffUnComp(const void *source, void *dest)
{
    UMOD_Assert(((uint32_t)source & 3) == 0);
    UMOD_Assert(((uint32_t)dest & 3) == 0);

    register uint32_t source_ asm("r0") = (uint32_t)source;
    register uint32_t dest_ asm("r1") = (uint32_t)dest;

    asm volatile(
        SWI_NUMBER(0x13) ::
        "r"(source_), "r"(dest_) :
        "r2", "r3", "memory"
    );
}

void SWI_RLUnCompWram(const void *source, void *dest)
{
    UMOD_Assert(((uint32_t)source & 3) == 0);

    register uint32_t source_ asm("r0") = (uint32_t)source;
    register uint32_t dest_ asm("r1") = (uint32_t)dest;

    asm volatile(
        SWI_NUMBER(0x14) ::
        "r"(source_), "r"(dest_) :
        "r2", "r3", "memory"
    );
}

void SWI_RLUnCompVram(const void *source, void *dest)
{
    UMOD_Assert(((uint32_t)source & 3) == 0);
    UMOD_Assert(((uint32_t)dest & 1) == 0);

    register uint32_t source_ asm("r0") = (uint32_t)source;
    register uint32_t dest_ asm("r1") = (uint32_t)dest;

    asm volatile(
        SWI_NUMBER(0x15) ::
        "r"(source_), "r"(dest_) :
        "r2", "r3", "memory"
    );
}

void SWI_Diff8bitUnFilterWram(const void *source, void *dest)
{
    UMOD_Assert(((uint32_t)source & 3) == 0);

    register uint32_t source_ asm("r0") = (uint32_t)source;
    register uint32_t dest_ asm("r1") = (uint32_t)dest;

    asm volatile(
        SWI_NUMBER(0x16) ::
        "r"(source_), "r"(dest_) :
        "r2", "r3", "memory"
    );
}

void SWI_Diff8bitUnFilterVram(const void *source, void *dest)
{
    UMOD_Assert(((uint32_t)source & 3) == 0);
    UMOD_Assert(((uint32_t)dest & 1) == 0);

    register uint32_t source_ asm("r0") = (uint32_t)source;
    register uint32_t dest_ asm("r1") = (uint32_t)dest;

    asm volatile(
        SWI_NUMBER(0x17) ::
        "r"(source_), "r"(dest_) :
        "r2", "r3", "memory"
    );
}

void SWI_Diff16bitUnFilter(const void *source, void *dest)
{
    UMOD_Assert(((uint32_t)source & 3) == 0);
    UMOD_Assert(((uint32_t)dest & 1) == 0);

    register uint32_t source_ asm("r0") = (uint32_t)source;
    register uint32_t dest_ asm("r1") = (uint32_t)dest;

    asm volatile(
        SWI_NUMBER(0x18) ::
        "r"(source_), "r"(dest_) :
        "r2", "r3", "memory"
    );
}

void SWI_SoundBias(uint32_t level)
{
    register uint32_t level_ asm("r0") = level;

    asm volatile(
        SWI_NUMBER(0x19) ::
        "r"(level_) :
        "r1", "r2", "r3", "memory"
    );
}

NORETURN void SWI_HardReset(void)
{
    asm volatile(
        SWI_NUMBER(0x26) :::
        "r0", "r1", "r2", "r3", "memory"
    );

    while (1); // Trap execution just in case
}
