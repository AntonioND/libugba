// SPDX-License-Identifier: LGPL-3.0-only
//
// Copyright (c) 2011-2015, 2019-2020 Antonio Niño Díaz

#include <stdlib.h>

#include <SDL2/SDL.h>

#include <gbaline.h>

#include "debug_utils.h"
#include "input_utils.h"

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

static int autotest_enabled = 0;

int Debug_Autotest(void)
{
    return autotest_enabled;
}

// Defined in the application that links this library
extern int GBA_main(int argc, char *argv[]);

int main(int argc, char *argv[])
{
    // SDL2 port initialization

    Debug_Init();
    atexit(Debug_End);

    if (Init() != 0)
        return 1;

    Win_MainCreate();

    GBA_FillFadeTables();

    // Detect arguments

    if (argc > 1)
    {
        if (strcmp(argv[1], "--autotest") == 0)
        {
             autotest_enabled = 1;

             // Remove argv[1]

             for (int i = 1; i < argc - 1; i++)
                 argv[i] = argv[i + 1];

             argc--;
        }
    }

    // Library initialization

    IRQ_Init();

    return GBA_main(argc, argv);
}
