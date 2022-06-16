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

    REG_IE = 0;

    // The emulated register IF doesn't work like in the GBA. In the GBA, when a
    // bit is written, it is set to 0.
    REG_IF = 0;

    REG_IME = 1;
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

// For internal library use. This is called whenever an event happens that could
// trigger an interrupt (even if IME is disabled, or the IE bit is disabled).
// This function checks if the interrupt vector has to be called before calling
// it.
void IRQ_Internal_CallHandler(irq_index index)
{
    if (index >= IRQ_NUMBER)
        return;

    // Leave the flag pending if the interrupt is masked out.
    REG_IF |= (1 << index);

    if (REG_IME == 0)
        return;

    if ((REG_IE & (1 << index)) == 0)
        return;

    REG_IF &= ~(1 << index);

    uint16_t old_ime = REG_IME;

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

    BIOS_INTR_FLAGS |= (1 << index);

    irq_vector vector = IRQ_VectorTable[index];
    if (vector)
        vector();

    REG_IME = old_ime;
}

void IRQ_TryHandleAllPendingInterrupts(void)
{
    if (REG_IME == 0)
        return;

    // HBLANK has the highest priority
    if (REG_IF & REG_IE & (1 << IRQ_HBLANK))
        IRQ_Internal_CallHandler(IRQ_HBLANK);

    // VCOUNT has the second highest priority
    if (REG_IF & REG_IE & (1 << IRQ_VCOUNT))
        IRQ_Internal_CallHandler(IRQ_VCOUNT);

    // The rest are in order
    for (int index = 0; index < IRQ_NUMBER; index++)
    {
        if ((index == IRQ_HBLANK) || (index == IRQ_VCOUNT))
            continue;

        if ((REG_IF & REG_IE & (1 << index)) == 0)
            continue;

        IRQ_Internal_CallHandler(index);
    }
}
