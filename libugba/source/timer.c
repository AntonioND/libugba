// SPDX-License-Identifier: MIT
//
// Copyright (c) 2020 Antonio Niño Díaz

#include <ugba/ugba.h>

#define PTR_REG_TM0CNT_L        PTR_REG_16(OFFSET_TM0CNT_L)
#define PTR_REG_TM0CNT_H        PTR_REG_16(OFFSET_TM0CNT_H)
#define PTR_REG_TM1CNT_L        PTR_REG_16(OFFSET_TM1CNT_L)
#define PTR_REG_TM1CNT_H        PTR_REG_16(OFFSET_TM1CNT_H)
#define PTR_REG_TM2CNT_L        PTR_REG_16(OFFSET_TM2CNT_L)
#define PTR_REG_TM2CNT_H        PTR_REG_16(OFFSET_TM2CNT_H)
#define PTR_REG_TM3CNT_L        PTR_REG_16(OFFSET_TM3CNT_L)
#define PTR_REG_TM3CNT_H        PTR_REG_16(OFFSET_TM3CNT_H)

static void TM_TimerStartInternal(int index, uint16_t reload_value,
                                  uint16_t prescaler, int cascade,
                                  int enable_irq)
{
    volatile uint16_t *tmcnt_l[4] = {
        PTR_REG_TM0CNT_L, PTR_REG_TM1CNT_L, PTR_REG_TM2CNT_L, PTR_REG_TM3CNT_L
    };

    volatile uint16_t *tmcnt_h[4] = {
        PTR_REG_TM0CNT_H, PTR_REG_TM1CNT_H, PTR_REG_TM2CNT_H, PTR_REG_TM3CNT_H
    };

    *tmcnt_h[index] = TMCNT_STOP;
    *tmcnt_l[index] = reload_value;
    *tmcnt_h[index] = prescaler | (cascade ? TMCNT_CASCADE : TMCNT_STANDALONE) |
                      (enable_irq ? TMCNT_IRQ_ENABLE : TMCNT_IRQ_DISABLE) |
                      TMCNT_START;

    const uint16_t offsets[4] = {
        OFFSET_TM0CNT_H, OFFSET_TM1CNT_H, OFFSET_TM2CNT_H, OFFSET_TM3CNT_H
    };

    UGBA_RegisterUpdatedOffset(offsets[index]);
}

void TM_TimerStart(int index, uint16_t reload_value, uint16_t prescaler,
                   int enable_irq)
{
    TM_TimerStartInternal(index, reload_value, prescaler, 0, enable_irq);
}

void TM_TimerStartCascade(int index, uint16_t period_ticks, int enable_irq)
{
    uint16_t reload_value = (UINT16_MAX + 1) - period_ticks;
    TM_TimerStartInternal(index, reload_value, 0, 1, enable_irq);
}

int TM_TimerStartMs(int index, uint32_t period_ms, int enable_irq)
{
    // Look for a prescaler value that lets us fit the given period in ms
    // into the initial value for TMxCNT_L. Start with low prescaler values to
    // keep as much granularity as possible, and increase them as needed.

    // GBA frequency is (1 << 24) Hz = 16777216 Hz (16.78 MHz)
    //
    // clocks_per_second = 1 << 24
    //
    // Translate period in milliseconds to clocks:
    //
    //                           period (ms) * clocks_per_second (clk/s)
    // clocks_per_period (clk) = ---------------------------------------
    //                                         1000 (ms/s)

    //uint64_t clocks_per_period = ((uint64_t)period_ms << 24) / 1000;
    uint64_t clocks_per_period = ((uint64_t)period_ms << 21) / 125;

    const uint32_t prescaler_shifts[4] = {
        // 1, 64, 256, 1024
        0, 6, 8, 10
    };

    for (int i = 0; i < 4; i++)
    {
        // Translate clocks to ticks + prescaler value

        // max_ticks_per_period = 65536 (Max reload value)
        //
        // ticks_per_period = clocks_per_period / prescaler (clocks/ticks)
        // if ticks_per_period <= max_ticks_per_period
        //     reload_value = max_ticks_per_period - ticks_per_period
        //     return success

        uint32_t ticks_per_period = clocks_per_period >> prescaler_shifts[i];
        uint32_t max_ticks_per_period = UINT16_MAX + 1;

        if (ticks_per_period <= max_ticks_per_period)
        {
            uint16_t reload_value = max_ticks_per_period - ticks_per_period;

            TM_TimerStart(index, reload_value, i, enable_irq);

            return 0;
        }
    }

    return -1;
}

void TM_TimerStop(int index)
{
    volatile uint16_t *tmcnt_h[4] = {
        PTR_REG_TM0CNT_H, PTR_REG_TM1CNT_H, PTR_REG_TM2CNT_H, PTR_REG_TM3CNT_H
    };

    *tmcnt_h[index] = TMCNT_STOP;

    const uint16_t offsets[4] = {
        OFFSET_TM0CNT_H, OFFSET_TM1CNT_H, OFFSET_TM2CNT_H, OFFSET_TM3CNT_H
    };

    UGBA_RegisterUpdatedOffset(offsets[index]);
}
