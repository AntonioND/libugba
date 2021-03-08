// SPDX-License-Identifier: MIT
//
// Copyright (c) 2020-2021 Antonio Niño Díaz

// Test that checks that different copy functions work, including DMA and BIOS.

#include <string.h>

#include <ugba/ugba.h>

// File generated with:
//     head -c 16K </dev/urandom >random.bin

#include "random_bin.h"

// 16 K buffer aligned to at least 32 bit (for DMA copies)
static uint64_t buffer[(16 * 1024) / sizeof(uint64_t)];

static void verify_copy(void)
{
    if (memcmp(&(buffer[0]), &(random_bin[0]), random_bin_size) == 0)
        CON_Print("passed\n");
    else
        CON_Print("failed\n");

    memset(&(buffer[0]), 0, random_bin_size);
}

int main(int argc, char *argv[])
{
    int ret;

    UGBA_Init(&argc, &argv);

    IRQ_Enable(IRQ_VBLANK);

    DISP_ModeSet(0);

    CON_InitDefault();

    CON_Print("memcpy(): ");
    memcpy(&(buffer[0]), &(random_bin[0]), random_bin_size);
    verify_copy();

    CON_Print("SWI_CpuSet_Copy16(): ");
    SWI_CpuSet_Copy16(&(random_bin[0]), &(buffer[0]), random_bin_size);
    memcpy(&(buffer[0]), &(random_bin[0]), random_bin_size);
    verify_copy();

    CON_Print("SWI_CpuSet_Copy32(): ");
    SWI_CpuSet_Copy32(&(random_bin[0]), &(buffer[0]), random_bin_size);
    memcpy(&(buffer[0]), &(random_bin[0]), random_bin_size);
    verify_copy();

    CON_Print("DMA_Copy16(3): ");
    ret = DMA_Copy16(3, &(random_bin[0]), &(buffer[0]), random_bin_size);
    if (ret != 0)
        CON_Print("[***] ");
    memcpy(&(buffer[0]), &(random_bin[0]), random_bin_size);
    verify_copy();

    CON_Print("DMA_Copy32(3): ");
    ret = DMA_Copy16(3, &(random_bin[0]), &(buffer[0]), random_bin_size);
    if (ret != 0)
        CON_Print("[***] ");
    memcpy(&(buffer[0]), &(random_bin[0]), random_bin_size);
    verify_copy();

    while (1)
        SWI_VBlankIntrWait();

    return 0;
}
