// SPDX-License-Identifier: MIT
//
// Copyright (c) 2021 Antonio Niño Díaz

// Example of how to use PSG channel 3.

#include <ugba/ugba.h>

void LoadWaveRAM(void)
{
    // Bank 0

    REG_SOUND3CNT_L = SOUND3CNT_L_BANK_SET(1);
    for (int i = 0; i < 8; i++)
        REG_WAVE_RAM[i] = 0xFF00; // 0x9F05

    // Bank 1

    REG_SOUND3CNT_L = SOUND3CNT_L_BANK_SET(0);
    for (int i = 0; i < 8; i++)
        REG_WAVE_RAM[i] = 0xF0F0;
}

void DelayFrames(int frames)
{
    while (frames > 0)
    {
        frames--;
        SWI_VBlankIntrWait();
    }
}

void WaitChannelFlagClear(uint16_t mask)
{
    // In the SDL2 port, REG_SOUNDCNT_X isn't updated until the VBL is reached.
    // It is needed to do at least one iteration of the loop for it to not exit
    // right away.
    do
    {
        CON_Print(".");
        SWI_VBlankIntrWait();
    }
    while (REG_SOUNDCNT_X & mask);
}

void WaitPressA(void)
{
    while (1)
    {
        SWI_VBlankIntrWait();

        KEYS_Update();

        if (KEYS_Pressed() & KEY_A)
            break;

        // This is used for automated tests
        if (KEYS_Held() & KEY_START)
            break;
    }
}

