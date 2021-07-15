// SPDX-License-Identifier: MIT
//
// Copyright (c) 2021 Antonio Niño Díaz

#include <ugba/ugba.h>

void IRQ_SetReferenceVCOUNT(uint32_t y)
{
    uint16_t dispstat = REG_DISPSTAT;

    dispstat &= ~DISPSTAT_VCOUNT_MASK;
    dispstat |= DISPSTAT_VCOUNT(y);

    REG_DISPSTAT = dispstat;
}
