// SPDX-License-Identifier: MIT
//
// Copyright (c) 2020 Antonio Niño Díaz

#include <ugba/ugba.h>

void keypad_handler_any(void);

void keypad_handler_all(void)
{
    CON_Print("A and B pressed\n");

    IRQ_SetHandler(IRQ_KEYPAD, keypad_handler_any);
    KEYS_IRQEnablePressedAny(KEY_LEFT | KEY_RIGHT | KEY_UP | KEY_DOWN);
}

void keypad_handler_any(void)
{
    IRQ_SetHandler(IRQ_KEYPAD, keypad_handler_all);
    KEYS_IRQEnablePressedAll(KEY_A | KEY_B);

    CON_Print("Direction pressed\n");
}

int main(int argc, char *argv[])
{
    UGBA_Init(&argc, &argv);

    IRQ_Enable(IRQ_VBLANK);
    IRQ_Enable(IRQ_KEYPAD);

    REG_DISPCNT = 0; // Mode 0

    CON_InitDefault();

    IRQ_SetHandler(IRQ_KEYPAD, keypad_handler_all);
    KEYS_IRQEnablePressedAll(KEY_A | KEY_B);

    while (1)
        SWI_VBlankIntrWait();

    return 0;
}
