// SPDX-License-Identifier: LGPL-3.0-only
//
// Copyright (c) 2011-2015, 2019-2021 Antonio Niño Díaz

#include <time.h>

#include <SDL2/SDL.h>

#include <ugba/ugba.h>

#include "win_config.h"
#include "win_main.h"
#include "window_handler.h"

#include "../config.h"
#include "../debug_utils.h"
#include "../png_utils.h"
#include "../core/video.h"

#include "debugger/win_gba_debugger.h"

//------------------------------------------------------------------

static int WinIDMain = -1;

//------------------------------------------------------------------

static int current_fps, old_fps;
static int frames_drawn = 0;
static SDL_TimerID fps_timer;

static Uint32 fps_callback_function(Uint32 interval, UNUSED void *param)
{
    current_fps = frames_drawn;
    frames_drawn = 0;

    return interval;
}

static void FPS_TimerInit(void)
{
    fps_timer = SDL_AddTimer(1000, fps_callback_function, NULL);
    if (fps_timer == 0)
    {
        Debug_Log("%s(): SDL_AddTimer() failed: %s", __func__, SDL_GetError());
    }
}

static void FPS_TimerEnd(void)
{
    SDL_RemoveTimer(fps_timer);
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

#ifdef ENABLE_SCREENSHOTS

static char timestamp_filename[256];

static void GenerateNewScreenshotTimestampFilename(void)
{
    long long int number = 0;

    time_t rawtime;
    time(&rawtime);
    struct tm *ptm = gmtime(&rawtime);

    // Generate base file name based on the current time and date
    char timestamp[50];
    snprintf(timestamp, sizeof(timestamp), "%04d%02d%02d_%02d%02d%02d",
             1900 + ptm->tm_year, 1 + ptm->tm_mon, ptm->tm_mday,
             1 + ptm->tm_hour, ptm->tm_min, ptm->tm_sec);

    // Append a number to the name so that we can take multiple screenshots the
    // same second.
    while (1)
    {
        snprintf(timestamp_filename, sizeof(timestamp_filename),
                 "screenshot_%s_%lld.png", timestamp, number);

        FILE *file = fopen(timestamp_filename, "rb");
        if (file == NULL)
            break; // This name is available

        fclose(file);

        number++;
    }
}

void Debug_Screenshot(const char *name)
{
    if (name == NULL)
        name = "screenshot.png";

    Save_PNG(name, &GBA_SCREEN[0], 240, 160, 0);
}

#endif // ENABLE_SCREENSHOTS

//------------------------------------------------------------------

static int exit_program_requested = 0;

void Win_MainExit(void)
{
    exit_program_requested = 1;
}

static int config_shown = 0;

int Win_MainIsConfigEnabled(void)
{
    return config_shown;
}

static int Win_MainEventCallback(SDL_Event *e)
{
    int exit_program = 0;

    if (e->type == SDL_KEYDOWN)
    {
        switch (e->key.keysym.sym)
        {
            case SDLK_F3:
                if (config_shown == 0)
                {
                    Win_ConfigCreate();
                    config_shown = 1;
                }
                else
                {
                    Config_Save();
                    config_shown = 0;
                }
                break;

#ifdef ENABLE_DEBUGGER

            case SDLK_F5:
                Win_GBAIOViewerCreate();
                break;

            case SDLK_F6:
                Win_GBATileViewerCreate();
                break;

            case SDLK_F7:
                Win_GBAMapViewerCreate();
                break;

            case SDLK_F8:
                Win_GBASprViewerCreate();
                break;

            case SDLK_F9:
                Win_GBAPalViewerCreate();
                break;

#endif // ENABLE_DEBUGGER

#ifdef ENABLE_SCREENSHOTS

            case SDLK_F12:
                GenerateNewScreenshotTimestampFilename();
                Debug_Screenshot(timestamp_filename);
                break;

#endif // ENABLE_SCREENSHOTS

            case SDLK_ESCAPE:
                if (config_shown)
                {
                    if (Win_ConfigEventCallback(e))
                    {
                        Config_Save();
                        config_shown = 0;
                    }
                }
                else
                    exit_program = 1;
                break;

            default:
                if (config_shown)
                    Win_ConfigEventCallback(e);
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

    int newzoom = GlobalConfig.screen_size;

    if ((newzoom > 0) && (newzoom <= ZOOM_MAX))
    {
        WIN_MAIN_CONFIG_ZOOM = newzoom;
    }

    WinIDMain = WH_Create(240 * WIN_MAIN_CONFIG_ZOOM,
                          160 * WIN_MAIN_CONFIG_ZOOM, 0, 0, 0);
    if (WinIDMain == -1)
    {
        Debug_Log("%s(): Window could not be created!", __func__);
        return 1;
    }

    WH_SetCaption(WinIDMain, "ugba");

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

void Win_MainSetZoom(int factor)
{
    WIN_MAIN_CONFIG_ZOOM = factor;

    int w = 240 * WIN_MAIN_CONFIG_ZOOM;
    int h = 160 * WIN_MAIN_CONFIG_ZOOM;

    WH_SetSize(WinIDMain, w, h, w, h, 0);
}

void Win_MainLoopHandle(void)
{
    if (exit_program_requested)
    {
        WH_CloseAll();
        exit(0);
    }

    GBA_ConvertScreenBufferTo24RGB(GBA_SCREEN);

    if (config_shown)
        Win_ConfigDrawOverlay(GBA_SCREEN);

    ScaleImage24RGB(WIN_MAIN_CONFIG_ZOOM, GBA_SCREEN,
                    240, 160, WIN_MAIN_GAME_SCREEN_BUFFER,
                    240 * WIN_MAIN_CONFIG_ZOOM,
                    160 * WIN_MAIN_CONFIG_ZOOM);

    frames_drawn++;

    // Update window caption

    if (old_fps != current_fps)
    {
        char caption[60];
        snprintf(caption, sizeof(caption), "ugba: %d fps - %.2f%%",
                current_fps, (float)current_fps * 10.0f / 6.0f);

        WH_SetCaption(WinIDMain, caption);

        old_fps = current_fps;
    }

#ifdef ENABLE_DEBUGGER

    // Update debugger windows

    Win_GBAIOViewerUpdate();
    Win_GBAMapViewerUpdate();
    Win_GBATileViewerUpdate();
    Win_GBASprViewerUpdate();
    Win_GBAPalViewerUpdate();

#endif // ENABLE_DEBUGGER
}
