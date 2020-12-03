// SPDX-License-Identifier: MIT
//
// Copyright (c) 2020 Antonio Niño Díaz

#include <ugba/ugba.h>

void DISPLAY_ModeSet(int mode)
{
    REG_DISPCNT = DISPCNT_BG_MODE(mode);
}

void DISPLAY_LayersEnable(int bg0, int bg1, int bg2, int bg3, int obj)
{
    uint16_t mask = DISPCNT_BG0_ENABLE | DISPCNT_BG1_ENABLE |
                    DISPCNT_BG2_ENABLE | DISPCNT_BG3_ENABLE |
                    DISPCNT_OBJ_ENABLE;

    REG_DISPCNT &= ~mask;

    if (bg0)
        REG_DISPCNT |= DISPCNT_BG0_ENABLE;
    if (bg1)
        REG_DISPCNT |= DISPCNT_BG1_ENABLE;
    if (bg2)
        REG_DISPCNT |= DISPCNT_BG2_ENABLE;
    if (bg3)
        REG_DISPCNT |= DISPCNT_BG3_ENABLE;
    if (obj)
        REG_DISPCNT |= DISPCNT_OBJ_ENABLE;
}

void DISPLAY_WindowsEnable(int win0, int win1, int obj)
{
    uint16_t mask = DISPCNT_WIN0_ENABLE | DISPCNT_WIN1_ENABLE |
                    DISPCNT_OBJ_WIN_ENABLE;

    REG_DISPCNT &= ~mask;

    if (win0)
        REG_DISPCNT |= DISPCNT_WIN0_ENABLE;
    if (win1)
        REG_DISPCNT |= DISPCNT_WIN1_ENABLE;
    if (obj)
        REG_DISPCNT |= DISPCNT_OBJ_WIN_ENABLE;
}

void DISPLAY_HBLIntervalFreeEnable(int enable)
{
    if (enable)
        REG_DISPCNT |= DISPCNT_HBL_INTERVAL_FREE;
    else
        REG_DISPCNT &= ~DISPCNT_HBL_INTERVAL_FREE;
}

void DISPLAY_ForcedBlankEnable(int enable)
{
    if (enable)
        REG_DISPCNT |= DISPCNT_FORCED_BLANK;
    else
        REG_DISPCNT &= ~DISPCNT_FORCED_BLANK;
}

void DISPLAY_Object1DMappingEnable(int enable)
{
    if (enable)
        REG_DISPCNT |= DISPCNT_OBJ_1D_MAPPING;
    else
        REG_DISPCNT &= ~DISPCNT_OBJ_1D_MAPPING;
}
