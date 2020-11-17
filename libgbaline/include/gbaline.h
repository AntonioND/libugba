// SPDX-License-Identifier: MIT
//
// Copyright (c) 2020 Antonio Niño Díaz

#ifndef GBALINE_H__
#define GBALINE_H__

#include "bios.h"
#include "debug.h"
#include "hardware.h"
#include "interrupts.h"
#include "obj.h"

// This function needs to be called at the start of main()
void GBALINE_Init(int *argc, char **argv[]);

#endif // GBALINE_H__
