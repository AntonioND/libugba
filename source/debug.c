// SPDX-License-Identifier: MIT
//
// Copyright (c) 2021 Antonio Niño Díaz

#include <string.h>

#include <ugba/ugba.h>

static void reverse(char *s)
{
    int e = strlen(s) - 1;

    for (int b = 0; b < e; b++, e--)
    {
        char c = s[b];
        s[b] = s[e];
        s[e] = c;
    }
}

static void itoa(int value, char *str)
{
    if (value < 0)
    {
        *str++ = '-';
        value = -value;
    }

    char *num = str;

    do
    {
        *str++ = (value % 10) + '0';
        value = value / 10;
    }
    while (value > 0);

    *str = '\0';

    reverse(num);
}

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
    itoa(line, &str[0]);
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
