// SPDX-License-Identifier: MIT
//
// Copyright (c) 2020 Antonio Niño Díaz

// Example that shows the behaviour of different key states. It is slowed down
// to 1/10 of the regular refresh rate so that the "pressed" and "released"
// states can be seen (if not they would only last for 1/60 of a second!).

#include <ugba/ugba.h>

int main(int argc, char *argv[])
{
    UGBA_Init(&argc, &argv);

    IRQ_Enable(IRQ_VBLANK);

    REG_DISPCNT = DISPCNT_BG_MODE(0);

    CON_InitDefault();

    int frame = 0;

    while (1)
    {
        SWI_VBlankIntrWait();

        // Sample state of keys once every 10 frames so that the effects on the
        // screen are slowed down enough for the user to see.

        if (frame == 0)
            KEYS_Update();

        frame++;
        if (frame == 10)
            frame = 0;

        uint16_t keys_pressed = KEYS_Pressed();
        uint16_t keys_held = KEYS_Held();
        uint16_t keys_released = KEYS_Released();

        struct {
            const char *name;
            uint16_t mask;
        } keyinfo[10] = {
            { "A", KEY_A },
            { "B", KEY_B },
            { "SELECT", KEY_SELECT },
            { "START", KEY_START },
            { "RIGHT", KEY_RIGHT },
            { "LEFT", KEY_LEFT },
            { "UP", KEY_UP },
            { "DOWN", KEY_DOWN },
            { "R", KEY_R },
            { "L", KEY_L },
        };

        for (int i = 0; i < 10; i++)
        {
            CON_CursorSet(0, i);
            CON_Print(keyinfo[i].name);

            CON_CursorSet(10, i);
            if (keys_pressed & keyinfo[i].mask)
                CON_Print("P");
            else
                CON_Print(" ");

            if (keys_held & keyinfo[i].mask)
                CON_Print("H");
            else
                CON_Print(" ");

            if (keys_released & keyinfo[i].mask)
                CON_Print("R");
            else
                CON_Print(" ");
        }
    }

    return 0;
}
