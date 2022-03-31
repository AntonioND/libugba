// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022 Antonio Niño Díaz

#include <ugba/ugba.h>

// This directive helps with unit testing. Every build system deals differently
// with relative vs absolute paths. This makes asserts always print the same
// error message.
#line 11 "source/sram.c"

EWRAM_CODE void SRAM_Write(void *dst, const void *src, size_t size)
{
    uintptr_t dst_addr_start = (uintptr_t)dst;
    uintptr_t dst_addr_end = (uintptr_t)dst + size;

    UGBA_Assert(dst_addr_start >= MEM_SRAM_ADDR);
    UGBA_Assert(dst_addr_end <= (MEM_SRAM_ADDR + MEM_SRAM_SIZE));

    UGBA_Assert(size <= MEM_SRAM_SIZE);

    const volatile uint8_t *source = src;
    volatile uint8_t *destination = dst;

    for (size_t i = 0; i < size; i++)
        destination[i] = source[i];
}

EWRAM_CODE void SRAM_Read(void *dst, const void *src, size_t size)
{
    uintptr_t src_addr_start = (uintptr_t)src;
    uintptr_t src_addr_end = (uintptr_t)src + size;

    UGBA_Assert(src_addr_start >= MEM_SRAM_ADDR);
    UGBA_Assert(src_addr_end <= (MEM_SRAM_ADDR + MEM_SRAM_SIZE));

    UGBA_Assert(size <= MEM_SRAM_SIZE);

    const volatile uint8_t *source = src;
    volatile uint8_t *destination = dst;

    for (size_t i = 0; i < size; i++)
        destination[i] = source[i];
}
