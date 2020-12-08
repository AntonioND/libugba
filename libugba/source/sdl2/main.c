// SPDX-License-Identifier: LGPL-3.0-only
//
// Copyright (c) 2011-2015, 2019-2020 Antonio Niño Díaz

#include <stdlib.h>

#include <SDL2/SDL.h>

#include <ugba/ugba.h>

#include "debug_utils.h"
#include "input_utils.h"
#include "lua_handler.h"

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

    //Config_Load();

    //Sound_Init();

    return 0;
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

    Input_Update_GBA();

    // Library initialization

    IRQ_Init();
}
