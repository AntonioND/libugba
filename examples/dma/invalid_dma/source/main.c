// SPDX-License-Identifier: MIT
//
// Copyright (c) 2020-2021 Antonio Niño Díaz

// Test that checks that invalid copies from ROM trigger an error message.

#include <string.h>

#include <ugba/ugba.h>

#ifdef __GBA__

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

#endif // __GBA__

int main(int argc, char *argv[])
{
    int ret;

    UGBA_Init(&argc, &argv);

    IRQ_Enable(IRQ_VBLANK);

    DISP_ModeSet(0);

    CON_InitDefault();

#ifdef __GBA__

    // The following tests should fail as only DMA3 can read the ROM. However,
    // the tests will pass on PC because it has no notion of ROM/EWRAM/IWRAM.

    CON_Print("DMA_Copy32(0): ");
    ret = DMA_Copy16(0, &(random_bin[0]), &(buffer[0]), random_bin_size);
    if (ret == 0)
        CON_Print("[***] ");
    memcpy(&(buffer[0]), &(random_bin[0]), random_bin_size);
    verify_copy();

    CON_Print("DMA_Copy32(1): ");
    ret = DMA_Copy16(1, &(random_bin[0]), &(buffer[0]), random_bin_size);
    if (ret == 0)
        CON_Print("[***] ");
    memcpy(&(buffer[0]), &(random_bin[0]), random_bin_size);
    verify_copy();

    CON_Print("DMA_Copy32(2): ");
    ret = DMA_Copy16(2, &(random_bin[0]), &(buffer[0]), random_bin_size);
    if (ret == 0)
        CON_Print("[***] ");
    memcpy(&(buffer[0]), &(random_bin[0]), random_bin_size);
    verify_copy();

#else

    CON_Print("Test skipped");

#endif // __GBA__

    while (1)
        SWI_VBlankIntrWait();

    return 0;
}
