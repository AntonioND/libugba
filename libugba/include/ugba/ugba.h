// SPDX-License-Identifier: MIT
//
// Copyright (c) 2020 Antonio Niño Díaz

#ifndef UGBA_H__
#define UGBA_H__

#include "background.h"
#include "bios.h"
#include "bios_wrappers.h"
#include "console.h"
#include "dma.h"
#include "display.h"
#include "debug.h"
#include "definitions.h"
#include "hardware.h"
#include "input.h"
#include "interrupts.h"
#include "obj.h"
#include "timer.h"
#include "vram.h"

// This function needs to be called at the start of main()
EXPORT_API void UGBA_Init(int *argc, char **argv[]);

#endif // UGBA_H__
