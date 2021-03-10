// SPDX-License-Identifier: MIT
//
// Copyright (c) 2021 Antonio Niño Díaz

// Example that shows how nested interrupts work.
//
// This example won't run on SDL2 because nested timer interrupts are not
// supported on SDL2. There is a timer thread that calls the callbacks, so it is
// needed to return from each callback for other callbacks to be called:
//
// https://github.com/libsdl-org/SDL/blob/3a1317ed47698c268574e1d0018edd6485f5dbc4/src/timer/SDL_timer.c#L110-L114
//
// Timers can still interrupt VBL and HBL interrupt handlers, for example, as
// they are in the main thread.

#include <ugba/ugba.h>

#ifdef __GBA__

volatile int timer_count_0 = 0;
volatile int timer_count_1 = 0;
volatile int timer_count_2 = 0;
volatile int timer_count_3 = 0;

#define TIMER_1_REPETITIONS     30
#define TIMER_2_REPETITIONS     40
#define TIMER_3_REPETITIONS     50

void timer_0_handler(void)
{
    // Prevent the timer 0 interrupt from being triggered again
    TM_TimerStop(0);
    IRQ_Disable(IRQ_TIMER0);

    REG_IME = 1;

    while (timer_count_1 == 0);

    timer_count_0++;
}

void timer_1_handler(void)
{
    // Prevent the timer 1 interrupt from being triggered again
    TM_TimerStop(1);
    IRQ_Disable(IRQ_TIMER1);

    REG_IME = 1;

    while (timer_count_2 == 0);

    timer_count_1++;
}

void timer_2_handler(void)
{
    // Prevent the timer 2 interrupt from being triggered again
    TM_TimerStop(2);
    IRQ_Disable(IRQ_TIMER2);

    REG_IME = 1;

    while (timer_count_3 == 0);

    timer_count_2++;
}

void timer_3_handler(void)
{
    // Prevent the timer 3 interrupt from being triggered again
    TM_TimerStop(3);
    IRQ_Disable(IRQ_TIMER3);

    REG_IME = 1;

    timer_count_3++;
}

int main(int argc, char *argv[])
{
    UGBA_Init(&argc, &argv);

    IRQ_Enable(IRQ_VBLANK);

    DISP_ModeSet(0);

    CON_InitDefault();

    CON_Print("Waiting...\n");

    SWI_VBlankIntrWait();

    // Start all timers with interrupts disabled and enable them when they are
    // all ready. By the time IME is set to 1, all the interrupts should be
    // pending.

    REG_IME = 0;

    IRQ_SetHandler(IRQ_TIMER0, timer_0_handler);
    IRQ_SetHandler(IRQ_TIMER1, timer_1_handler);
    IRQ_SetHandler(IRQ_TIMER2, timer_2_handler);
    IRQ_SetHandler(IRQ_TIMER3, timer_3_handler);

    IRQ_Enable(IRQ_TIMER0);
    IRQ_Enable(IRQ_TIMER1);
    IRQ_Enable(IRQ_TIMER2);
    IRQ_Enable(IRQ_TIMER3);

    TM_TimerStart(0, 0xFFFF, 1, 1);
    TM_TimerStart(1, 0xFFFF, 1, 1);
    TM_TimerStart(2, 0xFFFF, 1, 1);
    TM_TimerStart(3, 0xFFFF, 1, 1);

    REG_IME = 1;

    while (timer_count_0 == 0)
        SWI_VBlankIntrWait();

    CON_Print("Success!\n");

    while (1)
        SWI_VBlankIntrWait();
}

#else

int main(int argc, char *argv[])
{
    UGBA_Init(&argc, &argv);

    IRQ_Enable(IRQ_VBLANK);

    DISP_ModeSet(0);

    CON_InitDefault();

    CON_Print("Test skipped!\n");

    while (1)
        SWI_VBlankIntrWait();
}

#endif // __GBA__
