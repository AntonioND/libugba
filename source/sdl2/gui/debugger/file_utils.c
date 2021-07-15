// SPDX-License-Identifier: LGPL-3.0-only
//
// Copyright (c) 2011-2015, 2019-2021, Antonio Niño Díaz

#include <stdio.h>
#include <time.h>

#include "file_utils.h"

static char _fu_filename[MAX_PATHLEN];

char *FU_GetNewTimestampFilename(const char *basename)
{
    long long int number = 0;

    time_t rawtime;
    time(&rawtime);
    struct tm *ptm = gmtime(&rawtime);

    // Generate base file name based on the current time and date
    char timestamp[50];
    snprintf(timestamp, sizeof(timestamp), "%04d%02d%02d_%02d%02d%02d",
             1900 + ptm->tm_year, 1 + ptm->tm_mon, ptm->tm_mday,
             1 + ptm->tm_hour, ptm->tm_min, ptm->tm_sec);

    // Append a number to the name so that we can take multiple screenshots the
    // same second.
    while (1)
    {
        snprintf(_fu_filename, sizeof(_fu_filename), "%s_%s_%lld.png",
                 basename, timestamp, number);

        FILE *file = fopen(_fu_filename, "rb");
        if (file == NULL)
            break; // This name is available
        fclose(file);

        number++;
    }

    return _fu_filename;
}
