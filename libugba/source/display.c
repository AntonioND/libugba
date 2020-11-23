// SPDX-License-Identifier: MIT
//
// Copyright (c) 2020 Antonio Niño Díaz

#include <ugba/ugba.h>

void DISPLAY_ModeSet(int mode)
{
    REG_DISPCNT &= ~DISPCNT_BG_MODE_MASK;
    REG_DISPCNT |= DISPCNT_BG_MODE(mode);
}
