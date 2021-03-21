// SPDX-License-Identifier: LGPL-3.0-only
//
// Copyright (c) 2011-2015, 2019-2021 Antonio Niño Díaz

#include <stdlib.h>

#include <SDL2/SDL.h>

#include <ugba/ugba.h>

#include "debug_utils.h"
#include "input_utils.h"
#include "lua_handler.h"
#include "save_file.h"
#include "sound_utils.h"

#include "core/sound.h"
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
        Debug_Log("SDL could not initialize! SDL Error: %s", SDL_GetError());
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
        Debug_Log("SDL could not initialize! SDL Error: %s", SDL_GetError());
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
        // Use argv[0] to find the saved data
        if (*argc > 0)
        {
            size_t len = strlen(*argv[0]);

            char *sav_path = malloc(len + 1 + strlen(".sav"));

            if (sav_path == NULL)
            {
                Debug_Log("%s: Cant't allocate memory.", __func__);
            }
            else
            {
                snprintf(sav_path, len + 1, "%s", *argv[0]);

                size_t dot_pos = len;
                while (dot_pos > 0)
                {
                    char c = sav_path[dot_pos];

                    if ((c == '.') || (c == '\\') || (c == '/'))
                        break;

                    dot_pos--;
                }

                if (sav_path[dot_pos] != '.')
                {
                    dot_pos = len;
                }

                sav_path[dot_pos + 0] = '.';
                sav_path[dot_pos + 1] = 's';
                sav_path[dot_pos + 2] = 'a';
                sav_path[dot_pos + 3] = 'v';
                sav_path[dot_pos + 4] = '\0';

                UGBA_SaveFileOpen(sav_path);

                free(sav_path);
            }
        }

        if (*argc > 2)
        {
            if (strcmp((*argv)[1], "--lua") == 0)
            {
#ifdef LUA_INTERPRETER_ENABLED
                Script_RunLua((*argv)[2]);
#else
                Debug_Log("UGBA compiled without Lua support.");
#endif

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

    // Initialize hardware status

    Sound_MemWaveRamInitialize();

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

    // Initialize hardware status

    Sound_MemWaveRamInitialize();

    // Detect arguments

    UGBA_ParseArgs(argc, argv);

    Input_Update_GBA();

    // Library initialization

    IRQ_Init();

    REG_WAITCNT = WAITCNT_DEFAULT_STARTUP;
}
