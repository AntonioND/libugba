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

void int_to_str(int value, char *str)
{
    if (value >= 100)
    {
        int hundreds, tens, units, remainder;

        hundreds = value / 100;
        remainder = value % 100;

        str[0] = '0' + hundreds;

        tens = remainder / 10;
        units = remainder % 10;

        str[1] = '0' + tens;
        str[2] = '0' + units;
        str[3] = '\0';
    }
    else if (value >= 10)
    {
        int tens, units;

        tens = value / 10;
        units = value % 10;

        str[0] = '0' + tens;
        str[1] = '0' + units;
        str[2] = '\0';
    }
    else
    {
        str[0] = '0' + value;
        str[1] = '\0';
    }
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

    CON_Print("0:\n1:\n2:\n3:");

    while (1)
    {
        for (int i = 0; i < 4; i++)
        {
            char str[4];
            int_to_str(timer_count[i], str);
            CON_CursorSet(3, i);
            CON_Print(str);
        }

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
        { 18, 21 },
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

