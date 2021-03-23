// SPDX-License-Identifier: MIT
//
// Copyright (c) 2021 Antonio Niño Díaz

// Test behaviour of PSG channel 3 wave RAM.

#include <stdio.h>

#include <ugba/ugba.h>

#define PATTERN0 0x1200
#define PATTERN1 0x3400

int main(int argc, char *argv[])
{
    UGBA_Init(&argc, &argv);

    IRQ_Enable(IRQ_VBLANK);

    DISP_ModeSet(0);

    CON_InitDefault();

    // Check initial state of wave RAM
    // -------------------------------

    CON_Print("Initial state\n");

    SOUND_MasterEnable(1);

    REG_SOUND3CNT_L = SOUND3CNT_L_BANK_SET(0);

    for (int i = 0; i < 8; i++)
    {
        if (REG_WAVE_RAM[i] != 0x0000)
        {
            char str[50];
            snprintf(str, sizeof(str), "%d: Failed: 0x%04X\n",
                    __LINE__, REG_WAVE_RAM[i]);
            CON_Print(str);
        }
    }

    REG_SOUND3CNT_L = SOUND3CNT_L_BANK_SET(1);

    for (int i = 0; i < 8; i++)
    {
        if (REG_WAVE_RAM[i] != 0x0000)
        {
            char str[50];
            snprintf(str, sizeof(str), "%d: Failed: 0x%04X\n",
                    __LINE__, REG_WAVE_RAM[i]);
            CON_Print(str);
        }
    }

    // Test with sound hardware OFF
    // ----------------------------

    CON_Print("Hardware OFF\n");

    SOUND_MasterEnable(0);

    // Write pattern to bank 1

    REG_SOUND3CNT_L = SOUND3CNT_L_BANK_SET(0);

    for (int i = 0; i < 8; i++)
        REG_WAVE_RAM[i] = i + PATTERN1;

    // Write pattern to bank 0

    REG_SOUND3CNT_L = SOUND3CNT_L_BANK_SET(1);

    for (int i = 0; i < 8; i++)
        REG_WAVE_RAM[i] = i + PATTERN0;

    // The pattern in bank 0 should have replaced the one in bank 1

    // Check pattern from bank 1 (which shouldn't be there)

    REG_SOUND3CNT_L = SOUND3CNT_L_BANK_SET(0);

    for (int i = 0; i < 8; i++)
    {
        if (REG_WAVE_RAM[i] == i + PATTERN1)
        {
            char str[50];
            snprintf(str, sizeof(str), "%d: Failed: 0x%04X == 0x%04X\n",
                     __LINE__, REG_WAVE_RAM[i], i + PATTERN1);
            CON_Print(str);
        }
    }

    // Check pattern from bank 0 (which should still be there)

    REG_SOUND3CNT_L = SOUND3CNT_L_BANK_SET(1);

    for (int i = 0; i < 8; i++)
    {
        if (REG_WAVE_RAM[i] != i + PATTERN0)
        {
            char str[50];
            snprintf(str, sizeof(str), "%d: Failed: 0x%04X != 0x%04X\n",
                     __LINE__, REG_WAVE_RAM[i], i + PATTERN0);
            CON_Print(str);
        }
    }

    // Check which bank is selected by default
    // ---------------------------------------

    CON_Print("Default bank\n");

    SOUND_MasterEnable(1);

    // Check pattern when selecting bank 1

    REG_SOUND3CNT_L = SOUND3CNT_L_BANK_SET(0);

    if (REG_WAVE_RAM[0] != PATTERN0)
    {
        char str[50];
        snprintf(str, sizeof(str), "%d: Failed: 0x%04X\n",
                 __LINE__, REG_WAVE_RAM[0]);
        CON_Print(str);
    }

    // Check pattern when selecting bank 0

    REG_SOUND3CNT_L = SOUND3CNT_L_BANK_SET(1);

    if (REG_WAVE_RAM[0] != 0)
    {
        char str[50];
        snprintf(str, sizeof(str), "%d: Failed: 0x%04X\n",
                 __LINE__, REG_WAVE_RAM[0]);
        CON_Print(str);
    }

    // Test with sound hardware ON
    // ---------------------------

    CON_Print("Hardware ON\n");

    SOUND_MasterEnable(1);

    // Write pattern to bank 1

    REG_SOUND3CNT_L = SOUND3CNT_L_BANK_SET(0);

    for (int i = 0; i < 8; i++)
        REG_WAVE_RAM[i] = i + PATTERN1;

    // Write pattern to bank 0

    REG_SOUND3CNT_L = SOUND3CNT_L_BANK_SET(1);

    for (int i = 0; i < 8; i++)
        REG_WAVE_RAM[i] = i + PATTERN0;

    // Check pattern from bank 1

    REG_SOUND3CNT_L = SOUND3CNT_L_BANK_SET(0);

    for (int i = 0; i < 8; i++)
    {
        if (REG_WAVE_RAM[i] != i + PATTERN1)
        {
            char str[50];
            snprintf(str, sizeof(str), "%d: Failed: 0x%04X != 0x%04X\n",
                     __LINE__, REG_WAVE_RAM[i], i + PATTERN1);
            CON_Print(str);
        }
    }

    // Check pattern from bank 0

    REG_SOUND3CNT_L = SOUND3CNT_L_BANK_SET(1);

    for (int i = 0; i < 8; i++)
    {
        if (REG_WAVE_RAM[i] != i + PATTERN0)
        {
            char str[50];
            snprintf(str, sizeof(str), "%d: Failed: 0x%04X != 0x%04X\n",
                     __LINE__, REG_WAVE_RAM[i], i + PATTERN0);
            CON_Print(str);
        }
    }

    // Which bank is selected when the audio hardware is off?
    // ------------------------------------------------------

    // Apparently bank 1 is selected regardless of the state of the bank
    // selection bit before turning off the audio hardware.

    CON_Print("Active bank with hardware OFF\n");

    // Check pattern when selecting bank 1

    SOUND_MasterEnable(1);
    REG_SOUND3CNT_L = SOUND3CNT_L_BANK_SET(0);
    SOUND_MasterEnable(0);

    if (REG_WAVE_RAM[0] != PATTERN1)
    {
        char str[50];
        snprintf(str, sizeof(str), "%d: Failed: 0x%04X\n",
                 __LINE__, REG_WAVE_RAM[0]);
        CON_Print(str);
    }

    // Check pattern when selecting bank 0

    SOUND_MasterEnable(1);
    REG_SOUND3CNT_L = SOUND3CNT_L_BANK_SET(1);
    SOUND_MasterEnable(0);

    if (REG_WAVE_RAM[0] != PATTERN1)
    {
        char str[50];
        snprintf(str, sizeof(str), "%d: Failed: 0x%04X\n",
                 __LINE__, REG_WAVE_RAM[0]);
        CON_Print(str);
    }

    while (1)
        SWI_VBlankIntrWait();
}
