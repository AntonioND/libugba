// SPDX-License-Identifier: LGPL-3.0-only
//
// Copyright (c) 2020 Antonio Niño Díaz

#include <math.h>

#include <SDL2/SDL.h>

#include <ugba/ugba.h>

#include "debug_utils.h"
#include "input_utils.h"
#include "interrupts.h"
#include "lua_handler.h"

#include "core/dma.h"
#include "core/video.h"
#include "gui/win_main.h"
#include "gui/window_handler.h"

static int current_vcount = 0;

static void Input_Handle_Interrupt(void)
{
    if (!(REG_KEYCNT & KEYCNT_IRQ_ENABLE))
        return;

    uint16_t keys = REG_KEYCNT & 0x03FF;
    uint16_t keyspressed = (~REG_KEYINPUT) & 0x03FF;

    if (REG_KEYCNT & KEYCNT_IRQ_CONDITION_AND)
    {
        if ((keys & keyspressed) == keys)
        {
            IRQ_Internal_CallHandler(IRQ_KEYPAD);
        }
    }
    else // KEYCNT_IRQ_CONDITION_OR
    {
        if (keys & keyspressed)
            IRQ_Internal_CallHandler(IRQ_KEYPAD);
    }
}

static void handle_hbl(void)
{
    // First, VCOUNT interrupt
    if (current_vcount == (REG_DISPSTAT >> 8))
        IRQ_Internal_CallHandler(IRQ_VCOUNT);

    // Then, draw
    GBA_DrawScanline(current_vcount);

    // Handle DMA if active
    GBA_DMAHandleHBL();

    // Finally, HBL interrupt
    IRQ_Internal_CallHandler(IRQ_HBLANK);
}

static void handle_vbl(void)
{
    // Handle DMA if active
    GBA_DMAHandleVBL();

    // Handle VBL interrupt
    IRQ_Internal_CallHandler(IRQ_VBLANK);

    // Handle GUI
    // ----------

    // Handle events for all windows
    WH_HandleEvents();

    Win_MainLoopHandle();

    // Render main window every frame
    Win_MainRender();

    // Update input state. Do this before invoking the script handler, as the
    // script can overwrite the input.
    Input_Update_GBA();

    // Update script handler
    Script_FrameDrawn();

    // Now that the user and the script input have been handled, check
    // keypad interrupt
    Input_Handle_Interrupt();

    // Synchronise video
    if (Input_Speedup_Enabled())
    {
        SDL_Delay(0);
    }
    else
    {
#define FLOAT_MS_PER_FRAME ((double)1000.0 / (double)60.0)

        static double waitforticks = 0;

        while (waitforticks >= SDL_GetTicks())
            SDL_Delay(1);

        int ticksnow = SDL_GetTicks();

        // If the emulator missed a frame or more, adjust next frame
        if (waitforticks < (ticksnow - FLOAT_MS_PER_FRAME))
            waitforticks = ticksnow + FLOAT_MS_PER_FRAME;
        else
            waitforticks += FLOAT_MS_PER_FRAME;
    }
}

static void do_scanline_draw(void)
{
    if (current_vcount < 160)
        handle_hbl();
    else if (current_vcount == 160)
        handle_vbl();

    current_vcount++;

    if (current_vcount == 228)
        current_vcount = 0;

    REG_VCOUNT = current_vcount;
}

void SWI_Halt(void)
{
    do_scanline_draw();
}

void SWI_VBlankIntrWait(void)
{
    if (current_vcount == 160)
        do_scanline_draw();

    while (current_vcount != 160)
        do_scanline_draw();
}

void SWI_CpuSet(const void *src, void *dst, uint32_t len_mode)
{
    int count = len_mode & 0x001FFFFF;
    uint32_t mode = len_mode & ~0x001FFFFF;

    if (mode & SWI_MODE_32BIT)
    {
        uint32_t *src_ = (uint32_t *)((uintptr_t)src & ~3);
        uint32_t *dst_ = (uint32_t *)((uintptr_t)dst & ~3);

        if (mode & SWI_MODE_FILL)
        {
            uint32_t fill = *src_;
            while (count--)
                *dst_++ = fill;
        }
        else // Copy
        {
            while (count--)
                *dst_++ = *src_++;
        }
    }
    else // 16 bit
    {
        uint16_t *src_ = (uint16_t *)((uintptr_t)src & ~1);
        uint16_t *dst_ = (uint16_t *)((uintptr_t)dst & ~1);

        if (mode & SWI_MODE_FILL)
        {
            uint16_t fill = *src_;
            while (count--)
                *dst_++ = fill;
        }
        else // Copy
        {
            while (count--)
                *dst_++ = *src_++;
        }
    }
}

