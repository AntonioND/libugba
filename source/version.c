// SPDX-License-Identifier: MIT
//
// Copyright (c) 2020 Antonio Niño Díaz

#include <ugba/ugba.h>

uint32_t UGBA_VersionCompiledGet(void)
{
    return LIBUGBA_VERSION;
}

int UGBA_VersionIsCompatible(uint32_t version)
{
    // Version that the program was built against

    uint32_t major = (version >> 16) & 0xFF;
    uint32_t minor = (version >> 8) & 0xFF;
    //uint32_t patch = (version >> 0) & 0xFF;

    // Check against version that the library thinks it has

    if (major != LIBUGBA_VERSION_MAJOR)
        return 0;

    if (minor > LIBUGBA_VERSION_MINOR)
        return 0;

    return 1;
}
