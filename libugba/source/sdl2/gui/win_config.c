// SPDX-License-Identifier: LGPL-3.0-only
//
// Copyright (c) 2021, Antonio Niño Díaz

#include <SDL2/SDL.h>

#include "../config.h"
#include "../debug_utils.h"

#include "font_utils.h"
#include "win_main.h"
#include "win_utils.h"

//------------------------------------------------------------------------------

#define CONFIG_COLS         (33)
#define CONFIG_ROWS         (12)

#define WIN_CONFIG_WIDTH    ((CONFIG_COLS * FONT_WIDTH) + 2)
#define WIN_CONFIG_HEIGHT   ((CONFIG_ROWS * FONT_HEIGHT) + 2)

#define MARGIN_WIDTH        ((240 - WIN_CONFIG_WIDTH) / 2)
#define MARGIN_HEIGHT       ((160 - WIN_CONFIG_HEIGHT) / 2)

static unsigned char config_buffer[WIN_CONFIG_WIDTH * WIN_CONFIG_HEIGHT * 3];

//------------------------------------------------------------------------------

static gui_console config_con;
static gui_element config_textbox;

static gui_element *config_window_gui_elements[] = {
    &config_textbox,
    NULL
};

static gui_global config_window_gui = {
    config_window_gui_elements,
    NULL,
    NULL
};

//----------------------------------------------------------------

#define SELECTION_ZOOM              (0)
#define SELECTION_SOUND_ENABLE      (1)
#define SELECTION_SOUND_VOLUME      (2)
#define SELECTION_SOUND_CHANNELS    (3)
#define SELECTION_INPUT_MENU        (4)

static int config_selected_item = 0;

static void Win_ConfigUpdate(void)
{
    GUI_ConsoleClear(&config_con);

    GUI_ConsoleModePrintf(&config_con, 20, 0, "Configuration");
    GUI_ConsoleModePrintf(&config_con, 0, 1,
                          "  Zoom: x%d\n"
                          "\n"
                          "  Sound enabled: ON\n"
                          "\n"
                          "  Volume: 64\n"
                          "\n"
                          "  Sound channels:\n"
                          "    PSG  1[ ]  2[ ]  3[ ]  4[ ]\n"
                          "    DMA  A[ ]  B[ ]\n"
                          "\n"
                          "  Input configuration menu\n",
                          GlobalConfig.screen_size);

    const int selection_offset_y[5] = { 1, 3, 5, 7, 11 };

    GUI_ConsoleModePrintf(&config_con,
                          1, selection_offset_y[config_selected_item],
                          STR_ARROW_LEFT);
}

void Win_ConfigEventCallback(SDL_Event *e)
{
    if (e->type == SDL_KEYDOWN)
    {
        switch (e->key.keysym.sym)
        {
            case SDLK_UP:
                if (config_selected_item > 0)
                    config_selected_item--;
                break;

            case SDLK_DOWN:
                if (config_selected_item < 4)
                    config_selected_item++;
                break;

            case SDLK_RIGHT:
            {
                switch (config_selected_item)
                {
                    case SELECTION_ZOOM:
                        if (GlobalConfig.screen_size < 5)
                        {
                            GlobalConfig.screen_size++;
                            Win_MainSetZoom(GlobalConfig.screen_size);
                        }
                        break;
                    case SELECTION_SOUND_ENABLE:
                        break;
                    case SELECTION_SOUND_VOLUME:
                        break;
                    case SELECTION_SOUND_CHANNELS:
                        break;
                    case SELECTION_INPUT_MENU:
                        break;
                    default:
                        break;
                }
                break;
            }
            case SDLK_LEFT:
            {
                switch (config_selected_item)
                {
                    case SELECTION_ZOOM:
                        if (GlobalConfig.screen_size > 1)
                        {
                            GlobalConfig.screen_size--;
                            Win_MainSetZoom(GlobalConfig.screen_size);
                        }
                        break;
                    case SELECTION_SOUND_ENABLE:
                        break;
                    case SELECTION_SOUND_VOLUME:
                        break;
                    case SELECTION_SOUND_CHANNELS:
                        break;
                    case SELECTION_INPUT_MENU:
                        break;
                    default:
                        break;
                }
                break;
            }

            default:
                break;
        }
    }
}

void Win_ConfigDrawOverlay(unsigned char *dest_buffer)
{
    Win_ConfigUpdate();

    GUI_Draw(&config_window_gui, config_buffer,
             WIN_CONFIG_WIDTH, WIN_CONFIG_HEIGHT, 1);

    for (int j = 0; j < WIN_CONFIG_HEIGHT; j++)
    {
        for (int i = 0; i < WIN_CONFIG_WIDTH; i++)
        {
            int dstx = i + MARGIN_WIDTH;
            int dsty = j + MARGIN_HEIGHT;

            int dst_index = (dsty * 240 + dstx) * 3;
            int src_index = (j * WIN_CONFIG_WIDTH + i) * 3;

            for (int c = 0; c < 3; c++)
            {
                int val = dest_buffer[dst_index + c] * 8 +
                          config_buffer[src_index + c] * 120;

                dest_buffer[dst_index + c] = val / 128;
            }
        }
    }
}

int Win_ConfigCreate(void)
{
    GUI_SetTextBox(&config_textbox, &config_con,
                   1, 1,
                   CONFIG_COLS * FONT_WIDTH, CONFIG_ROWS * FONT_HEIGHT,
                   NULL);

    return 1;
}
