// SPDX-License-Identifier: LGPL-3.0-only
//
// Copyright (c) 2020 Antonio Niño Díaz

#include <SDL2/SDL.h>

#include <ugba/ugba.h>

#include "interrupts.h"

#include "../debug_utils.h"

static uint16_t curr_value[4];

static Uint32 Timer_3_Callback(Uint32 interval, UNUSED void *param)
{
    if (REG_IME == 1)
        IRQ_Internal_CallHandler(IRQ_TIMER3);

    return interval;
}

static Uint32 Timer_2_Callback(Uint32 interval, UNUSED void *param)
{
    if (REG_IME == 1)
        IRQ_Internal_CallHandler(IRQ_TIMER2);

    if (REG_TM3CNT_H & TMCNT_CASCADE)
    {
        curr_value[3] += 1;
        if (curr_value[3] == 0)
        {
            curr_value[3] = REG_TM3CNT_L;
            Timer_3_Callback(0, NULL);
        }
    }

    return interval;
}

static Uint32 Timer_1_Callback(Uint32 interval, UNUSED void *param)
{
    if (REG_IME == 1)
        IRQ_Internal_CallHandler(IRQ_TIMER1);

    if (REG_TM2CNT_H & TMCNT_CASCADE)
    {
        curr_value[2] += 1;
        if (curr_value[2] == 0)
        {
            curr_value[2] = REG_TM2CNT_L;
            Timer_2_Callback(0, NULL);
        }
    }

    return interval;
}

static Uint32 Timer_0_Callback(Uint32 interval, UNUSED void *param)
{
    if (REG_IME == 1)
        IRQ_Internal_CallHandler(IRQ_TIMER0);

    if (REG_TM1CNT_H & TMCNT_CASCADE)
    {
        curr_value[1] += 1;
        if (curr_value[1] == 0)
        {
            curr_value[1] = REG_TM1CNT_L;
            Timer_1_Callback(0, NULL);
        }
    }

    return interval;
}

SDL_TimerCallback timer_callback[4] = {
    Timer_0_Callback, Timer_1_Callback, Timer_2_Callback, Timer_3_Callback
};

SDL_TimerID TimerID[4];

#define PTR_REG_TM0CNT_L        PTR_REG_16(OFFSET_TM0CNT_L)
#define PTR_REG_TM0CNT_H        PTR_REG_16(OFFSET_TM0CNT_H)
#define PTR_REG_TM1CNT_L        PTR_REG_16(OFFSET_TM1CNT_L)
#define PTR_REG_TM1CNT_H        PTR_REG_16(OFFSET_TM1CNT_H)
#define PTR_REG_TM2CNT_L        PTR_REG_16(OFFSET_TM2CNT_L)
#define PTR_REG_TM2CNT_H        PTR_REG_16(OFFSET_TM2CNT_H)
#define PTR_REG_TM3CNT_L        PTR_REG_16(OFFSET_TM3CNT_L)
#define PTR_REG_TM3CNT_H        PTR_REG_16(OFFSET_TM3CNT_H)

static void GBA_RefreshTimer(int index)
{
    volatile uint16_t *tmcnt_l[4] = {
        PTR_REG_TM0CNT_L, PTR_REG_TM1CNT_L, PTR_REG_TM2CNT_L, PTR_REG_TM3CNT_L
    };

    volatile uint16_t *tmcnt_h[4] = {
        PTR_REG_TM0CNT_H, PTR_REG_TM1CNT_H, PTR_REG_TM2CNT_H, PTR_REG_TM3CNT_H
    };

    uint16_t reload_value = *tmcnt_l[index];
    curr_value[index] = reload_value;

    uint16_t flags = *tmcnt_h[index];

    if (TimerID[index])
        SDL_RemoveTimer(TimerID[index]);

    if ((flags & TMCNT_START) == 0)
        return;

    // Clocks per frame = 280_896
    //
    // Note:
    //     Clocks per second = 16_780_000 (Oscillator freq in Hz)
    //     Note that 16_780_000 / 60 = 279_666.66 = aprox clocks per frame
    //     However, the frequency is only an aproximation, oscillators aren't
    //     100% accurate. Just use clocks per frame for the following
    //     calculations.
    //

    const int prescaler_values[4] = {
        1, 64, 256, 1024
    };
    uint64_t prescaler = prescaler_values[flags & 3];
    uint64_t ticks_per_period = UINT16_MAX - reload_value;
    ticks_per_period += 1;
    uint64_t clocks_per_period = ticks_per_period * prescaler;

    const uint64_t clocks_per_second = 1 << 24;

    uint64_t delay_ms = (clocks_per_period * 1000) / clocks_per_second;

    if (delay_ms == 0)
    {
        Debug_Log("Timer %d: SDL_Timer not accurate enough. "
                  "This is normal if using it for DMA audio streaming.",
                  index);
        return;
    }

    TimerID[index] = SDL_AddTimer(delay_ms, timer_callback[index], NULL);
}

void GBA_TimerUpdateRegister(uint32_t offset)
{
    if (offset == OFFSET_TM0CNT_H)
        GBA_RefreshTimer(0);
    else if (offset == OFFSET_TM1CNT_H)
        GBA_RefreshTimer(1);
    else if (offset == OFFSET_TM2CNT_H)
        GBA_RefreshTimer(2);
    else if (offset == OFFSET_TM3CNT_H)
        GBA_RefreshTimer(3);
}
