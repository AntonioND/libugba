// SPDX-License-Identifier: MIT
//
// Copyright (c) 2020 Antonio Niño Díaz

#ifndef CONSOLE_H__
#define CONSOLE_H__

#include "definitions.h"

EXPORT_API void CON_InitDefault(void);
EXPORT_API void CON_PutChar(char c);
EXPORT_API void CON_Print(const char *string);
EXPORT_API void CON_CursorSet(int x, int y);
EXPORT_API void CON_Clear(void);

#endif // CONSOLE_H__

