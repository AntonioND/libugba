// SPDX-License-Identifier: MIT
//
// Copyright (c) 2021 Antonio Niño Díaz

#include <ugba/ugba.h>

#define SCSD_MODE_MEDIA     (0x3)
#define SCSD_MODE_RAM_RW    (0x5)

static uint16_t *scsd_set_mode(uint16_t mode)
{
    volatile uint16_t *reg = (volatile uint16_t *)0x9FFFFFE;
    const uint16_t password = 0xA55A;

    *reg = password;
    *reg = password;
    *reg = mode;
    *reg = mode;

    return (uint16_t *)0x8000000;
}

// Detect SuperCard SD
static int scsd_detect(void)
{
    const uint16_t test_value = 0xAA55;

    // Try to write in R/W mode

    volatile uint16_t *ram = scsd_set_mode(SCSD_MODE_RAM_RW);

    uint16_t old_value = *ram; // Preserve old value

    *ram = test_value;
    if (*ram != test_value)
        return 0;

    *ram = old_value; // Restore old value

    // Try to write in RO mode

    scsd_set_mode(SCSD_MODE_MEDIA);

    *ram = test_value;
    if (*ram == test_value)
        return 0;

    return 1;
}

uint16_t UGBA_FlashcartOptimizedWaitstates(void)
{
    if (scsd_detect())
        return WAITCNT_SCSD;
    else
        return WAITCNT_OPTIMIZED;
}
