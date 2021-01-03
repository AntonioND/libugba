// SPDX-License-Identifier: LGPL-3.0-only
//
// Copyright (c) 2020 Antonio Niño Díaz

#include <stddef.h>

#include <ugba/ugba.h>

static irq_vector IRQ_VectorTable[IRQ_NUMBER];

void IRQ_Init(void)
{
    for (int i = 0; i < IRQ_NUMBER; i ++)
        IRQ_VectorTable[i] = NULL;

    REG_IME = 1;
    REG_IE = 0;

    // The emulated register IF doesn't work like in the GBA. In the GBA, when a
    // bit is written, it is set to 0.
    REG_IF = 0;
}

void IRQ_SetHandler(irq_index index, irq_vector function)
{
    if (index < IRQ_NUMBER)
        IRQ_VectorTable[index] = function;
}

void IRQ_Enable(irq_index index)
{
    if (index >= IRQ_NUMBER)
        return;

    if (index == IRQ_VBLANK)
        REG_DISPSTAT |= DISPSTAT_VBLANK_IRQ_ENABLE;
    else if (index == IRQ_HBLANK)
        REG_DISPSTAT |= DISPSTAT_HBLANK_IRQ_ENABLE;
    else if (index == IRQ_VCOUNT)
        REG_DISPSTAT |= DISPSTAT_VCOUNT_IRQ_ENABLE;

    REG_IE |= (1 << index);
}

void IRQ_Disable(irq_index index)
{
    if (index >= IRQ_NUMBER)
        return;

    if (index == IRQ_VBLANK)
        REG_DISPSTAT &= ~DISPSTAT_VBLANK_IRQ_ENABLE;
    else if (index == IRQ_HBLANK)
        REG_DISPSTAT &= ~DISPSTAT_HBLANK_IRQ_ENABLE;
    else if (index == IRQ_VCOUNT)
        REG_DISPSTAT &= ~DISPSTAT_VCOUNT_IRQ_ENABLE;

    REG_IE &= ~(1 << index);
}

// For internal library use
void IRQ_Internal_CallHandler(irq_index index)
{
    if (index >= IRQ_NUMBER)
        return;

    if (REG_IME == 0)
        return;

    if ((REG_IE & (1 << index)) == 0)
        return;

    if (index == IRQ_VBLANK)
    {
        if ((REG_DISPSTAT & DISPSTAT_VBLANK_IRQ_ENABLE) == 0)
            return;
    }
    else if (index == IRQ_HBLANK)
    {
        if ((REG_DISPSTAT & DISPSTAT_HBLANK_IRQ_ENABLE) == 0)
            return;
    }
    else if (index == IRQ_VCOUNT)
    {
        if ((REG_DISPSTAT & DISPSTAT_VCOUNT_IRQ_ENABLE) == 0)
            return;
    }

    irq_vector vector = IRQ_VectorTable[index];
    if (vector)
        vector();
}
