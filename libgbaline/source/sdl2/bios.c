// SPDX-License-Identifier: LGPL-3.0-only
//
// Copyright (c) 2020 Antonio Niño Díaz

#include <SDL2/SDL.h>

#include <gbaline.h>

#include "input_utils.h"

#include "core/video.h"
#include "gui/win_main.h"
#include "gui/window_handler.h"

static int current_vcount = 0;

void IRQ_Internal_CallHandler(irq_index index);

static void handle_hbl(void)
{
    GBA_DrawScanline(current_vcount);

    IRQ_Internal_CallHandler(IRQ_HBLANK);

    if (current_vcount == (REG_DISPSTAT >> 8))
        IRQ_Internal_CallHandler(IRQ_VCOUNT);
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
