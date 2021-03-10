// SPDX-License-Identifier: MIT
//
// Copyright (c) 2021 Antonio Niño Díaz

// Test that checks the value of WAITCNT after initializing the library.

#include <inttypes.h>
#include <stdio.h>

#include <ugba/ugba.h>

int main(int argc, char *argv[])
{
    UGBA_Init(&argc, &argv);

    REG_WAITCNT = UGBA_FlashcartOptimizedWaitstates();

    // Display results

    IRQ_Enable(IRQ_VBLANK);

    DISP_ModeSet(0);

    CON_InitDefault();

    char string[50];
    snprintf(string, sizeof(string),
             "WAITCNT: 0x%" PRIX16 "\n"
             "SuperCard SD: %s",
             REG_WAITCNT,
             (REG_WAITCNT == WAITCNT_SCSD) ? "Detected" : "Not detected");
    CON_Print(string);

    while (1)
        SWI_VBlankIntrWait();
}
