// SPDX-License-Identifier: LGPL-3.0-only
//
// Copyright (c) 2020 Antonio Niño Díaz

#include <SDL2/SDL.h>

#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>

#include <gbaline.h>

#include "debug_utils.h"

#include "gui/win_main.h"

static char *script_path = NULL;
static SDL_Thread *script_thread;
static int script_running = 0;

// ----------------------------------------------------------------------------

static volatile int remaining_frames = 0;
static volatile int is_waiting = 0;

// Called by the game thread. If the script never calls ``continue()`` this can
// block
void Script_FrameDrawn(void)
{
    if (script_running == 0)
        return;

    if (remaining_frames == 0)
        return;

    remaining_frames--;

    if (remaining_frames == 0)
    {
        // Wait here until the script thread lets the game thread continue
        is_waiting = 1;

        int i = 0;
        while (is_waiting)
        {
            SDL_Delay(1);
            i++;
            if (i == 2000)
            {
                // Exit after two seconds of inactivity in case the script
                // forgot to finish the wait period.
                Debug_Log("%s: Timeout", __func__);
                break;
            }
        }
    }
}

// ----------------------------------------------------------------------------

static int lua_run_frames_and_pause(lua_State *L)
{
    // Number of arguments
    int narg = lua_gettop(L);
    if (narg != 1)
    {
        Debug_Log("%s(): Invalid number of arguments: %d", __func__, narg);
        return 0;
    }

    // Get argument of the function and remove it from the stack
    lua_Integer y = lua_tointeger(L, -1);
    lua_pop(L, 1);

    Debug_Log("%s(%lld)", __func__, y);

    remaining_frames = y;

    while (remaining_frames > 0)
        SDL_Delay(0);

    // Number of results
    return 0;
}

static int lua_continue(lua_State *L)
{
    // Number of arguments
    int narg = lua_gettop(L);
    if (narg != 0)
    {
        Debug_Log("%s(): Invalid number of arguments: %d", __func__, narg);
        return 0;
    }

    Debug_Log("%s()", __func__);

    is_waiting = 0;

    // Number of results
    return 0;
}

static int lua_screenshot(lua_State *L)
{
    // Number of arguments
    int narg = lua_gettop(L);
    if (narg == 0)
    {
        Debug_Log("%s()", __func__);
        Debug_Screenshot(NULL);
    }
    else if (narg == 1)
    {
        const char *name = lua_tostring(L, -1);

        Debug_Log("%s(%s)", __func__, name);
        Debug_Screenshot(name);

        lua_pop(L, 1);
    }
    else
    {
        Debug_Log("%s(): Invalid number of arguments: %d", __func__, narg);
        return 0;
    }

    // Number of results
    return 0;
}

uint16_t get_bit_from_key_name(const char *name)
{
    struct {
        const char *name;
        uint16_t mask;
    } keyinfo[10] = {
        { "A", KEY_A },
        { "B", KEY_B },
        { "SELECT", KEY_SELECT },
        { "START", KEY_START },
        { "RIGHT", KEY_RIGHT },
        { "LEFT", KEY_LEFT },
        { "UP", KEY_UP },
        { "DOWN", KEY_DOWN },
        { "R", KEY_R },
        { "L", KEY_L },
    };

    for (int i = 0; i < 10; i++)
    {
        if (strcmp(keyinfo[i].name, name) == 0)
            return keyinfo[i].mask;
    }

    Debug_Log("%s: Unknown name: %s", __func__, name);
    return 0;
}

static int lua_set_input(lua_State *L)
{
    // Number of arguments
    int narg = lua_gettop(L);

    Debug_Log("%s()", __func__);

    uint16_t pressed = 0;

    for (int i = 0; i < narg; i++)
    {
        const char *name = lua_tostring(L, -1);

        Debug_Log("    %s", name);

        pressed |= get_bit_from_key_name(name);

        lua_pop(L, 1);
    }

    REG_KEYINPUT = ~pressed;

    // Number of results
    return 0;
}

static int lua_exit(lua_State *L)
{
    // Number of arguments
    int narg = lua_gettop(L);
    if (narg != 0)
    {
        Debug_Log("%s(): Invalid number of arguments: %d", __func__, narg);
        return 0;
    }

    Debug_Log("%s()", __func__);

    Win_MainExit();

    // Number of results
    return 0;
}

// ----------------------------------------------------------------------------

static int Script_Runner(void unused__ *ptr)
{
    int ret = 1;

    // Create Lua state
    lua_State *L = luaL_newstate();

    // Load Lua libraries
    luaL_openlibs(L);

    // Load script from file
    int status = luaL_loadfile(L, script_path);
    if (status) {
        // On error, the error message is at the top of the stack
        fprintf(stderr, "Couldn't load file: %s", lua_tostring(L, -1));
        goto exit;
    }

    // Register C functions
    lua_register(L, "run_frames_and_pause", lua_run_frames_and_pause);
    lua_register(L, "continue", lua_continue);
    lua_register(L, "screenshot", lua_screenshot);
    lua_register(L, "set_input", lua_set_input);
    lua_register(L, "exit", lua_exit);

    // Run script with 0 arguments and expect one return value
    int result = lua_pcall(L, 0, 1, 0);
    if (result) {
        fprintf(stderr, "Failed to run script: %s", lua_tostring(L, -1));
        goto exit;
    }

    // Get returned value and remove it from the stack (it's at the top)
    int retval  = lua_tointeger(L, -1);
    lua_pop(L, 1);

    Debug_Log("Script returned: %d", retval);

    // Close Lua
    lua_close(L);

    ret = 0;
exit:
    return ret;
}

int Script_RunLua(const char *path)
{
    if (path == NULL)
        return 1;

    size_t len = strlen(path);

    script_path = malloc(len + 1);
    if (script_path == NULL)
        return 1;

    snprintf(script_path, len + 1, "%s", path);

    script_thread = SDL_CreateThread(Script_Runner, "Script Runner", NULL);
    if (script_thread == NULL)
    {
        Debug_Log("SDL_CreateThread failed: %s", SDL_GetError());
        return 1;
    }

    script_running = 1;

    return 0;
}

void Script_WaitEnd(void)
{
    int return_status;

    SDL_WaitThread(script_thread, &return_status);

    if (return_status != 0)
        Debug_Log("%s: Thread returned with: %d", __func__, return_status);

    free(script_path);

    script_running = 0;
}
