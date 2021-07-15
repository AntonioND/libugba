// SPDX-License-Identifier: LGPL-3.0-only
//
// Copyright (c) 2011-2015, 2019-2020 Antonio Niño Díaz

#include <stdlib.h>
#include <string.h>

#include <SDL2/SDL.h>

#include <ugba/ugba.h>

#include "gui/win_main.h"

#include "debug_utils.h"
#include "input_utils.h"

//------------------------------------------------------------------------------

typedef struct
{
    int is_opened;
    SDL_Joystick *joystick;
    char name[50]; // Should be more than enough to identify a joystick
} joystick_info;

#define MAX_JOYSTICKS 5
static joystick_info Joystick[MAX_JOYSTICKS];
static int joystick_number;

//------------------------------------------------------------------------------

// Use keyboard by default
static int controler_index = -1;

// This is the default configuration, changed when loading the config file (if
// it exists). Each value can be a SDLK_* define or a joystick button number. By
// default, use the keyboard.
static SDL_KeyCode player_key[P_NUM_KEYS] = {
    // A, B, L, R, UP, RIGHT, DOWN, LEFT, START, SELECT,
    SDLK_x, SDLK_z, SDLK_a, SDLK_s,
    SDLK_UP, SDLK_RIGHT, SDLK_DOWN, SDLK_LEFT,
    SDLK_RETURN, SDLK_RSHIFT
};

static SDL_KeyCode player_key_speedup = SDLK_SPACE;

const char *GBKeyNames[P_NUM_KEYS] = {
    "A", "B", "L", "R", "Up", "Right", "Down", "Left", "Start", "Select"
};

//------------------------------------------------------------------------------

// -1 = keyboard, others = number of joystick
void Input_PlayerSetController(int index)
{
    controler_index = index;
}

int Input_PlayerGetController(void)
{
    return controler_index;
}

//----------------------------

void Input_ControlsSetKey(key_config_enum keyindex, SDL_KeyCode keyscancode)
{
    if ((keyindex == P_KEY_NONE) || (keyindex == P_NUM_KEYS))
        return;

    if (keyindex == P_KEY_SPEEDUP)
        player_key_speedup = keyscancode;
    else
        player_key[keyindex] = keyscancode;
}

SDL_KeyCode Input_ControlsGetKey(key_config_enum keyindex)
{
    if ((keyindex == P_KEY_NONE) || (keyindex == P_NUM_KEYS))
        return 0;

    if (keyindex == P_KEY_SPEEDUP)
        return player_key_speedup;

    return player_key[keyindex];
}

//------------------------------------------------------------------------------

int Input_IsGameBoyKeyPressed(key_config_enum keyindex)
{
    if (controler_index == -1) // Keyboard
    {
        const Uint8 *state = SDL_GetKeyboardState(NULL);
        int key = Input_ControlsGetKey(keyindex);
        if (key == 0)
            return 0;

        SDL_KeyCode code = Input_ControlsGetKey(keyindex);
        SDL_Scancode scancode = SDL_GetScancodeFromKey(code);
        return state[scancode];
    }
    else // Joystick
    {
        int joystick_index = controler_index;
        int btn = Input_ControlsGetKey(keyindex);
        SDL_Joystick *joystick = Joystick[joystick_index].joystick;

        if (btn & KEYCODE_IS_AXIS) // Axis
        {
            btn &= ~KEYCODE_IS_AXIS; // Remove axis flag

            int is_btn_positive = (btn & KEYCODE_POSITIVE_AXIS);
            btn &= ~KEYCODE_POSITIVE_AXIS; // Remove positive flag

            int axis_value = SDL_JoystickGetAxis(joystick, btn);

            if (is_btn_positive && (axis_value > (16 * 1024)))
                return 1;
            else if ((!is_btn_positive) && (axis_value < (-16 * 1024)))
                return 1;
            else
                return 0;
        }
        else if (btn & KEYCODE_IS_HAT) // Axis
        {
            btn &= ~KEYCODE_IS_HAT; // Remove hat flag

            int position = (btn >> 4);
            int hat_index = btn & 0xF;

            if (position & SDL_JoystickGetHat(joystick, hat_index))
                return 1;
            else
                return 0;
        }
        else // Button
        {
            if (btn != -1)
                return SDL_JoystickGetButton(joystick, btn);
            else
                return 0;
        }
    }

    return 0;
}

void Input_GetKeyboardElementName(char *name, int namelen, int btncode)
{
    if (btncode <= 0)
        snprintf(name, namelen, "None");
    else
        snprintf(name, namelen, "%s", SDL_GetKeyName(btncode));
}

