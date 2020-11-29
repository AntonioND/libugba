// SPDX-License-Identifier: MIT
//
// Copyright (c) 2020 Antonio Niño Díaz

#ifndef DISPLAY_H__
#define DISPLAY_H__

#include "hardware.h"

// Set background display mode without affecting the other flags of REG_DISPCNT.
EXPORT_API void DISPLAY_ModeSet(int mode);

#endif // DISPLAY_H__
