// SPDX-License-Identifier: MIT
//
// Copyright (c) 2021 Antonio Niño Díaz

// Example of how to save data in SRAM. This example lets you modify the first
// byte of SRAM, and it displays its value on the screen. If you close the
// example and reopen it, it will still hold the value it had when the example
// was closed.

#include <stdio.h>

#include <ugba/ugba.h>

int main(int argc, char *argv[])
{
    UGBA_Init(&argc, &argv);

    IRQ_Enable(IRQ_VBLANK);

    DISP_ModeSet(0);

    CON_InitDefault();

    CON_Print("Saved value: 0x");
    uint8_t *sram = MEM_SRAM;

    while (1)
    {
        char str[10];
        snprintf(str, sizeof(str), "%02X", sram[0]);
        CON_CursorSet(15, 0);
        CON_Print(str);

        KEYS_Update();

        uint16_t keys_pressed = KEYS_Pressed();

        if (keys_pressed & KEY_UP)
            sram[0]++;
        if (keys_pressed & KEY_DOWN)
            sram[0]--;

        if (keys_pressed & KEY_START)
            sram[0] = 0;

        SWI_VBlankIntrWait();
    }
}
