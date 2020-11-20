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

    // Update input state
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

void SWI_ObjAffineSet(obj_affine_src_t *src, void *dst,
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