void Input_GetJoystickElementName(char *name, int namelen, int btncode)
{
    if (btncode == -1)
    {
        snprintf(name, namelen, "None");
    }
    else if (btncode & KEYCODE_IS_AXIS) // Axis
    {
        btncode &= ~KEYCODE_IS_AXIS; // Remove axis flag

        int is_btn_positive = (btncode & KEYCODE_POSITIVE_AXIS);
        btncode &= ~KEYCODE_POSITIVE_AXIS; // Remove positive flag

        snprintf(name, namelen, "Axis %d%c", btncode,
                 is_btn_positive ? '+' : '-');
    }
    else if (btncode & KEYCODE_IS_HAT) // Axis
    {
        btncode &= ~KEYCODE_IS_HAT; // Remove hat flag

        int position = (btncode >> 4);
        int hat_index = btncode & 0xF;

        char *dir = "Centered [!]";
        if (position & SDL_HAT_UP)
            dir = "Up";
        else if (position & SDL_HAT_RIGHT)
            dir = "Right";
        else if (position & SDL_HAT_DOWN)
            dir = "Down";
        else if (position & SDL_HAT_LEFT)
            dir = "Left";

        snprintf(name, namelen, "Hat %d %s", hat_index, dir);
    }
    else // Button
    {
        snprintf(name, namelen, "Button %d", btncode);
    }
}

//------------------------------------------------------------------------------

static void GBA_HandleInput(int a, int b, int l, int r, int st, int se,
                            int dr, int dl, int du, int dd)
{
    uint16_t input = 0;

    if (a == 0)
        input |= BIT(0);
    if (b == 0)
        input |= BIT(1);
    if (se == 0)
        input |= BIT(2);
    if (st == 0)
        input |= BIT(3);
    if (dr == 0)
        input |= BIT(4);
    if (dl == 0)
        input |= BIT(5);
    if (du == 0)
        input |= BIT(6);
    if (dd == 0)
        input |= BIT(7);
    if (r == 0)
        input |= BIT(8);
    if (l == 0)
        input |= BIT(9);

    REG_KEYINPUT = input;
}

void Input_Update_GBA(void)
{
    if (Win_MainIsConfigEnabled())
    {
        GBA_HandleInput(0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
    }
    else
    {
        int a = Input_IsGameBoyKeyPressed(P_KEY_A);
        int b = Input_IsGameBoyKeyPressed(P_KEY_B);
        int l = Input_IsGameBoyKeyPressed(P_KEY_L);
        int r = Input_IsGameBoyKeyPressed(P_KEY_R);
        int st = Input_IsGameBoyKeyPressed(P_KEY_START);
        int se = Input_IsGameBoyKeyPressed(P_KEY_SELECT);
        int dr = Input_IsGameBoyKeyPressed(P_KEY_RIGHT);
        int dl = Input_IsGameBoyKeyPressed(P_KEY_LEFT);
        int du = Input_IsGameBoyKeyPressed(P_KEY_UP);
        int dd = Input_IsGameBoyKeyPressed(P_KEY_DOWN);

        GBA_HandleInput(a, b, l, r, st, se, dr, dl, du, dd);
    }
}

int Input_Speedup_Enabled(void)
{
    const Uint8 *state = SDL_GetKeyboardState(NULL);

    return state[SDL_SCANCODE_SPACE];
}

//------------------------------------------------------------------------------

SDL_Joystick *Input_GetJoystick(int index)
{
    if ((index < 0) || (index >= MAX_JOYSTICKS))
        return NULL;

    return Joystick[index].joystick;
}

char *Input_GetJoystickName(int index)
{
    if ((index < 0) || (index >= MAX_JOYSTICKS))
        return NULL;

    return Joystick[index].name;
}

int Input_GetJoystickFromName(char *name)
{
    for (int i = 0; i < MAX_JOYSTICKS; i++)
    {
        if (strncmp(name, Joystick[i].name, sizeof(Joystick[i].name)) == 0)
            return i;
    }

    if (strncmp(name, "Keyboard", strlen("Keyboard")) == 0)
        return -1;

    return -2;
}

static void Input_CloseSystem(void)
{
    for (int i = 0; i < MAX_JOYSTICKS; i++)
    {
        if (!Joystick[i].is_opened)
            continue;

        Joystick[i].is_opened = 0;
        SDL_JoystickClose(Joystick[i].joystick);
        Joystick[i].joystick = NULL;
    }
}

int Input_GetJoystickNumber(void)
{
    return joystick_number;
}

void Input_InitSystem(void)
{
    atexit(Input_CloseSystem);

    // Clear things
    for (int i = 0; i < MAX_JOYSTICKS; i++)
    {
        Joystick[i].is_opened = 0;
        Joystick[i].joystick = NULL;
    }

    joystick_number = SDL_NumJoysticks();

    for (int i = 0; i < joystick_number; i++) // Load joysticks
    {
        Joystick[i].joystick = SDL_JoystickOpen(i);
        Joystick[i].is_opened = (Joystick[i].joystick != NULL);

        const char *name = SDL_JoystickNameForIndex(i);

        if (name)
            snprintf(Joystick[i].name, sizeof(Joystick[i].name), "%s", name);
        else
            snprintf(Joystick[i].name, sizeof(Joystick[i].name),
                     "Unknown Joystick");
    }
}
