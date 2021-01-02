// SPDX-License-Identifier: MIT
//
// Copyright (c) 2020 Antonio Niño Díaz

// Example that shows how to identify the device type based on the BIOS checksum

#include <inttypes.h>
#include <stdio.h>

#include <ugba/ugba.h>

int main(int argc, char *argv[])
{
    UGBA_Init(&argc, &argv);

    IRQ_Enable(IRQ_VBLANK);

    DISP_ModeSet(0);

    CON_InitDefault();

    const char *type = "Unknown";

    uint32_t sum = SWI_GetBiosChecksum();
    if (sum == SWI_CHECKSUM_GBA)
        type = "GBA, GBA SP, GB Micro";
    else if (sum == SWI_CHECKSUM_NDS)
        type = "NDS, 3DS in NDS mode";

    char text[100];
    snprintf(text, sizeof(text), "Checksum: 0x%08" PRIX32 "\nType: %s",
             sum, type);
    CON_Print(text);

    while (1)
        SWI_VBlankIntrWait();
}

