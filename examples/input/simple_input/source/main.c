// SPDX-License-Identifier: MIT
//
// Copyright (c) 2020 Antonio Niño Díaz

#include <ugba/ugba.h>

int main(int argc, char *argv[])
{
    UGBA_Init(&argc, &argv);

    IRQ_Enable(IRQ_VBLANK);

    REG_DISPCNT = 0; // Mode 0

    CON_InitDefault();

    while (1)
    {
        SWI_VBlankIntrWait();

        KEYS_Update();

        uint16_t keys_pressed = KEYS_Pressed();
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
            if (keys_pressed & keyinfo[i].mask)
            {
                CON_Print("\n");
                CON_Print(keyinfo[i].name);
                CON_Print(" pressed");
            }
            if (keys_released & keyinfo[i].mask)
            {
                CON_Print("\n");
                CON_Print(keyinfo[i].name);
                CON_Print(" released");
            }
        }
    }

    return 0;
}
