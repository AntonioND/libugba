// SPDX-License-Identifier: MIT
//
// Copyright (c) 2021 Antonio Niño Díaz

// Example of how to use PSG channel 1.

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

void TestChannel1(void)
{
    CON_Print("CH1\n");

    // Test length. This should set the flag to zero after the length is
    // elapsed.

    CON_Print("1");

    REG_SOUND1CNT_L = SOUND1CNT_L_SWEEP_TIME_SET(0);

    REG_SOUND1CNT_H = SOUND1CNT_H_LENGTH_SET(30)
                    | SOUND1CNT_H_WAVE_DUTY_50
                    | SOUND1CNT_H_ENV_VOLUME_SET(7)
                    | SOUND1CNT_H_ENV_STEP_TIME_SET(0);

    REG_SOUND1CNT_X = SOUND1CNT_X_FREQUENCY_SET(1900)
                    | SOUND1CNT_X_ONE_SHOT
                    | SOUND1CNT_X_RESTART;

    WaitChannelFlagClear(SOUNDCNT_X_PSG_1_IS_ON);

    WaitPressA();

    CON_Print("\n");

    // Test increasing envelope. This should leave the ON flag set even after
    // the envelope ends.

    CON_Print("2");

    REG_SOUND1CNT_L = SOUND1CNT_L_SWEEP_TIME_SET(0);

    REG_SOUND1CNT_H = SOUND1CNT_H_WAVE_DUTY_50
                    | SOUND1CNT_H_ENV_VOLUME_SET(3)
                    | SOUND1CNT_H_ENV_STEP_TIME_SET(1)
                    | SOUND1CNT_H_ENV_DIR_INC;

    REG_SOUND1CNT_X = SOUND1CNT_X_FREQUENCY_SET(1900)
                    | SOUND1CNT_X_RESTART;

    DelayFrames(60);

    if ((REG_SOUNDCNT_X & SOUNDCNT_X_PSG_1_IS_ON) == 0)
        CON_Print(" Fail ");

    WaitPressA();

    CON_Print("\n");

    // Test decreasing envelope. This should leave the ON flag set even after
    // the envelope ends.

    CON_Print("3");

    REG_SOUND1CNT_L = SOUND1CNT_L_SWEEP_TIME_SET(0);

    REG_SOUND1CNT_H = SOUND1CNT_H_WAVE_DUTY_50
                    | SOUND1CNT_H_ENV_VOLUME_SET(7)
                    | SOUND1CNT_H_ENV_STEP_TIME_SET(3)
                    | SOUND1CNT_H_ENV_DIR_DEC;

    REG_SOUND1CNT_X = SOUND1CNT_X_FREQUENCY_SET(1900)
                    | SOUND1CNT_X_RESTART;

    DelayFrames(60);

    if ((REG_SOUNDCNT_X & SOUNDCNT_X_PSG_1_IS_ON) == 0)
        CON_Print(" Fail ");

    WaitPressA();

    CON_Print("\n");

    // Sweep up

    CON_Print("4");

    REG_SOUND1CNT_L = SOUND1CNT_L_SWEEP_SHIFT_SET(2)
                    | SOUND1CNT_L_SWEEP_DIR_INC
                    | SOUND1CNT_L_SWEEP_TIME_SET(4);

    REG_SOUND1CNT_H = SOUND1CNT_H_LENGTH_SET(0)
                    | SOUND1CNT_H_WAVE_DUTY_50
                    | SOUND1CNT_H_ENV_VOLUME_SET(7)
                    | SOUND1CNT_H_ENV_STEP_TIME_SET(0);

    REG_SOUND1CNT_X = SOUND1CNT_X_FREQUENCY_SET(1024)
                    | SOUND1CNT_X_ONE_SHOT
                    | SOUND1CNT_X_RESTART;

    DelayFrames(60);

    if (REG_SOUNDCNT_X & SOUNDCNT_X_PSG_1_IS_ON)
        CON_Print(" Fail ");

    WaitPressA();

    CON_Print("\n");

    // Sweep down

    CON_Print("5");

    REG_SOUND1CNT_L = SOUND1CNT_L_SWEEP_SHIFT_SET(2)
                    | SOUND1CNT_L_SWEEP_DIR_DEC
                    | SOUND1CNT_L_SWEEP_TIME_SET(4);

    REG_SOUND1CNT_H = SOUND1CNT_H_WAVE_DUTY_50
                    | SOUND1CNT_H_ENV_VOLUME_SET(7)
                    | SOUND1CNT_H_ENV_STEP_TIME_SET(0);

    REG_SOUND1CNT_X = SOUND1CNT_X_FREQUENCY_SET(1024)
                    | SOUND1CNT_X_RESTART;

    DelayFrames(60);

    if ((REG_SOUNDCNT_X & SOUNDCNT_X_PSG_1_IS_ON) == 0)
        CON_Print(" Fail ");

    WaitPressA();

    CON_Print("\n");

    // Sweep + envelope + length. This should set the flag to zero after the
    // speficied length.

    CON_Print("6");

    REG_SOUND1CNT_L = SOUND1CNT_L_SWEEP_SHIFT_SET(2)
                    | SOUND1CNT_L_SWEEP_DIR_DEC
                    | SOUND1CNT_L_SWEEP_TIME_SET(7);

    REG_SOUND1CNT_H = SOUND1CNT_H_LENGTH_SET(6)
                    | SOUND1CNT_H_WAVE_DUTY_50
                    | SOUND1CNT_H_ENV_VOLUME_SET(7)
                    | SOUND1CNT_H_ENV_STEP_TIME_SET(7)
                    | SOUND1CNT_H_ENV_DIR_DEC;

    REG_SOUND1CNT_X = SOUND1CNT_X_FREQUENCY_SET(1900)
                    | SOUND1CNT_X_ONE_SHOT
                    | SOUND1CNT_X_RESTART;

    WaitChannelFlagClear(SOUNDCNT_X_PSG_1_IS_ON);

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

    TestChannel1();

    while (1)
        SWI_VBlankIntrWait();

    return 0;
}
