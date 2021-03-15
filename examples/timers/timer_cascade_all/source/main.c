// SPDX-License-Identifier: MIT
//
// Copyright (c) 2020-2021 Antonio Niño Díaz

// Example that shows how to setup all timers in cascade mode.

#include <stdio.h>

#include <ugba/ugba.h>

volatile int timer_count[4];

void timer_0_handler(void)
{
    timer_count[0] += 1;
}

void timer_1_handler(void)
{
    timer_count[1] += 1;
}

void timer_2_handler(void)
{
    timer_count[2] += 1;
}

void timer_3_handler(void)
{
    timer_count[3] += 1;
}

int main(int argc, char *argv[])
{
    UGBA_Init(&argc, &argv);

    IRQ_Enable(IRQ_VBLANK);

    DISP_ModeSet(0);

    IRQ_SetHandler(IRQ_TIMER0, timer_0_handler);
    IRQ_SetHandler(IRQ_TIMER1, timer_1_handler);
    IRQ_SetHandler(IRQ_TIMER2, timer_2_handler);
    IRQ_SetHandler(IRQ_TIMER3, timer_3_handler);
    IRQ_Enable(IRQ_TIMER0);
    IRQ_Enable(IRQ_TIMER1);
    IRQ_Enable(IRQ_TIMER2);
    IRQ_Enable(IRQ_TIMER3);

    CON_InitDefault();

    // - Setup timer 0 to fire every 50 ms.
    // - Setup timer 1 to fire every 2 ticks, in cascade mode with timer 0.
    // - Setup timer 2 to fire every 3 ticks, in cascade mode with timer 1.
    // - Setup timer 3 to fire every 2 ticks, in cascade mode with timer 2.
    //
    // Result:
    //
    // - Timer 0: 50 ms
    // - Timer 1: 50 * 2 = 100 ms
    // - Timer 2: 50 * 2 * 3 = 300 ms
    // - Timer 3: 50 * 2 * 3 * 2 = 600 ms

    TM_TimerStartMs(0, 50, 1);
    TM_TimerStartCascade(1, 2, 1);
    TM_TimerStartCascade(2, 3, 1);
    TM_TimerStartCascade(3, 2, 1);

    while (1)
    {
        char str[50];
        snprintf(str, sizeof(str), "0: %d\n1: %d\n2: %d\n3: %d",
                 timer_count[0], timer_count[1],
                 timer_count[2], timer_count[3]);
        CON_CursorSet(0, 0);
        CON_Print(str);

        SWI_VBlankIntrWait();

        if (timer_count[0] == 20)
        {
            TM_TimerStop(0);

            KEYS_Update();

            uint16_t keys = KEYS_Held();
            if (keys & KEY_A)
                break;
        }
    }

    TM_TimerStop(0);

    CON_Clear();
    CON_CursorSet(0, 0);

    int reference[4] = {
        20, 10, 3, 1
    };

    for (int i = 0; i < 4; i++)
    {
        if (timer_count[i] == reference[i])
            CON_Print("Success\n");
        else
            CON_Print("Failed\n");
    }

    while (1)
        SWI_VBlankIntrWait();
}

