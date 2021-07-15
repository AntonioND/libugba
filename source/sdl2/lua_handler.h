// SPDX-License-Identifier: LGPL-3.0-only
//
// Copyright (c) 2020 Antonio Niño Díaz

#ifndef SDL2_LUA_HANDLER_H__
#define SDL2_LUA_HANDLER_H__

#ifdef LUA_INTERPRETER_ENABLED

// Called by the game thread whenever a frame is drawn and handled
void Script_FrameDrawn(void);

// Run the script in the file pointed by path
int Script_RunLua(const char *path);

// Wait until the script is finished
void Script_WaitEnd(void);

#endif // LUA_INTERPRETER_ENABLED

#endif // SDL2_LUA_HANDLER_H__
