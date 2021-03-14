// SPDX-License-Identifier: LGPL-3.0-only
//
// Copyright (c) 2021 Antonio Niño Díaz

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <ugba/ugba.h>

#include "debug_utils.h"
#include "file_utils.h"

static char *sav_path;

static void UGBA_SaveFileClose(void)
{
    Debug_Log("Saving data...");

    File_Save(sav_path, MEM_SRAM, MEM_SRAM_SIZE);

    free(sav_path);

    Debug_Log("Data saved!");
}

void UGBA_SaveFileOpen(const char *path)
{
    Debug_Log("Reading saved data: %s", path);

    // Copy name of file

    size_t sav_path_size = strlen(path) + 1;

    sav_path = malloc(sav_path_size);
    if (sav_path == NULL)
    {
        Debug_Log("%s: Can't allocate memory", __func__);
        return;
    }

    snprintf(sav_path, sav_path_size, "%s", path);

    // Load file

    void *buffer;
    size_t size;

    File_Load(sav_path, &buffer, &size);

    if (size > 0)
    {
        // If file exists, copy contents to SRAM

        if (size > MEM_SRAM_SIZE)
        {
            Debug_Log("%s: Saved file is too big, truncated (%zu > %u)",
                      __func__, size, MEM_SRAM_SIZE);
            size = MEM_SRAM_SIZE;
        }

        memcpy(MEM_SRAM, buffer, size);

        // Cleanup

        free(buffer);
    }

    // Make sure that save file is written back to disk on exit

    atexit(UGBA_SaveFileClose);
}
