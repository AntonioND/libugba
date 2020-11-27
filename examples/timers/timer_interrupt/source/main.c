// SPDX-License-Identifier: MIT
//
// Copyright (c) 2020 Antonio Niño Díaz

// Example that shows how to setup standalone timers

#include <stdio.h>

#include <ugba/ugba.h>

int frames_per_period[3];
int frame_count[3];

void timer_0_handler(void)
{
    frames_per_period[0] = frame_count[0];
    frame_count[0] = 0;
}

void timer_1_handler(void)
{
    frames_per_period[1] = frame_count[1];
    frame_count[1] = 0;
}

void timer_2_handler(void)
{
    frames_per_period[2] = frame_count[2];
    frame_count[2] = 0;
}

int main(int argc, char *argv[])
{
    UGBA_Init(&argc, &argv);

    IRQ_SetHandler(IRQ_TIMER0, timer_0_handler);
    IRQ_SetHandler(IRQ_TIMER1, timer_1_handler);
    IRQ_SetHandler(IRQ_TIMER2, timer_2_handler);
    IRQ_Enable(IRQ_TIMER0);
    IRQ_Enable(IRQ_TIMER1);
    IRQ_Enable(IRQ_TIMER2);

    IRQ_Enable(IRQ_VBLANK);

    REG_DISPCNT = DISPCNT_BG_MODE(0);

    CON_InitDefault();

    // Setup 4 timers with different periods. Each one of them will use a
    // different prescaler value. The last one should fail to setup as the
    // period is too long for a single timer (it would be needed to use two
    // timers in cascade mode).
    TM_TimerStartMs(0, 100, 0, 1);
    TM_TimerStartMs(1, 500, 0, 1);
    TM_TimerStartMs(2, 1000, 0, 1);

    if (TM_TimerStartMs(3, 5000, 0, 1) == 0)
    {
        CON_Print("Timer 3 didn't fail to setup!");

        while (1)
            SWI_VBlankIntrWait();
    }

    while (1)
    {
        char str[50];
        snprintf(str, sizeof(str), "0: %d \n1: %d \n2: %d", frames_per_period[0],
                 frames_per_period[1], frames_per_period[2]);
        CON_CursorSet(0, 0);
        CON_Print(str);

        SWI_VBlankIntrWait();

        frame_count[0]++;
        frame_count[1]++;
        frame_count[2]++;
    }
}
