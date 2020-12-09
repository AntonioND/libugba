// SPDX-License-Identifier: MIT
//
// Copyright (c) 2020 Antonio Niño Díaz

// Test that checks that the emulated BIOS function SoundBias() is working as
// expected on real hardware.

#include <ugba/ugba.h>

uint32_t get_bias_level(void)
{
    return (REG_SOUNDBIAS & 0x3FE) >> 1;
}

int main(int argc, char *argv[])
{
    UGBA_Init(&argc, &argv);

    SWI_SoundBias(0);
    if (get_bias_level() != 0)
        return 1;

    for (uint32_t i = 1; i < 0x2000; i <<= 1)
    {
        SWI_SoundBias(i);
        if (get_bias_level() != 0x100)
            return 1;
    }

    return 0;
}
