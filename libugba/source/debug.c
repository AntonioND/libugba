// SPDX-License-Identifier: MIT
//
// Copyright (c) 2021 Antonio Niño Díaz

#include <stdlib.h>

#include <ugba/ugba.h>

void UGBA_AssertFunction(const char *file, int line, const char *func,
                         const char *expr)
{
    // Disable interrupts so that nothing can interrupt this

    REG_IME = 0;

    // Stop sound, DMA, blending effects, windows, scroll, etc.

    SWI_RegisterRamReset(SWI_RAM_RESET_IO_SERIAL |
                         SWI_RAM_RESET_IO_SOUND |
                         SWI_RAM_RESET_IO_OTHER);

    // Setup display

    DISP_ModeSet(0);

    CON_InitDefault();

    // Print message. Split it in several calls so that it only needs a few
    // bytes of stack instead of hundreds of bytes if everything was printed
    // with one call to snprintf() and CON_Print().

    CON_Print(file);
    CON_Print(":");

    char str[10];
    itoa(line, &str[0], 10);
    CON_Print(str);

    CON_Print("\n\n");

    CON_Print(func);
    CON_Print("()\n\n");

    CON_Print("Expression failed\n\n");

    CON_Print(expr);

    // Enable minimal interrupt handling to be able to enter a low power
    // infinite loop.

    IRQ_Init();
    IRQ_Enable(IRQ_VBLANK);

    while (1)
        SWI_VBlankIntrWait();
}
