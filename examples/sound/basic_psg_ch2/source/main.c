// SPDX-License-Identifier: MIT
//
// Copyright (c) 2021 Antonio Niño Díaz

// Example of how to use PSG channel 2.

#include <ugba/ugba.h>

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

void TestChannel2(void)
{
    CON_Print("CH2\n");

    // Test length + 12.5% duty. This should set the flag to zero after the
    // length is elapsed.

    CON_Print("1");

    REG_SOUND2CNT_L = SOUND2CNT_L_LENGTH_SET(30)
                    | SOUND2CNT_L_WAVE_DUTY_12
                    | SOUND2CNT_L_ENV_VOLUME_SET(7)
                    | SOUND2CNT_L_ENV_STEP_TIME_SET(0);

    REG_SOUND2CNT_H = SOUND2CNT_H_FREQUENCY_SET(1900)
                    | SOUND2CNT_H_ONE_SHOT
                    | SOUND2CNT_H_RESTART;

    WaitChannelFlagClear(SOUNDCNT_X_PSG_2_IS_ON);

    WaitPressA();

    CON_Print("\n");

    // Test length + 25% duty. This should set the flag to zero after the
    // length is elapsed.

    CON_Print("2");

    REG_SOUND2CNT_L = SOUND2CNT_L_LENGTH_SET(5)
                    | SOUND2CNT_L_WAVE_DUTY_25
                    | SOUND2CNT_L_ENV_VOLUME_SET(7)
                    | SOUND2CNT_L_ENV_STEP_TIME_SET(0);

    REG_SOUND2CNT_H = SOUND2CNT_H_FREQUENCY_SET(1900)
                    | SOUND2CNT_H_ONE_SHOT
                    | SOUND2CNT_H_RESTART;

    WaitChannelFlagClear(SOUNDCNT_X_PSG_2_IS_ON);

    WaitPressA();

    CON_Print("\n");

    // Test length + 50% duty. This should set the flag to zero after the
    // length is elapsed.

    CON_Print("3");

    REG_SOUND2CNT_L = SOUND2CNT_L_LENGTH_SET(5)
                    | SOUND2CNT_L_WAVE_DUTY_50
                    | SOUND2CNT_L_ENV_VOLUME_SET(7)
                    | SOUND2CNT_L_ENV_STEP_TIME_SET(0);

    REG_SOUND2CNT_H = SOUND2CNT_H_FREQUENCY_SET(1900)
                    | SOUND2CNT_H_ONE_SHOT
                    | SOUND2CNT_H_RESTART;

    WaitChannelFlagClear(SOUNDCNT_X_PSG_2_IS_ON);

    WaitPressA();

    CON_Print("\n");

    // Test length + 75% duty. This should set the flag to zero after the
    // length is elapsed.

    CON_Print("4");

    REG_SOUND2CNT_L = SOUND2CNT_L_LENGTH_SET(5)
                    | SOUND2CNT_L_WAVE_DUTY_75
                    | SOUND2CNT_L_ENV_VOLUME_SET(7)
                    | SOUND2CNT_L_ENV_STEP_TIME_SET(0);

    REG_SOUND2CNT_H = SOUND2CNT_H_FREQUENCY_SET(1900)
                    | SOUND2CNT_H_ONE_SHOT
                    | SOUND2CNT_H_RESTART;

    WaitChannelFlagClear(SOUNDCNT_X_PSG_2_IS_ON);

    WaitPressA();

    CON_Print("\n");

    // Test increasing envelope. This should leave the ON flag set even after
    // the envelope ends.

    CON_Print("5");

    REG_SOUND2CNT_L = SOUND2CNT_L_WAVE_DUTY_50
                    | SOUND2CNT_L_ENV_VOLUME_SET(3)
                    | SOUND2CNT_L_ENV_STEP_TIME_SET(1)
                    | SOUND2CNT_L_ENV_DIR_INC;

    REG_SOUND2CNT_H = SOUND2CNT_H_FREQUENCY_SET(1900)
                    | SOUND2CNT_H_RESTART;

    DelayFrames(60);

    if ((REG_SOUNDCNT_X & SOUNDCNT_X_PSG_2_IS_ON) == 0)
        CON_Print(" Fail");

    WaitPressA();

    CON_Print("\n");

    // Test decreasing envelope. This should leave the ON flag set even after
    // the envelope ends.

    CON_Print("6");

    REG_SOUND2CNT_L = SOUND2CNT_L_WAVE_DUTY_50
                    | SOUND2CNT_L_ENV_VOLUME_SET(7)
                    | SOUND2CNT_L_ENV_STEP_TIME_SET(3)
                    | SOUND2CNT_L_ENV_DIR_DEC;

    REG_SOUND2CNT_H = SOUND2CNT_H_FREQUENCY_SET(1900)
                    | SOUND2CNT_H_RESTART;

    DelayFrames(60);

    if ((REG_SOUNDCNT_X & SOUNDCNT_X_PSG_2_IS_ON) == 0)
        CON_Print(" Fail!");

    WaitPressA();

    CON_Print("\n");

    // Envelope + length. This should set the flag to zero after the speficied
    // length.

    CON_Print("7");

    REG_SOUND2CNT_L = SOUND2CNT_L_LENGTH_SET(0)
                    | SOUND2CNT_L_WAVE_DUTY_50
                    | SOUND2CNT_L_ENV_VOLUME_SET(7)
                    | SOUND2CNT_L_ENV_STEP_TIME_SET(7)
                    | SOUND2CNT_L_ENV_DIR_DEC;

    REG_SOUND2CNT_H = SOUND2CNT_H_FREQUENCY_SET(1900)
                    | SOUND2CNT_H_ONE_SHOT
                    | SOUND2CNT_H_RESTART;

    WaitChannelFlagClear(SOUNDCNT_X_PSG_2_IS_ON);

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

    SOUND_PSG_MasterVolume(100);
    SOUND_PSG_Volume(7, 7);
    SOUND_PSG_Pan(1, 1, 1, 1, 1, 1, 1, 1);

    TestChannel2();

    while (1)
        SWI_VBlankIntrWait();

    return 0;
}
