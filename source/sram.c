// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022 Antonio Niño Díaz

#include <ugba/ugba.h>

EWRAM_CODE int SRAM_Write(void *dst, const void *src, size_t size)
{
    uintptr_t dst_addr_start = (uintptr_t)dst;
    uintptr_t dst_addr_end = (uintptr_t)dst + size;

    if (size > MEM_SRAM_SIZE)
        return -1;

    if (dst_addr_start < MEM_SRAM_ADDR)
        return -2;
    if (dst_addr_end > (MEM_SRAM_ADDR + MEM_SRAM_SIZE))
        return -3;

    const volatile uint8_t *source = src;
    volatile uint8_t *destination = dst;

    for (size_t i = 0; i < size; i++)
        destination[i] = source[i];

    return 0;
}

EWRAM_CODE int SRAM_Read(void *dst, const void *src, size_t size)
{
    uintptr_t src_addr_start = (uintptr_t)src;
    uintptr_t src_addr_end = (uintptr_t)src + size;

    if (size > MEM_SRAM_SIZE)
        return -1;

    if (src_addr_start < MEM_SRAM_ADDR)
        return -2;
    if (src_addr_end > (MEM_SRAM_ADDR + MEM_SRAM_SIZE))
        return -3;

    const volatile uint8_t *source = src;
    volatile uint8_t *destination = dst;

    for (size_t i = 0; i < size; i++)
        destination[i] = source[i];

    return 0;
}
