// SPDX-License-Identifier: LGPL-3.0-only
//
// Copyright (c) 2011-2015, 2019-2020 Antonio Niño Díaz

#include <SDL2/SDL.h>

#include "win_main.h"
#include "window_handler.h"

#include "../debug_utils.h"
#include "../general_utils.h"
#include "../core/video.h"

//------------------------------------------------------------------

static int WinIDMain = -1;

//------------------------------------------------------------------

static int WinMain_FPS;
static int WinMain_frames_drawn = 0;
static SDL_TimerID WinMain_FPS_timer;

static Uint32 _fps_callback_function(Uint32 interval, unused__ void *param)
{
    WinMain_FPS = WinMain_frames_drawn;
    WinMain_frames_drawn = 0;

    char caption[60];
    snprintf(caption, sizeof(caption),
             "libgbaine: %d fps - %.2f%%", WinMain_FPS,
             (float)WinMain_FPS * 10.0f / 6.0f);

    WH_SetCaption(WinIDMain, caption);

    return interval;
}

static void FPS_TimerInit(void)
{
    WinMain_FPS_timer = SDL_AddTimer(1000, _fps_callback_function, NULL);
    if (WinMain_FPS_timer == 0)
    {
        Debug_Log("%s(): SDL_AddTimer() failed: %s", __func__, SDL_GetError());
    }
}

static void FPS_TimerEnd(void)
{
    SDL_RemoveTimer(WinMain_FPS_timer);
}

//------------------------------------------------------------------

static void ScaleImage24RGB(int zoom, unsigned char *srcbuf, int srcw, int srch,
                            unsigned char *dstbuf, int dstw, int dsth)
{
    int dest_x_offset = (dstw - (srcw * zoom)) / 2;
    int dest_y_offset = (dsth - (srch * zoom)) / 2;

    int dest_x_end = dest_x_offset + srcw * zoom;
    int dest_y_end = dest_y_offset + srch * zoom;

    int srcx = 0;
    int srcy = 0;

    int srcx_inc_count = 0;
    int srcy_inc_count = 0;

    //int destx = dest_x_offset;
    int desty = dest_y_offset;

    for (; desty < dest_y_end; desty++)
    {
        srcx = 0;
        int destx = dest_x_offset;
        unsigned char *dstline = &(dstbuf[(desty * dstw + destx) * 3]);
        unsigned char *srcline = &(srcbuf[(srcy * srcw + srcx) * 3]);

        for (; destx < dest_x_end; destx++)
        {
            *dstline++ = *(srcline + 0);
            *dstline++ = *(srcline + 1);
            *dstline++ = *(srcline + 2);

            srcx_inc_count++;
            if (srcx_inc_count == zoom)
            {
                srcline += 3;
                srcx_inc_count = 0;
                srcx++;
            }
        }

        srcy_inc_count++;
        if (srcy_inc_count == zoom)
        {
            srcy_inc_count = 0;
            srcy++;
        }
    }
}

//------------------------------------------------------------------

#define ZOOM_MAX 5

static int WIN_MAIN_CONFIG_ZOOM = 2;

static unsigned char GBA_SCREEN[240 * 160 * 3];
static unsigned char WIN_MAIN_GAME_SCREEN_BUFFER[240 * ZOOM_MAX *
                                                 160 * ZOOM_MAX * 3];

//------------------------------------------------------------------

static int Win_MainEventCallback(SDL_Event *e)
{
    int exit_program = 0;

    if (e->type == SDL_KEYDOWN)
    {
        switch (e->key.keysym.sym)
        {
            case SDLK_F12:
                //GBA_Screenshot();
                break;

            case SDLK_c:
                if (SDL_GetModState() & KMOD_CTRL)
                {
                    exit_program = 1;
                }
                break;

            case SDLK_ESCAPE:
                exit_program = 1;
                break;
        }
    }
    else if (e->type == SDL_WINDOWEVENT)
    {
        if (e->window.event == SDL_WINDOWEVENT_CLOSE)
        {
            exit_program = 1;
        }
    }

    if (exit_program)
    {
        WH_CloseAll();
        exit(0);
        return 1;
    }

    return 0;
}

int Win_MainCreate(void)
{
    // Default screen size is 2x
    WIN_MAIN_CONFIG_ZOOM = 2;

#if 0
    int newzoom = EmulatorConfig.screen_size;
    if ((newzoom > 0) && (newzoom <= ZOOM_MAX))
    {
        WIN_MAIN_CONFIG_ZOOM = newzoom;
    }
#endif

    WinIDMain = WH_Create(240 * WIN_MAIN_CONFIG_ZOOM,
                          160 * WIN_MAIN_CONFIG_ZOOM, 0, 0, 0);
    if (WinIDMain == -1)
    {
        Debug_Log("%s(): Window could not be created!", __func__);
        return 1;
    }

    WH_SetCaption(WinIDMain, "libgbaine");

    WH_SetEventCallback(WinIDMain, Win_MainEventCallback);
    WH_SetEventMainWindow(WinIDMain);

    FPS_TimerInit();
    atexit(FPS_TimerEnd);

    //Sound_SetCallback(GBA_SoundCallback);

    return 0;
}

void Win_MainRender(void)
{
    WH_Render(WinIDMain, WIN_MAIN_GAME_SCREEN_BUFFER);
}

void Win_MainLoopHandle(void)
{
#if 0
    if (WH_HasKeyboardFocus(WinIDMain))
    {
        Sound_Enable();

        if (speedup)
            GBA_SoundResetBufferPointers();

        Input_Update_GBA();
#endif
        GBA_ConvertScreenBufferTo24RGB(GBA_SCREEN);

        //GBA_SCREEN[(240 * 20 + 20) * 3] = 0xFF;
        //GBA_SCREEN[(240 * 140 + 220) * 3] = 0xFF;
        //GBA_SCREEN[239 * 159 * 3] = 0xFF;

        ScaleImage24RGB(WIN_MAIN_CONFIG_ZOOM, GBA_SCREEN,
                        240, 160, WIN_MAIN_GAME_SCREEN_BUFFER,
                        240 * WIN_MAIN_CONFIG_ZOOM,
                        160 * WIN_MAIN_CONFIG_ZOOM);

        WinMain_frames_drawn++;
#if 0
    }
    else
    {
        //Sound_Disable();
    }
#endif
}
