// SPDX-License-Identifier: MIT
//
// Copyright (c) 2020-2021 Antonio Niño Díaz

#include <ugba/ugba.h>

void UGBA_Init(int *argc, char **argv[])
{
    // Library initialization

    IRQ_Init();

    REG_WAITCNT = WAITCNT_DEFAULT_STARTUP;
}