void TestChannel3(void)
{
    CON_Print("CH3\n");

    // Test length. This should set the flag to zero after the length is
    // elapsed.

    CON_Print("1");

    REG_SOUND3CNT_L = SOUND3CNT_L_SIZE_32
                    | SOUND3CNT_L_BANK_SET(0)
                    | SOUND3CNT_L_ENABLE;

    REG_SOUND3CNT_H = SOUND3CNT_H_LENGTH_SET(200)
                    | SOUND3CNT_H_VOLUME_75;

    REG_SOUND3CNT_X = SOUND3CNT_X_SAMPLE_RATE_SET(100)
                    | SOUND3CNT_X_ONE_SHOT
                    | SOUND3CNT_X_RESTART;

    WaitChannelFlagClear(SOUNDCNT_X_PSG_3_IS_ON);

    WaitPressA();

    CON_Print("\n");

    // Test bank 0. This should set the flag to zero after the length is
    // elapsed.

    CON_Print("2");

    REG_SOUND3CNT_L = SOUND3CNT_L_SIZE_32
                    | SOUND3CNT_L_BANK_SET(0)
                    | SOUND3CNT_L_ENABLE;

    REG_SOUND3CNT_H = SOUND3CNT_H_LENGTH_SET(200)
                    | SOUND3CNT_H_VOLUME_100;

    REG_SOUND3CNT_X = SOUND3CNT_X_SAMPLE_RATE_SET(100)
                    | SOUND3CNT_X_ONE_SHOT
                    | SOUND3CNT_X_RESTART;

    WaitChannelFlagClear(SOUNDCNT_X_PSG_3_IS_ON);

    WaitPressA();

    CON_Print("\n");

    // Test bank 1. This should set the flag to zero after the length is
    // elapsed.

    CON_Print("3");

    REG_SOUND3CNT_L = SOUND3CNT_L_SIZE_32
                    | SOUND3CNT_L_BANK_SET(1)
                    | SOUND3CNT_L_ENABLE;

    REG_SOUND3CNT_H = SOUND3CNT_H_LENGTH_SET(200)
                    | SOUND3CNT_H_VOLUME_100;

    REG_SOUND3CNT_X = SOUND3CNT_X_SAMPLE_RATE_SET(100)
                    | SOUND3CNT_X_ONE_SHOT
                    | SOUND3CNT_X_RESTART;

    WaitChannelFlagClear(SOUNDCNT_X_PSG_3_IS_ON);

    WaitPressA();

    CON_Print("\n");

    // Test bank 0 + 2 banks. This should set the flag to zero after the length
    // is elapsed.

    CON_Print("4");

    REG_SOUND3CNT_L = SOUND3CNT_L_SIZE_64
                    | SOUND3CNT_L_BANK_SET(0)
                    | SOUND3CNT_L_ENABLE;

    REG_SOUND3CNT_H = SOUND3CNT_H_LENGTH_SET(200)
                    | SOUND3CNT_H_VOLUME_100;

    REG_SOUND3CNT_X = SOUND3CNT_X_SAMPLE_RATE_SET(100)
                    | SOUND3CNT_X_ONE_SHOT
                    | SOUND3CNT_X_RESTART;

    WaitChannelFlagClear(SOUNDCNT_X_PSG_3_IS_ON);

    WaitPressA();

    CON_Print("\n");

    // Test bank 1 + 2 banks. This should set the flag to zero after the length
    // is elapsed.

    CON_Print("5");

    REG_SOUND3CNT_L = SOUND3CNT_L_SIZE_64
                    | SOUND3CNT_L_BANK_SET(1)
                    | SOUND3CNT_L_ENABLE;

    REG_SOUND3CNT_H = SOUND3CNT_H_LENGTH_SET(200)
                    | SOUND3CNT_H_VOLUME_100;

    REG_SOUND3CNT_X = SOUND3CNT_X_SAMPLE_RATE_SET(100)
                    | SOUND3CNT_X_ONE_SHOT
                    | SOUND3CNT_X_RESTART;

    WaitChannelFlagClear(SOUNDCNT_X_PSG_3_IS_ON);

    WaitPressA();

    CON_Print("\n");

    // Test with no length set.

    CON_Print("6");

    REG_SOUND3CNT_L = SOUND3CNT_L_SIZE_32
                    | SOUND3CNT_L_BANK_SET(0)
                    | SOUND3CNT_L_ENABLE;

    REG_SOUND3CNT_H = SOUND3CNT_H_VOLUME_25;

    REG_SOUND3CNT_X = SOUND3CNT_X_SAMPLE_RATE_SET(200)
                    | SOUND3CNT_X_RESTART;

    DelayFrames(60);

    if ((REG_SOUNDCNT_X & SOUNDCNT_X_PSG_3_IS_ON) == 0)
        CON_Print(" Fail(1)");

    REG_SOUND3CNT_L = SOUND3CNT_L_DISABLE;

    REG_SOUND3CNT_X = SOUND3CNT_X_RESTART;

    DelayFrames(5);

    if (REG_SOUNDCNT_X & SOUNDCNT_X_PSG_3_IS_ON)
        CON_Print(" Fail(2)");

    WaitPressA();

    CON_Print("\n");

    // Test with 0% volume and fixed length. This should set the flag to zero
    // after the length is elapsed.

    CON_Print("7");

    REG_SOUND3CNT_L = SOUND3CNT_L_SIZE_32
                    | SOUND3CNT_L_BANK_SET(0)
                    | SOUND3CNT_L_ENABLE;

    REG_SOUND3CNT_H = SOUND3CNT_H_LENGTH_SET(230)
                    | SOUND3CNT_H_VOLUME_0;

    REG_SOUND3CNT_X = SOUND3CNT_X_SAMPLE_RATE_SET(100)
                    | SOUND3CNT_X_ONE_SHOT
                    | SOUND3CNT_X_RESTART;

    WaitChannelFlagClear(SOUNDCNT_X_PSG_3_IS_ON);

    WaitPressA();

    CON_Print("\n");
}

int main(int argc, char *argv[])
{
    UGBA_Init(&argc, &argv);

    IRQ_Enable(IRQ_VBLANK);

    DISP_ModeSet(0);

    CON_InitDefault();

    // The sound hardware needs to be enabled to write to any other register.
    SOUND_MasterEnable(1);

    LoadWaveRAM();

    SOUND_PSG_MasterVolume(100);
    SOUND_PSG_Volume(7, 7);
    SOUND_PSG_Pan(1, 1, 1, 1, 1, 1, 1, 1);

    TestChannel3();

    while (1)
        SWI_VBlankIntrWait();

    return 0;
}