void SWI_CpuFastSet(const void *src, void *dst, uint32_t len_mode)
{
    int count = len_mode & 0x001FFFF8; // Must be a multiple of 8 words
    uint32_t mode = len_mode & ~0x001FFFFF;

    uint32_t *src_ = (uint32_t *)((uintptr_t)src & ~3);
    uint32_t *dst_ = (uint32_t *)((uintptr_t)dst & ~3);

    if (mode & SWI_MODE_FILL)
    {
        uint32_t fill = *src_;
        while (count--)
            *dst_++ = fill;
    }
    else // Copy
    {
        while (count--)
            *dst_++ = *src_++;
    }
}

uint16_t SWI_Sqrt(uint32_t value)
{
    return (uint16_t)sqrt(value);
}

int16_t SWI_ArcTan(int16_t tan)
{
    int32_t r0 = tan;

    // This is what the BIOS does... Not accurate at the bounds.
    int32_t r1 = -((r0 * r0) >> 14);
    int32_t r3 = ((((int32_t)0xA9) * r1) >> 14) + 0x390;
    r3 = ((r1 * r3) >> 14) + 0x91C;
    r3 = ((r1 * r3) >> 14) + 0xFB6;
    r3 = ((r1 * r3) >> 14) + 0x16AA;
    r3 = ((r1 * r3) >> 14) + 0x2081;
    r3 = ((r1 * r3) >> 14) + 0x3651;
    r3 = ((r1 * r3) >> 14) + 0xA259;
    int32_t result = (r0 * r3) >> 16;

    return result;

    // Emulated: Accurate always
    // r0: Tan, 16bit: 1bit sign, 1bit integral part, 14bit decimal part
    // Return: "-PI/2 < THETA < PI/2" in a range of C000h-4000h.
    // result = (int32_t)(atan(((float)(int16_t)r0) / (float)(1 << 14))
    //                         * ((float)0x4000 / M_PI_2));
}

int16_t SWI_ArcTan2(int16_t x, int16_t y)
{
    int32_t x_ = x;
    int32_t y_ = y;

    if (y_ == 0)
    {
        if (x_ < 0)
            return (int16_t)0x8000;

        return 0;
    }

    if (x_ == 0)
    {
        if (y_ < 0)
            return (int16_t)0xC000;

        return 0x4000;
    }

    if (y_ >= 0)
    {
        if (x_ >= 0)
        {
            if (x_ < y_)
                return 0x4000 - SWI_ArcTan((x_ << 14) / y_);

            return SWI_ArcTan((y_ << 14) / x_);
        }

        if (-x_ < y_)
            return 0x4000 - SWI_ArcTan((x_ << 14) / y_);

        return 0x8000 + SWI_ArcTan((y_ << 14) / x_);
    }

    if (x_ > 0)
    {
        if (x_ < -y_)
            return 0xC000 - SWI_ArcTan((x_ << 14) / y_);

        return 0x10000 + SWI_ArcTan((y_ << 14) / x_);
    }

    if (-x_ > -y_)
        return 0x8000 + SWI_ArcTan((y_ << 14) / x_);

    return 0xC000 - SWI_ArcTan((x_ << 14) / y_);

    // Emulated: Accurate always
    // float x_ = ((float)x) / (float)(1 << 14);
    // float y_ = ((float)y) / (float)(1 << 14);
    // Return: 0000h-FFFFh for 0 <= THETA < 2PI.
    // int16_t result =
    //             (int32_t)(atan2(y_, x_) * (((float)0xFFFF) / (2.0 * M_PI)));
    // return result;
}

