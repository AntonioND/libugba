// SPDX-License-Identifier: LGPL-3.0-only
//
// Copyright (c) 2020 Antonio Niño Díaz

#include <SDL2/SDL.h>

#include <ugba/ugba.h>

#include "interrupts.h"
#include "dma.h"
#include "sound.h"
#include "video.h"

#include "../debug_utils.h"
#include "../input_utils.h"
#include "../lua_handler.h"

#include "../gui/win_main.h"
#include "../gui/window_handler.h"

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

    uint16_t dispstat_vcount = REG_DISPSTAT & DISPSTAT_VCOUNT_MASK;
    dispstat_vcount >>= DISPSTAT_VCOUNT_SHIFT;

    if (current_vcount == dispstat_vcount)
        IRQ_Internal_CallHandler(IRQ_VCOUNT);

    // Then, draw
    GBA_DrawScanline(current_vcount);

    // Handle DMA if active
    GBA_DMAHandleHBL();

    // Finally, HBL interrupt
    IRQ_Internal_CallHandler(IRQ_HBLANK);
}

static void handle_hbl_during_vbl(void)
{
    // First, VCOUNT interrupt

    uint16_t dispstat_vcount = REG_DISPSTAT & DISPSTAT_VCOUNT_MASK;
    dispstat_vcount >>= DISPSTAT_VCOUNT_SHIFT;

    if (current_vcount == dispstat_vcount)
        IRQ_Internal_CallHandler(IRQ_VCOUNT);

    // In this case, there is nothing to draw, and DMA isn't triggered.

    // Finally, HBL interrupt
    IRQ_Internal_CallHandler(IRQ_HBLANK);
}

static void handle_vbl(void)
{
    // Handle DMA if active
    GBA_DMAHandleVBL();

    // Handle sound before calling the VBL interrupt handler
    Sound_Handle_VBL();

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
    {
        handle_hbl();
    }
    else if (current_vcount == 160)
    {
        handle_vbl();
        handle_hbl_during_vbl();
    }
    else
    {
        handle_hbl_during_vbl();
    }

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

NORETURN void SWI_SoftReset(void)
{
    // TODO: Is there a way to implement this in a cross platform way?

    Debug_Log("%s(): Not supported!\n");
    WH_CloseAll();
    exit(0);
}

NORETURN void SWI_HardReset(void)
{
    // TODO: Is there a way to implement this in a cross platform way?

    Debug_Log("%s(): Not supported!\n");
    WH_CloseAll();
    exit(0);
}

void SWI_SoundBias(uint32_t level)
{
    // This function ignores the delay

    uint32_t value = level ? 0x100 : 0;

    REG_SOUNDBIAS &= ~SOUNDBIAS_BIAS_MASK;
    REG_SOUNDBIAS |= SOUNDBIAS_BIAS_LEVEL(value);
}
