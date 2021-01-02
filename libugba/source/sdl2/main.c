// SPDX-License-Identifier: LGPL-3.0-only
//
// Copyright (c) 2011-2015, 2019-2021 Antonio Niño Díaz

#include <stdlib.h>

#include <SDL2/SDL.h>

#include <ugba/ugba.h>

#include "debug_utils.h"
#include "input_utils.h"
#include "lua_handler.h"
#include "sound_utils.h"

#include "core/video.h"
#include "gui/win_main.h"
#include "gui/window_handler.h"

static int Init(void)
{
    WH_Init();

    // Initialize SDL
    if (SDL_Init(SDL_INIT_TIMER | SDL_INIT_AUDIO | SDL_INIT_VIDEO |
                 SDL_INIT_EVENTS) != 0)
    {
        Debug_Log("SDL could not initialize! SDL Error: %s\n", SDL_GetError());
        return 1;
    }
    atexit(SDL_Quit);

    // Init this before loading the configuration
    Input_InitSystem();

    // Initialize audio
    Sound_Init();

    return 0;
}

static int InitHeadless(void)
{
    // Initialize SDL
    if (SDL_Init(SDL_INIT_TIMER | SDL_INIT_EVENTS) != 0)
    {
        Debug_Log("SDL could not initialize! SDL Error: %s\n", SDL_GetError());
        return 1;
    }
    atexit(SDL_Quit);

    // Init this before loading the configuration
    Input_InitSystem();

    return 0;
}

static void UGBA_ParseArgs(int *argc, char **argv[])
{
    if ((argc != NULL) && (argv != NULL))
    {
        if (*argc > 2)
        {
            if (strcmp((*argv)[1], "--lua") == 0)
            {
                Script_RunLua((*argv)[2]);

                // Remove argv[1] and argv[2]

                for (int i = 1; i < *argc - 2; i++)
                    (*argv)[i] = (*argv)[i + 2];

                *argc = *argc - 2;
            }
        }
    }
}

void UGBA_Init(int *argc, char **argv[])
{
    // SDL2 port initialization

    Debug_Init();
    atexit(Debug_End);

    if (Init() != 0)
        exit(1);

    Win_MainCreate();

    GBA_FillFadeTables();

    // Detect arguments

    UGBA_ParseArgs(argc, argv);

    // Update key input state

    Input_Update_GBA();

    // Library initialization

    IRQ_Init();

    REG_WAITCNT = WAITCNT_DEFAULT_STARTUP;
}

void UGBA_InitHeadless(int *argc, char **argv[])
{
    // SDL2 port initialization

    Debug_Init();
    atexit(Debug_End);

    if (InitHeadless() != 0)
        exit(1);

    GBA_FillFadeTables();

    // Detect arguments

    UGBA_ParseArgs(argc, argv);

    Input_Update_GBA();

    // Library initialization

    IRQ_Init();

    REG_WAITCNT = WAITCNT_DEFAULT_STARTUP;
}
