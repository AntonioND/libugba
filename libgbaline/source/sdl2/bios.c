// SPDX-License-Identifier: LGPL-3.0-only
//
// Copyright (c) 2020 Antonio Niño Díaz

#include <SDL2/SDL.h>

#include <gbaline.h>

#include "debug_utils.h"
#include "input_utils.h"
#include "lua_handler.h"

#include "core/video.h"
#include "gui/win_main.h"
#include "gui/window_handler.h"

static int current_vcount = 0;

void IRQ_Internal_CallHandler(irq_index index);

static void handle_hbl(void)
{
    // First, VCOUNT interrupt
    if (current_vcount == (REG_DISPSTAT >> 8))
        IRQ_Internal_CallHandler(IRQ_VCOUNT);

    // Then, draw
    GBA_DrawScanline(current_vcount);

    // Finally, HBL interrupt
    IRQ_Internal_CallHandler(IRQ_HBLANK);
}

static void handle_vbl(void)
{
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

void SWI_ObjAffineSet(const obj_affine_src_t *src, void *dst,
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
