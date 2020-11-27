// SPDX-License-Identifier: MIT
//
// Copyright (c) 2020 Antonio Niño Díaz

#ifndef TIMER_H__
#define TIMER_H__

#include "definitions.h"

EXPORT_API
void TM_TimerStart(int index, uint16_t reload_value, uint16_t prescaler,
                   int cascade, int enable_irq);

EXPORT_API
int TM_TimerStartMs(int index, uint32_t period_ms, int cascade, int enable_irq);

EXPORT_API
void TM_TimerStop(int index);

#endif // TIMER_H__

