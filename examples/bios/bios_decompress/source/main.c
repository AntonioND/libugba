// SPDX-License-Identifier: MIT
//
// Copyright (c) 2020 Antonio Niño Díaz

// Test that checks that different BIOS decompression functions work as expected

#include <string.h>

#include <ugba/ugba.h>

// Files generated with GBACrusher

#include "main_c_bin.h"
#include "main_c_diff_16_bin.h"
#include "main_c_diff_8_bin.h"

// 16 K buffer aligned to 32 bit
static uint32_t buffer[(16 * 1024) / sizeof(uint32_t)];

static void verify_extract(void)
{
    if (memcmp(&(buffer[0]), &(main_c_bin[0]), main_c_bin_size) == 0)
        CON_Print("OK\n");
    else
        CON_Print("fail\n");
}

int main(int argc, char *argv[])
{
    UGBA_Init(&argc, &argv);

    IRQ_Enable(IRQ_VBLANK);

    REG_DISPCNT = DISPCNT_BG_MODE(0);

    CON_InitDefault();

    CON_Print("Diff8bitUnFilterWram(): ");
    memset(&(buffer[0]), 0, sizeof(buffer));
    SWI_Diff8bitUnFilterWram(&(main_c_diff_8_bin[0]), &(buffer[0]));
    verify_extract();

    CON_Print("Diff8bitUnFilterVram(): ");
    memset(&(buffer[0]), 0, sizeof(buffer));
    SWI_Diff8bitUnFilterVram(&(main_c_diff_8_bin[0]), &(buffer[0]));
    verify_extract();

    CON_Print("Diff16bitUnFilter(): ");
    memset(&(buffer[0]), 0, sizeof(buffer));
    SWI_Diff16bitUnFilter(&(main_c_diff_16_bin[0]), &(buffer[0]));
    verify_extract();

    // TODO: Other formats

    while (1)
        SWI_VBlankIntrWait();

    return 0;
}
