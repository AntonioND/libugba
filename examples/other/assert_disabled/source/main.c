// SPDX-License-Identifier: MIT
//
// Copyright (c) 2021 Antonio Niño Díaz

// Test custom assert macro.

#include <ugba/ugba.h>

int main(int argc, char *argv[])
{
    UGBA_Init(&argc, &argv);

    IRQ_Init();
    IRQ_Enable(IRQ_VBLANK);

    DISP_ModeSet(0);

    CON_InitDefault();

    CON_Print("This goes before the asserts.\n");

    UMOD_Assert(sizeof(uint32_t) == 4);

    // The following is needed so that the filename is consistent in every
    // environment (__FILE__ uses the absolute path if the build system passes
    // an absolute path to the compiler). Don't do this in your own code.
#line 25 "main.c"
    UMOD_Assert(sizeof(uint32_t) == 10);

    CON_Print("This goes after the asserts.\n");

    while (1)
        SWI_VBlankIntrWait();
}

