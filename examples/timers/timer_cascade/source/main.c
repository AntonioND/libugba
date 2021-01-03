// SPDX-License-Identifier: MIT
//
// Copyright (c) 2020-2021 Antonio Niño Díaz

// Example that shows how to setup timers in cascade mode

#include <stdio.h>

#include <ugba/ugba.h>

int timer_count[4];

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

    // - Setup timer 0 to fire every 200 ms.
    //
    // - Setup timer 1 to fire every 3 ticks, in cascade mode with timer 0. It
    //   will generate an interrupt every 200 * 3 ms (0.6 seconds).

    TM_TimerStartMs(0, 200, 1);
    TM_TimerStartCascade(1, 3, 1);

    // - Setup timer 2 to fire every 100 ms.
    //
    // - Setup timer 3 to fire every 4 ticks, in cascade mode with
    //   timer 2. It will generate an interrupt every 100 * 4 ms (0.4 seconds).

    TM_TimerStartMs(2, 100, 1);
    TM_TimerStartCascade(3, 4, 1);

    while (1)
    {
        char str[50];
        snprintf(str, sizeof(str), "0: %d\n1: %d\n2: %d\n3: %d",
                 timer_count[0], timer_count[1],
                 timer_count[2], timer_count[3]);
        CON_CursorSet(0, 0);
        CON_Print(str);

        SWI_VBlankIntrWait();

        KEYS_Update();

        uint16_t keys = KEYS_Held();
        if (keys & KEY_A)
            break;
    }

    TM_TimerStop(0);
    TM_TimerStop(1);
    TM_TimerStop(2);
    TM_TimerStop(3);

    CON_Clear();
    CON_CursorSet(0, 0);

    int range[4][2] = {
        { 9, 10 },
        { 3, 3 },
        { 19, 21 },
        { 4, 5 }
    };

    for (int i = 0; i < 4; i++)
    {
        if ((timer_count[i] >= range[i][0]) && (timer_count[i] <= range[i][1]))
            CON_Print("Success\n");
        else
            CON_Print("Failed\n");
    }

    while (1)
        SWI_VBlankIntrWait();
}

