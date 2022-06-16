// SPDX-License-Identifier: LGPL-3.0-only
//
// Copyright (c) 2011-2015, 2019-2020 Antonio Niño Díaz

#ifndef SDL2_INTERRUPTS_H__
#define SDL2_INTERRUPTS_H__

#include <ugba/ugba.h>

void IRQ_Internal_CallHandler(irq_index index);
void IRQ_TryHandleAllPendingInterrupts(void);

#endif // SDL2_INTERRUPTS_H__
