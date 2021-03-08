// SPDX-License-Identifier: MIT
//
// Copyright (c) 2020-2021 Antonio Niño Díaz

#include <ugba/ugba.h>

void DISP_ModeSet(int mode)
{
    REG_DISPCNT = DISPCNT_BG_MODE(mode);
}

void DISP_LayersEnable(int bg0, int bg1, int bg2, int bg3, int obj)
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

void DISP_WindowsEnable(int win0, int win1, int obj)
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

void DISP_HBLIntervalFreeEnable(int enable)
{
    if (enable)
        REG_DISPCNT |= DISPCNT_HBL_INTERVAL_FREE;
    else
        REG_DISPCNT &= ~DISPCNT_HBL_INTERVAL_FREE;
}

void DISP_ForcedBlankEnable(int enable)
{
    if (enable)
        REG_DISPCNT |= DISPCNT_FORCED_BLANK;
    else
        REG_DISPCNT &= ~DISPCNT_FORCED_BLANK;
}

void DISP_Object1DMappingEnable(int enable)
{
    if (enable)
        REG_DISPCNT |= DISPCNT_OBJ_1D_MAPPING;
    else
        REG_DISPCNT &= ~DISPCNT_OBJ_1D_MAPPING;
}

void WIN_Win0SizeSet(uint32_t left, uint32_t right, uint32_t top, uint32_t bottom)
{
    REG_WIN0H = WINH_SET(left, right);
    REG_WIN0V = WINV_SET(top, bottom);
}

void WIN_Win1SizeSet(uint32_t left, uint32_t right, uint32_t top, uint32_t bottom)
{
    REG_WIN1H = WINH_SET(left, right);
    REG_WIN1V = WINV_SET(top, bottom);
}

void WIN_Win0LayersSet(uint16_t flags_in, uint16_t flags_out)
{
    REG_WININ &= ~WIN0_ALL_ENABLE;
    REG_WININ |= flags_in;

    REG_WINOUT &= ~WIN0_ALL_ENABLE;
    REG_WINOUT |= flags_out;
}

void WIN_Win1LayersSet(uint16_t flags_in, uint16_t flags_out)
{
    REG_WININ &= ~WIN1_ALL_ENABLE;
    REG_WININ |= flags_in;

    REG_WINOUT &= ~WIN1_ALL_ENABLE;
    REG_WINOUT |= flags_out;
}

void DISP_BlendSetup(uint16_t layers_1, uint16_t layers_2, uint16_t effect)
{
    REG_BLDCNT = layers_1 | layers_2 | effect;
}

void DISP_BlendAlphaSet(int eva, int evb)
{
    REG_BLDALPHA = BLDALPHA_EVA(eva) | BLDALPHA_EVB(evb);
}

void DISP_BlendYSet(int evy)
{
    REG_BLDY = BLDY_EVY(evy);
}

void DISP_MosaicSet(uint32_t bgh, uint32_t bgw, uint32_t objh, uint32_t objw)
{
    REG_MOSAIC = MOSAIC_BG_SIZE(bgh, bgw) | MOSAIC_OBJ_SIZE(objh, objw);
}
