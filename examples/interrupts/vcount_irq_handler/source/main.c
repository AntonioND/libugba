// SPDX-License-Identifier: MIT
//
// Copyright (c) 2020 Antonio Niño Díaz

// Example that shows how to use the VCOUNT interrupt.

#include <ugba/ugba.h>

void vbl_handler(void)
{
    MEM_BACKDROP_COLOR = RGB15(31, 0, 0);
}

void vcount_handler(void)
{
    MEM_BACKDROP_COLOR = RGB15(0, 0, 31);
}

int main(int argc, char *argv[])
{
    UGBA_Init(&argc, &argv);

    // Enable interrupts
    IRQ_SetHandler(IRQ_VBLANK, vbl_handler);
    IRQ_SetHandler(IRQ_VCOUNT, vcount_handler);
    IRQ_Enable(IRQ_VBLANK);
    IRQ_Enable(IRQ_VCOUNT);

    REG_DISPSTAT &= ~DISPSTAT_VCOUNT_MASK;
    REG_DISPSTAT |= DISPSTAT_VCOUNT(100);

    REG_DISPCNT = DISPCNT_BG_MODE(0);

    while (1)
        SWI_VBlankIntrWait();
}
