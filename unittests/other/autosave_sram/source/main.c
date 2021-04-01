// SPDX-License-Identifier: MIT
//
// Copyright (c) 2021 Antonio Niño Díaz

// Example code that saves data to SRAM to test the autosave feature. This test
// is only built for SDL2, not for GBA, as it doesn't make sense to test the
// behaviour of autosave on the GBA. On SDL2 there is a delay of N seconds
// between each autosave, so it is possible to test how it behaves. On the GBA
// values are saved right away after they are written.

#include <stdio.h>
#include <stdlib.h>

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

        // Normally, when the SDL2 builds exit (with exit() or by pressing ESC)
        // the contents of SRAM are saved. During the initialization of the
        // library, atexit() is called with a function that saves the contents
        // of SRAM to the saved file.
        //
        // _Exit() skips all the functions that should be called because of
        // atexit(). This means that exiting with _Exit() simulates a crash in
        // the program that would prevent saving normally.
        if (keys_pressed & KEY_SELECT)
            _Exit(0);

        SWI_VBlankIntrWait();
    }
}
