// SPDX-License-Identifier: MIT
//
// Copyright (c) 2020 Antonio Niño Díaz

// Example that shows how to setup timers in cascade mode

#include <stdio.h>

#include <ugba/ugba.h>

int main(int argc, char *argv[])
{
    UGBA_Init(&argc, &argv);

    IRQ_Enable(IRQ_VBLANK);

    REG_DISPCNT = DISPCNT_BG_MODE(0);

    CON_InitDefault();

    // Normally this would go at the beginning of the program, before
    // UGBA_Init()

    if (UGBA_VersionIsCompatible(LIBUGBA_VERSION))
        CON_Print("Success\n");
    else
        CON_Print("Failed\n");

    // Different patch version: Should succeed
    if (UGBA_VersionIsCompatible(LIBUGBA_VERSION + (1 << 0)))
        CON_Print("Success\n");
    else
        CON_Print("Failed\n");

    // Different minor version (lower than the library): Should succeed
    if (UGBA_VersionIsCompatible(LIBUGBA_VERSION - (1 << 8)))
        CON_Print("Success\n");
    else
        CON_Print("Failed\n");

    // Different minor version (higher than the library): Should fail
    if (UGBA_VersionIsCompatible(LIBUGBA_VERSION + (1 << 8)))
        CON_Print("Failed\n");
    else
        CON_Print("Success\n");

    // Different major version: Should fail
    if (UGBA_VersionIsCompatible(LIBUGBA_VERSION + (1 << 16)))
        CON_Print("Failed\n");
    else
        CON_Print("Success\n");

    while (1)
        SWI_VBlankIntrWait();
}

