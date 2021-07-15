// SPDX-License-Identifier: LGPL-3.0-only
//
// Copyright (c) 2021, Antonio Niño Díaz

#include <SDL2/SDL.h>

#include "../config.h"
#include "../debug_utils.h"
#include "../input_utils.h"

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
static int config_selected_sound_channel = 0;

static int config_input_menu_enabled = 0;
static int config_input_selected_item = 0;
static int config_input_waiting_for_press = 0;
static int config_input_waiting_blink_frames = 0;

static void Win_ConfigUpdate(void)
{
    GUI_ConsoleClear(&config_con);

    if (config_input_menu_enabled == 0)
    {
        GUI_ConsoleModePrintf(&config_con, 13, 0, "Global Configuration");

        GUI_ConsoleModePrintf(&config_con, 2, 1, "Zoom: x%d",
                              GlobalConfig.screen_size);

        GUI_ConsoleModePrintf(&config_con, 2, 3, "Sound enabled: %s",
                              GlobalConfig.sound_mute ? "OFF" : "ON");

        GUI_ConsoleModePrintf(&config_con, 2, 5, "Volume: %d",
                              GlobalConfig.volume);

        int channel[6] = { 0 };
        for (int i = 0; i < 6; i++)
        {
            int c = ' ';

            if (GlobalConfig.channel_flags & (1 << i))
                c = CHR_SQUAREBLACK_MID;

            channel[i] = c;
        }

        GUI_ConsoleModePrintf(&config_con, 0, 7,
                              "  Sound channels:\n"
                              "    PSG  1[%c]  2[%c]  3[%c]  4[%c]\n"
                              "    DMA  A[%c]  B[%c]",
                              channel[0], channel[1], channel[2], channel[3],
                              channel[4], channel[5]);

        GUI_ConsoleModePrintf(&config_con, 2, 11, "Input configuration menu");

        // Print cursor

        const int selection_offset_y[5] = { 1, 3, 5, 7, 11 };

        GUI_ConsoleModePrintf(&config_con,
                            1, selection_offset_y[config_selected_item],
                            STR_ARROW_LEFT);

        if (config_selected_item == SELECTION_SOUND_CHANNELS)
        {
            const int channel_x[6] = { 8, 14, 20, 26, 8, 14 };
            const int channel_y[6] = { 8, 8, 8, 8, 9, 9 };

            int x = channel_x[config_selected_sound_channel];
            int y = channel_y[config_selected_sound_channel];

            GUI_ConsoleModePrintf(&config_con, x, y, STR_ARROW_LEFT);
        }
    }
    else
    {
        GUI_ConsoleModePrintf(&config_con, 14, 0, "Input Configuration");

        int selected_controller = Input_PlayerGetController();
        const char *name = "Unknown";

        if (selected_controller == -1)
            name = "Keyboard";
        else if (selected_controller != -2)
            name = Input_GetJoystickName(selected_controller);

        char str[31];
        snprintf(str, sizeof(str), " < %s >", name);
        GUI_ConsoleModePrintf(&config_con, 2, 1, str);

        for (int i = 0; i < P_NUM_KEYS; i++)
        {
            char keyname[20];
            int btn = Input_ControlsGetKey(i);

            if (Input_PlayerGetController() == -1)
                Input_GetKeyboardElementName(keyname, sizeof(keyname), btn);
            else
                Input_GetJoystickElementName(keyname, sizeof(keyname), btn);

            GUI_ConsoleModePrintf(&config_con, 2, i + 2, "%s: %s",
                                  GBKeyNames[i], keyname);
        }

        int show_cursor = 1;

        if (config_input_waiting_for_press)
        {
            if ((config_input_waiting_blink_frames & 16) == 0)
                show_cursor = 0;
            config_input_waiting_blink_frames++;
        }

        if (show_cursor)
        {
            GUI_ConsoleModePrintf(&config_con,
                                  1, 1 + config_input_selected_item,
                                  STR_ARROW_LEFT);
        }
    }
}

