// SPDX-License-Identifier: MIT
//
// Copyright (c) 2021 Antonio Niño Díaz

// Example that shows how to place code in IWRAM and checks that it is faster
// than the same code in EWRAM. It also tests that a function in ARM is slower
// than in Thumb because of being placed in EWRAM. EWRAM has a 16-bit bus, so
// Thumb instructions can be read in one go, ARM instructions need two reads.

#include <inttypes.h>
#include <stdio.h>

#include <ugba/ugba.h>

IWRAM_CODE uint32_t divide_iwram(uint32_t a, uint32_t b);
EWRAM_CODE uint32_t divide_ewram(uint32_t a, uint32_t b);
uint32_t divide_arm(uint32_t a, uint32_t b);
uint32_t divide_thumb(uint32_t a, uint32_t b);

volatile uint32_t result;

#define DO_TEST(fn_name, result_var_name)                   \
                                                            \
    TM_TimerStart(0, 0, 256, 0);                            \
                                                            \
    for (uint32_t a = 100; a < 200; a++)                    \
    {                                                       \
        for (uint32_t b = 1; b < 100; b++)                  \
        result = fn_name(a, b);                             \
    }                                                       \
                                                            \
    uint32_t result_var_name = REG_TM0CNT_L;                \
                                                            \
    TM_TimerStop(0);                                        \

int main(int argc, char *argv[])
{
    UGBA_Init(&argc, &argv);

    // Test functions in IWRAM and EWRAM

    DO_TEST(divide_iwram, time_iwram);
    DO_TEST(divide_ewram, time_ewram);

    // Test ARM and Thumb functions

    DO_TEST(divide_arm, time_arm);
    DO_TEST(divide_thumb, time_thumb);

    // Display results

    IRQ_Enable(IRQ_VBLANK);

    DISP_ModeSet(0);

    CON_InitDefault();

    char string[100];
    snprintf(string, sizeof(string),
             "IWRAM: %" PRIu32 "\n"
             "EWRAM: %" PRIu32 "\n"
             "ARM:   %" PRIu32 "\n"
             "Thumb: %" PRIu32 "\n",
             time_iwram, time_ewram,
             time_arm, time_thumb);
    CON_Print(string);

    while (1)
    {
        SWI_VBlankIntrWait();

        KEYS_Update();

        uint16_t keys = KEYS_Held();

        if (keys & KEY_A)
            break;
    }

    CON_Clear();

    // A function in IWRAM will always be faster than the same function in
    // EWRAM. However, a function in ARM takes up more space than in Thumb,
    // which actually makes it slower in EWRAM (where they are placed by
    // default).

    if ((time_iwram < time_ewram) && (time_arm > time_thumb))
        CON_Print("Test OK");
    else
        CON_Print("Test FAILED");

    while (1)
        SWI_VBlankIntrWait();
}
