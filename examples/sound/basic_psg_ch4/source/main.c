// SPDX-License-Identifier: MIT
//
// Copyright (c) 2021 Antonio Niño Díaz

// Example of how to use PSG channel 4.

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

void TestChannel4(void)
{
    CON_Print("CH4\n");

    // Test length. This should set the flag to zero after the length is
    // elapsed.

    CON_Print("1");

    REG_SOUND4CNT_L = SOUND4CNT_L_LENGTH_SET(30)
                    | SOUND4CNT_L_ENV_VOLUME_SET(7)
                    | SOUND4CNT_L_ENV_STEP_TIME_SET(0);

    REG_SOUND4CNT_H = SOUND4CNT_H_DIV_RATIO_SET(2)
                    | SOUND4CNT_H_WIDTH_15_BITS
                    | SOUND4CNT_H_FREQUENCY_SET(4)
                    | SOUND4CNT_H_ONE_SHOT
                    | SOUND4CNT_H_RESTART;

    WaitChannelFlagClear(SOUNDCNT_X_PSG_4_IS_ON);

    WaitPressA();

    CON_Print("\n");

    // Test increasing envelope. This should leave the ON flag set even after
    // the envelope ends.

    CON_Print("2");

    REG_SOUND4CNT_L = SOUND4CNT_L_ENV_VOLUME_SET(3)
                    | SOUND4CNT_L_ENV_STEP_TIME_SET(1)
                    | SOUND4CNT_L_ENV_DIR_INC;

    REG_SOUND4CNT_H = SOUND4CNT_H_DIV_RATIO_SET(2)
                    | SOUND4CNT_H_WIDTH_15_BITS
                    | SOUND4CNT_H_FREQUENCY_SET(4)
                    | SOUND4CNT_H_RESTART;

    DelayFrames(60);

    if ((REG_SOUNDCNT_X & SOUNDCNT_X_PSG_4_IS_ON) == 0)
        CON_Print(" Fail");

    WaitPressA();

    CON_Print("\n");

    // Test decreasing envelope. This should leave the ON flag set even after
    // the envelope ends.

    CON_Print("3");

    REG_SOUND4CNT_L = SOUND4CNT_L_ENV_VOLUME_SET(7)
                    | SOUND4CNT_L_ENV_STEP_TIME_SET(3)
                    | SOUND4CNT_L_ENV_DIR_DEC;

    REG_SOUND4CNT_H = SOUND4CNT_H_DIV_RATIO_SET(2)
                    | SOUND4CNT_H_WIDTH_15_BITS
                    | SOUND4CNT_H_FREQUENCY_SET(4)
                    | SOUND4CNT_H_RESTART;

    DelayFrames(60);

    if ((REG_SOUNDCNT_X & SOUNDCNT_X_PSG_4_IS_ON) == 0)
        CON_Print(" Fail");

    WaitPressA();

    CON_Print("\n");

    // Envelope + length. This should set the flag to zero after the speficied
    // length.

    CON_Print("4");

    REG_SOUND4CNT_L = SOUND4CNT_L_LENGTH_SET(0)
                    | SOUND4CNT_L_ENV_VOLUME_SET(7)
                    | SOUND4CNT_L_ENV_STEP_TIME_SET(7)
                    | SOUND4CNT_L_ENV_DIR_DEC;

    REG_SOUND4CNT_H = SOUND4CNT_H_DIV_RATIO_SET(2)
                    | SOUND4CNT_H_WIDTH_15_BITS
                    | SOUND4CNT_H_FREQUENCY_SET(4)
                    | SOUND4CNT_H_ONE_SHOT
                    | SOUND4CNT_H_RESTART;

    WaitChannelFlagClear(SOUNDCNT_X_PSG_4_IS_ON);

    WaitPressA();

    CON_Print("\n");

    // Test width = 7 bits.

    CON_Print("5");

    REG_SOUND4CNT_L = SOUND4CNT_L_ENV_VOLUME_SET(7)
                    | SOUND4CNT_L_ENV_STEP_TIME_SET(0);

    REG_SOUND4CNT_H = SOUND4CNT_H_DIV_RATIO_SET(1)
                    | SOUND4CNT_H_WIDTH_7_BITS
                    | SOUND4CNT_H_FREQUENCY_SET(6)
                    | SOUND4CNT_H_RESTART;

    DelayFrames(60);

    if ((REG_SOUNDCNT_X & SOUNDCNT_X_PSG_4_IS_ON) == 0)
        CON_Print(" Fail");

    WaitPressA();

    CON_Print("\n");

    // Test width = 15 bits.

    CON_Print("6");

    REG_SOUND4CNT_L = SOUND4CNT_L_ENV_VOLUME_SET(7)
                    | SOUND4CNT_L_ENV_STEP_TIME_SET(0);

    REG_SOUND4CNT_H = SOUND4CNT_H_DIV_RATIO_SET(1)
                    | SOUND4CNT_H_WIDTH_15_BITS
                    | SOUND4CNT_H_FREQUENCY_SET(6)
                    | SOUND4CNT_H_RESTART;

    DelayFrames(60);

    if ((REG_SOUNDCNT_X & SOUNDCNT_X_PSG_4_IS_ON) == 0)
        CON_Print(" Fail");

    WaitPressA();

    CON_Print("\n");

    // Test div ratios

    CON_Print("7");

    REG_SOUND4CNT_L = SOUND4CNT_L_ENV_VOLUME_SET(7)
                    | SOUND4CNT_L_ENV_STEP_TIME_SET(0);

    REG_SOUND4CNT_H = SOUND4CNT_H_DIV_RATIO_SET(3)
                    | SOUND4CNT_H_WIDTH_15_BITS
                    | SOUND4CNT_H_FREQUENCY_SET(7)
                    | SOUND4CNT_H_RESTART;

    DelayFrames(60);

    if ((REG_SOUNDCNT_X & SOUNDCNT_X_PSG_4_IS_ON) == 0)
        CON_Print(" Fail");

    WaitPressA();

    CON_Print("\n");

    // Test invalid frequencies

    CON_Print("8");

    REG_SOUND4CNT_L = SOUND4CNT_L_ENV_VOLUME_SET(7)
                    | SOUND4CNT_L_ENV_STEP_TIME_SET(0);

    REG_SOUND4CNT_H = SOUND4CNT_H_DIV_RATIO_SET(1)
                    | SOUND4CNT_H_WIDTH_15_BITS
                    | SOUND4CNT_H_FREQUENCY_SET(14)
                    | SOUND4CNT_H_RESTART;

    DelayFrames(60);

    if ((REG_SOUNDCNT_X & SOUNDCNT_X_PSG_4_IS_ON) == 0)
        CON_Print(" Fail");

    WaitPressA();

    CON_Print("\n");

    // Test valid frequency

    CON_Print("9");

    REG_SOUND4CNT_L = SOUND4CNT_L_ENV_VOLUME_SET(7)
                    | SOUND4CNT_L_ENV_STEP_TIME_SET(0);

    REG_SOUND4CNT_H = SOUND4CNT_H_DIV_RATIO_SET(1)
                    | SOUND4CNT_H_WIDTH_15_BITS
                    | SOUND4CNT_H_FREQUENCY_SET(6)
                    | SOUND4CNT_H_RESTART;

    DelayFrames(60);

    if ((REG_SOUNDCNT_X & SOUNDCNT_X_PSG_4_IS_ON) == 0)
        CON_Print(" Fail");

    WaitPressA();

    CON_Print("\n");

    // Test valid frequency

    CON_Print("10");

    REG_SOUND4CNT_L = SOUND4CNT_L_ENV_VOLUME_SET(7)
                    | SOUND4CNT_L_ENV_STEP_TIME_SET(0);

    REG_SOUND4CNT_H = SOUND4CNT_H_DIV_RATIO_SET(1)
                    | SOUND4CNT_H_WIDTH_15_BITS
                    | SOUND4CNT_H_FREQUENCY_SET(13)
                    | SOUND4CNT_H_RESTART;

    DelayFrames(60);

    if ((REG_SOUNDCNT_X & SOUNDCNT_X_PSG_4_IS_ON) == 0)
        CON_Print(" Fail");

    WaitPressA();

    CON_Print("\n");

    // Disable

    REG_SOUND4CNT_L = SOUND4CNT_L_ENV_VOLUME_SET(0)
                    | SOUND4CNT_L_ENV_STEP_TIME_SET(0);

    REG_SOUND4CNT_H = SOUND4CNT_H_RESTART;
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

    TestChannel4();

    while (1)
        SWI_VBlankIntrWait();

    return 0;
}
