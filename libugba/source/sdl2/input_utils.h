// SPDX-License-Identifier: LGPL-3.0-only
//
// Copyright (c) 2011-2015, 2019-2020 Antonio Niño Díaz

#ifndef SDL2_INPUT_UTILS_H__
#define SDL2_INPUT_UTILS_H__

#include <SDL2/SDL.h>

typedef enum
{
    P_KEY_A,
    P_KEY_B,
    P_KEY_L,
    P_KEY_R,
    P_KEY_UP,
    P_KEY_RIGHT,
    P_KEY_DOWN,
    P_KEY_LEFT,
    P_KEY_START,
    P_KEY_SELECT,

    P_NUM_KEYS,

    P_KEY_SPEEDUP,

    P_KEY_NONE
} key_config_enum;

#define KEYCODE_POSITIVE_AXIS (1 << 8)
#define KEYCODE_IS_AXIS       (1 << 9)
#define KEYCODE_IS_HAT        (1 << 10)

extern const char *GBKeyNames[P_NUM_KEYS];

void Input_ControlsSetKey(key_config_enum keyindex, SDL_KeyCode keyscancode);
SDL_KeyCode Input_ControlsGetKey(key_config_enum keyindex);

// The index is the number of the joystick. The keyboard is -1.
void Input_PlayerSetController(int index);
int Input_PlayerGetController(void);

//------------------------------------------------------------------------------

void Input_Update_GBA(void);

int Input_Speedup_Enabled(void);

//-----------------------------------------------------------------------------

// btncode is a SDL_KeyCode
void Input_GetKeyboardElementName(char *name, int namelen, int btncode);
void Input_GetJoystickElementName(char *name, int namelen, int btncode);

//------------------------------------------------------------------------------

SDL_Joystick *Input_GetJoystick(int index);
char *Input_GetJoystickName(int index);
int Input_GetJoystickNumber(void);

// Returns: -1 = keyboard, 0-3 = controller index, -2 = error
int Input_GetJoystickFromName(char *name);

// This function only works with joysticks that are connected when opening the
// emulator.
void Input_InitSystem(void);

#endif // SDL2_INPUT_UTILS_H__