static int GlobalConfigEventCallback(SDL_Event *e)
{
    if (e->type == SDL_KEYDOWN)
    {
        switch (e->key.keysym.sym)
        {
            case SDLK_ESCAPE:
                return 1;

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
                        GlobalConfig.sound_mute ^= 1;
                        break;
                    case SELECTION_SOUND_VOLUME:
                        if (GlobalConfig.volume < 100)
                            GlobalConfig.volume++;
                        break;
                    case SELECTION_SOUND_CHANNELS:
                        if (config_selected_sound_channel < 5)
                            config_selected_sound_channel++;
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
                        GlobalConfig.sound_mute ^= 1;
                        break;
                    case SELECTION_SOUND_VOLUME:
                        if (GlobalConfig.volume > 0)
                            GlobalConfig.volume--;
                        break;
                    case SELECTION_SOUND_CHANNELS:
                        if (config_selected_sound_channel > 0)
                            config_selected_sound_channel--;
                        break;
                    case SELECTION_INPUT_MENU:
                        break;
                    default:
                        break;
                }
                break;
            }
            case SDLK_RETURN:
            {
                switch (config_selected_item)
                {
                    case SELECTION_ZOOM:
                        break;
                    case SELECTION_SOUND_ENABLE:
                        GlobalConfig.sound_mute ^= 1;
                        break;
                    case SELECTION_SOUND_VOLUME:
                        break;
                    case SELECTION_SOUND_CHANNELS:
                    {
                        int flag = 1 << config_selected_sound_channel;
                        GlobalConfig.channel_flags ^= flag;
                        break;
                    }
                    case SELECTION_INPUT_MENU:
                        config_input_menu_enabled = 1;
                        config_input_selected_item = 0;
                        config_input_waiting_for_press = 0;
                        config_input_waiting_blink_frames = 0;
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

    return 0;
}

static int win_config_inputget_callback(SDL_Event *e)
{
    if (config_input_waiting_for_press == 0)
        return 0;

    int controller = Input_PlayerGetController();

    if (controller == -1)
    {
        // Keyboard
        if (e->type == SDL_KEYDOWN)
        {
            SDL_KeyCode scancode; // scancode name

            scancode = SDL_GetKeyFromScancode(e->key.keysym.scancode);

            Input_ControlsSetKey(config_input_selected_item - 1, scancode);

            config_input_waiting_for_press = 0;
        }
    }
    else // Joypad
    {
        if (e->type == SDL_JOYBUTTONDOWN)
        {
            if (e->jbutton.which == controller)
            {
                int btn = e->jbutton.button;

                Input_ControlsSetKey(config_input_selected_item - 1, btn);

                config_input_waiting_for_press = 0;
            }
        }
        else if (e->type == SDL_JOYAXISMOTION)
        {
            if (e->jbutton.which == controller)
            {
                int btn = KEYCODE_IS_AXIS
                          + ((e->jaxis.value > 0) ? KEYCODE_POSITIVE_AXIS : 0)
                          + e->jaxis.axis;

                Input_ControlsSetKey(config_input_selected_item - 1, btn);

                config_input_waiting_for_press = 0;
            }
        }
        else if (e->type == SDL_JOYHATMOTION)
        {
            if (e->jbutton.which == controller)
            {
                int btn = KEYCODE_IS_HAT + (e->jhat.value << 4) + e->jhat.hat;

                Input_ControlsSetKey(config_input_selected_item - 1, btn);

                config_input_waiting_for_press = 0;
            }
        }
    }

    return 0;
}


static int InputConfigEventCallback(SDL_Event *e)
{
    win_config_inputget_callback(e);

    if (e->type == SDL_KEYDOWN)
    {
        switch (e->key.keysym.sym)
        {
            case SDLK_ESCAPE:
                config_input_menu_enabled = 0;
                break;

            case SDLK_UP:
                if (config_input_waiting_for_press == 0)
                {
                    if (config_input_selected_item > 0)
                        config_input_selected_item--;
                }
                break;

            case SDLK_DOWN:
                if (config_input_waiting_for_press == 0)
                {
                    if (config_input_selected_item < 10)
                        config_input_selected_item++;
                }
                break;

            case SDLK_RIGHT:
                if (config_input_selected_item == 0)
                {
                    int current = Input_PlayerGetController();
                    int max = Input_GetJoystickNumber() - 1;
                    if (current < max)
                        Input_PlayerSetController(current + 1);
                }
                break;

            case SDLK_LEFT:
                if (config_input_selected_item == 0)
                {
                    int current = Input_PlayerGetController();
                    if (current > -1)
                        Input_PlayerSetController(current - 1);
                }
                break;

            case SDLK_RETURN:
                if (config_input_waiting_for_press == 0)
                {
                    if (config_input_selected_item > 0)
                    {
                        config_input_waiting_for_press = 1;
                        config_input_waiting_blink_frames = 0;
                    }
                }
                break;

            default:
                break;
        }
    }

    return 0;
}

int Win_ConfigEventCallback(SDL_Event *e)
{
    if (config_input_menu_enabled == 0)
        return GlobalConfigEventCallback(e);
    else
        return InputConfigEventCallback(e);
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
