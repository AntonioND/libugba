// SPDX-License-Identifier: MIT
//
// Copyright (c) 2020 Antonio Niño Díaz

#ifndef BIOS_H__
#define BIOS_H__

#include "definitions.h"

// Names taken from GBATEK

EXPORT_API void SWI_Halt(void);
EXPORT_API void SWI_VBlankIntrWait(void);

#endif // BIOS_H__
