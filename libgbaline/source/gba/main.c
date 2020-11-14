// SPDX-License-Identifier: MIT
//
// Copyright (c) 2020 Antonio Niño Díaz

#include <gbaline.h>

// Defined in the application that links this library
extern int GBA_main(int argc, char *argv[]);

int main(int argc, char *argv[])
{
    // Library initialization

    IRQ_Init();

    return GBA_main(argc, argv);
}
