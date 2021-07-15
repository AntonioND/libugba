// SPDX-License-Identifier: MIT
//
// Copyright (c) 2020 Antonio Niño Díaz

#include <ugba/ugba.h>

static volatile uint16_t *BG_ControlRegisterGet(int index)
{
    volatile uint16_t *table[4] = {
        PTR_REG_16(OFFSET_BG0CNT),
        PTR_REG_16(OFFSET_BG1CNT),
        PTR_REG_16(OFFSET_BG2CNT),
        PTR_REG_16(OFFSET_BG3CNT)
    };

    return table[index];
}

void BG_RegularInit(int index, bg_regular_size size, bg_color_mode colors,
                    uintptr_t tile_base_addr, uintptr_t map_base_addr)
{
    const uint16_t size_table[BG_REGULAR_NUMBER] = {
        [BG_REGULAR_256x256] = BGCNT_REGULAR_256x256,
        [BG_REGULAR_512x256] = BGCNT_REGULAR_512x256,
        [BG_REGULAR_256x512] = BGCNT_REGULAR_256x512,
        [BG_REGULAR_512x512] = BGCNT_REGULAR_512x512,
    };

    volatile uint16_t *reg = BG_ControlRegisterGet(index);

    uint16_t value =
        size_table[size] |
        (colors == BG_256_COLORS ? BGCNT_256_COLORS : BGCNT_16_COLORS) |
        BGCNT_BG_PRIORITY(index) | // Default priority = background index
        BGCNT_TILES_BLOCK_BASE(tile_base_addr) |
        BGCNT_MAP_BLOCK_BASE(map_base_addr);

    *reg = value;
}

static int scroll_x[4], scroll_y[4];

void BG_RegularScrollSet(int index, int x, int y)
{
    scroll_x[index] = x;
    scroll_y[index] = y;

    const struct {
        volatile uint16_t *hofs;
        volatile uint16_t *vofs;
    } table[4] = {
        { PTR_REG_16(OFFSET_BG0HOFS), PTR_REG_16(OFFSET_BG0VOFS) },
        { PTR_REG_16(OFFSET_BG1HOFS), PTR_REG_16(OFFSET_BG1VOFS) },
        { PTR_REG_16(OFFSET_BG2HOFS), PTR_REG_16(OFFSET_BG2VOFS) },
        { PTR_REG_16(OFFSET_BG3HOFS), PTR_REG_16(OFFSET_BG3VOFS) }
    };

    volatile uint16_t *hofs = table[index].hofs;
    volatile uint16_t *vofs = table[index].vofs;

    *hofs = x;
    *vofs = y;
}

void BG_RegularScrollGet(int index, int *x, int *y)
{
    *x = scroll_x[index];
    *y = scroll_y[index];
}

void BG_AffineInit(int index, bg_affine_size size,
                   uintptr_t tile_base_addr, uintptr_t map_base_addr,
                   int wrap)
{
    const uint16_t size_table[BG_AFFINE_NUMBER] = {
        [BG_AFFINE_128x128] = BGCNT_AFFINE_128x128,
        [BG_AFFINE_256x256] = BGCNT_AFFINE_256x256,
        [BG_AFFINE_512x512] = BGCNT_AFFINE_512x512,
        [BG_AFFINE_1024x1024] = BGCNT_AFFINE_1024x1024,
    };

    volatile uint16_t *reg = BG_ControlRegisterGet(index);

    uint16_t value =
        size_table[size] |
        BGCNT_BG_PRIORITY(index) | // Default priority = background index
        BGCNT_TILES_BLOCK_BASE(tile_base_addr) |
        BGCNT_MAP_BLOCK_BASE(map_base_addr) |
        (wrap ? BGCNT_BG2_BG3_WRAP : 0);

    *reg = value;
}

void BG_AffineTransformSet(int index, bg_affine_dst *tr)
{
    if (index == 2)
    {
        REG_BG2PA = tr->pa;
        REG_BG2PB = tr->pb;
        REG_BG2PC = tr->pc;
        REG_BG2PD = tr->pd;
        REG_BG2X = tr->xoff;
        REG_BG2Y = tr->yoff;
    }
    else if (index == 3)
    {
        REG_BG3PA = tr->pa;
        REG_BG3PB = tr->pb;
        REG_BG3PC = tr->pc;
        REG_BG3PD = tr->pd;
        REG_BG3X = tr->xoff;
        REG_BG3Y = tr->yoff;
    }
}

void BG_AffineWrapEnable(int index, int wrap)
{
    volatile uint16_t *reg = BG_ControlRegisterGet(index);

    uint16_t value = *reg & ~BGCNT_BG2_BG3_WRAP;
    value |= wrap ? BGCNT_BG2_BG3_WRAP : 0;

    *reg = value;
}

void BG_PrioritySet(int index, int priority)
{
    volatile uint16_t *reg = BG_ControlRegisterGet(index);

    *reg &= ~BGCNT_BG_PRIORITY_MASK;
    *reg |= BGCNT_BG_PRIORITY(priority);
}

void BG_MosaicEnable(int index, int enabled)
{
    volatile uint16_t *reg = BG_ControlRegisterGet(index);

    if (enabled)
        *reg |= BGCNT_MOSAIC_ENABLE;
    else
        *reg &= ~BGCNT_MOSAIC_ENABLE;
}

uint16_t *BG_Mode3FramebufferGet(void)
{
    return MEM_VRAM_MODE3_FB;
}

uint16_t *BG_Mode4FramebufferActiveGet(void)
{
    if (REG_DISPCNT & DISPCNT_USE_BACKBUFFER)
        return MEM_VRAM_MODE4_FRAME1;
    else
        return MEM_VRAM_MODE4_FRAME0;
}

uint16_t *BG_Mode4FramebufferBackGet(void)
{
    if (!(REG_DISPCNT & DISPCNT_USE_BACKBUFFER))
        return MEM_VRAM_MODE4_FRAME1;
    else
        return MEM_VRAM_MODE4_FRAME0;
}

uint16_t *BG_Mode5FramebufferActiveGet(void)
{
    if (REG_DISPCNT & DISPCNT_USE_BACKBUFFER)
        return MEM_VRAM_MODE5_FRAME1;
    else
        return MEM_VRAM_MODE5_FRAME0;
}

uint16_t *BG_Mode5FramebufferBackGet(void)
{
    if (!(REG_DISPCNT & DISPCNT_USE_BACKBUFFER))
        return MEM_VRAM_MODE5_FRAME1;
    else
        return MEM_VRAM_MODE5_FRAME0;
}

void BG_FramebufferSet(int backbuffer)
{
    if (backbuffer)
        REG_DISPCNT |= DISPCNT_USE_BACKBUFFER;
    else
        REG_DISPCNT &= ~DISPCNT_USE_BACKBUFFER;
}

void BG_FramebufferSwap(void)
{
    REG_DISPCNT ^= DISPCNT_USE_BACKBUFFER;
}

void BG_BackdropColorSet(uint16_t color)
{
    MEM_BACKDROP_COLOR = color;
}
