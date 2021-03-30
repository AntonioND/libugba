// SPDX-License-Identifier: MIT
//
// Copyright (c) 2021 Antonio Niño Díaz

// Example of how to use the global settings of the PSG channels.

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

void TestGlobal(void)
{
    // Start by setting up all channels and silencing everything but channel 1
    // for the first part

    SOUND_PSG_MasterVolume(100);
    SOUND_PSG_Volume(7, 7);
    SOUND_PSG_Pan(1, 1, 0, 0, 0, 0, 0, 0);

    REG_SOUND1CNT_L = SOUND1CNT_L_SWEEP_TIME_SET(0);

    REG_SOUND1CNT_H = SOUND1CNT_H_WAVE_DUTY_50
                    | SOUND1CNT_H_ENV_VOLUME_SET(7)
                    | SOUND1CNT_H_ENV_STEP_TIME_SET(0);

    REG_SOUND1CNT_X = SOUND1CNT_X_FREQUENCY_SET(1900)
                    | SOUND1CNT_X_RESTART;

    REG_SOUND2CNT_L = SOUND2CNT_L_WAVE_DUTY_50
                    | SOUND2CNT_L_ENV_VOLUME_SET(3)
                    | SOUND2CNT_L_ENV_STEP_TIME_SET(1)
                    | SOUND2CNT_L_ENV_DIR_INC;

    REG_SOUND2CNT_H = SOUND2CNT_H_FREQUENCY_SET(1000)
                    | SOUND2CNT_H_RESTART;

    REG_SOUND3CNT_L = SOUND3CNT_L_SIZE_32
                    | SOUND3CNT_L_BANK_SET(0)
                    | SOUND3CNT_L_ENABLE;

    REG_SOUND3CNT_H = SOUND3CNT_H_VOLUME_100;

    REG_SOUND3CNT_X = SOUND3CNT_X_SAMPLE_RATE_SET(200)
                    | SOUND3CNT_X_RESTART;

    REG_SOUND4CNT_L = SOUND4CNT_L_ENV_VOLUME_SET(7)
                    | SOUND4CNT_L_ENV_STEP_TIME_SET(0);

    REG_SOUND4CNT_H = SOUND4CNT_H_DIV_RATIO_SET(1)
                    | SOUND4CNT_H_WIDTH_15_BITS
                    | SOUND4CNT_H_FREQUENCY_SET(4)
                    | SOUND4CNT_H_RESTART;

    // Master volume

    CON_Print("Master volume\n");

    SOUND_PSG_MasterVolume(100);
    DelayFrames(20);
    SOUND_PSG_MasterVolume(50);
    DelayFrames(20);
    SOUND_PSG_MasterVolume(100);

    WaitPressA();

    // Left/right volumes

    CON_Print("L/R volume\n");

    SOUND_PSG_Volume(7, 7);
    DelayFrames(20);
    SOUND_PSG_Volume(2, 7);
    DelayFrames(20);
    SOUND_PSG_Volume(7, 2);
    DelayFrames(20);
    SOUND_PSG_Volume(7, 7);

    WaitPressA();

    // Panning

    SOUND_PSG_Pan(0, 0, 0, 0, 0, 0, 0, 0);
    DelayFrames(20);

    CON_Print("Left panning\n");

    SOUND_PSG_Pan(1, 0, 0, 0, 0, 0, 0, 0);
    DelayFrames(20);
    SOUND_PSG_Pan(0, 0, 1, 0, 0, 0, 0, 0);
    DelayFrames(20);
    SOUND_PSG_Pan(0, 0, 0, 0, 1, 0, 0, 0);
    DelayFrames(20);
    SOUND_PSG_Pan(0, 0, 0, 0, 0, 0, 1, 0);
    DelayFrames(20);
    SOUND_PSG_Pan(0, 0, 0, 0, 0, 0, 0, 0);

    WaitPressA();

    CON_Print("Right panning\n");

    SOUND_PSG_Pan(0, 1, 0, 0, 0, 0, 0, 0);
    DelayFrames(20);
    SOUND_PSG_Pan(0, 0, 0, 1, 0, 0, 0, 0);
    DelayFrames(20);
    SOUND_PSG_Pan(0, 0, 0, 0, 0, 1, 0, 0);
    DelayFrames(20);
    SOUND_PSG_Pan(0, 0, 0, 0, 0, 0, 0, 1);
    DelayFrames(20);
    SOUND_PSG_Pan(0, 0, 0, 0, 0, 0, 0, 0);

    WaitPressA();

    // Disable all

    REG_SOUND1CNT_L = SOUND1CNT_L_SWEEP_TIME_SET(0);
    REG_SOUND1CNT_H = SOUND1CNT_H_ENV_VOLUME_SET(0)
                    | SOUND1CNT_H_ENV_STEP_TIME_SET(0);
    REG_SOUND1CNT_X = SOUND1CNT_X_RESTART;

    REG_SOUND2CNT_L = SOUND2CNT_L_ENV_VOLUME_SET(0)
                    | SOUND2CNT_L_ENV_STEP_TIME_SET(0);
    REG_SOUND2CNT_H = SOUND2CNT_H_RESTART;

    REG_SOUND3CNT_L = SOUND3CNT_L_DISABLE;
    REG_SOUND3CNT_H = SOUND3CNT_H_VOLUME_0;
    REG_SOUND3CNT_X = SOUND3CNT_X_RESTART;

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

    LoadWaveRAM();

    TestGlobal();

    while (1)
        SWI_VBlankIntrWait();

    return 0;
}