void SWI_BgAffineSet(const bg_affine_src *src, bg_affine_dst *dst,
                     uint32_t count)
{
    while (count--)
    {
        int32_t cx = src->bgx;
        int32_t cy = src->bgy;
        float dispx = (float)src->scrx;
        float dispy = (float)src->scry;
        float sx = ((float)src->scalex) / (float)(1 << 8);
        float sy = ((float)src->scaley) / (float)(1 << 8);
        float angle = (float)2.0 * (float)M_PI
                      * (((float)(uint8_t)(src->angle >> 8)) / (float)0xFF);

        float sin_ = sin(angle);
        float cos_ = cos(angle);
        dst->pa = cos_ * (float)(1 << 8) * sx;
        dst->pb = -sin_ * (float)(1 << 8) * sx;
        dst->pc = sin_ * (float)(1 << 8) * sy;
        dst->pd = cos_ * (float)(1 << 8) * sy;

        dst->xoff = cx - (int32_t)((cos_ * sx * dispx - sin_ * sx * dispy)
                   * (float)(1 << 8));
        dst->yoff = cy - (int32_t)((sin_ * sy * dispx + cos_ * sy * dispy)
                   * (float)(1 << 8));

        src++;
        dst++;
    }
}

void SWI_ObjAffineSet(const obj_affine_src *src, void *dst,
                      uint32_t count, uint32_t increment)
{
    if (increment & 1)
    {
        Debug_Log("%s: Please, set increment to a multiple of 2", __func__);
    }

    int16_t *out = dst;

    for (uint32_t i = 0; i < count; i++)
    {
        float sx = ((float)src->sx) / (float)(1 << 8);
        float sy = ((float)src->sy) / (float)(1 << 8);
        float angle = (float)2.0 * (float)M_PI
                      * (((float)(uint8_t)(src->angle >> 8)) / (float)0xFF);
        src++;

        float sin_ = sin(angle);
        float cos_ = cos(angle);
        int16_t A = (int16_t)(cos_ * (float)(1 << 8) * sx);
        int16_t B = (int16_t)(-sin_ * (float)(1 << 8) * sx);
        int16_t C = (int16_t)(sin_ * (float)(1 << 8) * sy);
        int16_t D = (int16_t)(cos_ * (float)(1 << 8) * sy);

        *out = A;
        out += increment / 2;
        *out = B;
        out += increment / 2;
        *out = C;
        out += increment / 2;
        *out = D;
        out += increment / 2;
    }
}

// The only difference between LZ77UnCompReadNormalWrite8bit() and
// LZ77UnCompReadNormalWrite16bit() is the width of the writes to the
// destination. There is no difference in the emulated BIOS.
static void SWI_UncompressLZ77(const void *source, void *dest)
{
    const uint8_t *src = source;
    uint16_t *dst = dest;

    // The header is 32 bits
    uint32_t header = *(uint32_t *)src;
    src += 4;

    uint32_t compression_type = (header >> 4) & 7;
    if (compression_type != 1)
    {
        Debug_Log("%s: Invalid type: %d", __func__, compression_type);
        return;
    }

    uint32_t size = (header >> 8) & 0x00FFFFFF;

    uint8_t *buffer = malloc(size + 2);
    if (buffer == NULL)
    {
        Debug_Log("%s: Not enough memory", __func__);
        return;
    }

    uint8_t *buffertmp = buffer;
    uint32_t total = 0;
    while (size > total)
    {
        uint8_t flag = *src++;

        for (int i = 0; i < 8; i++)
        {
            if (flag & 0x80)
            {
                // Compressed - Copy N+3 Bytes from Dest-Disp-1 to Dest

                uint16_t info = ((uint16_t)*src++) << 8;
                info |= (uint16_t)*src++;

                uint32_t displacement = (info & 0x0FFF);
                int num = 3 + ((info >> 12) & 0xF);
                uint32_t offset = total - displacement - 1;
                if (offset > total) // This also checks for negative values
                {
                    Debug_Log("%s: Error while decoding", __func__);
                    free(buffer);
                    return;
                }
                while (num--)
                {
                    *buffertmp++ = ((uint8_t *)buffer)[offset++];
                    total++;
                    if (size <= total)
                        break;
                }
            }
            else
            {
                // Uncompressed - Copy 1 Byte from Source to Dest
                *buffertmp++ = *src++;
                total++;
                if (size <= total)
                    break;
            }
            flag <<= 1;
        }
    }

    // Copy to destination
    memcpy(dst, buffer, size);

    free(buffer);
}

void SWI_LZ77UnCompReadNormalWrite8bit(const void *source, void *dest)
{
    SWI_UncompressLZ77(source, dest);
}

void SWI_LZ77UnCompReadNormalWrite16bit(const void *source, void *dest)
{
    SWI_UncompressLZ77(source, dest);
}
