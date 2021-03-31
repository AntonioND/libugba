// SPDX-License-Identifier: LGPL-3.0-only
//
// Copyright (c) 2021 Antonio Niño Díaz

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <SDL2/SDL.h>

#include <ugba/ugba.h>

#include "debug_utils.h"
#include "file_utils.h"

// Every 3 seconds, check if the SRAM contents have changed. If so, save them to
// the ssave data file.
//
// Normally, data is saved to disk when the program exits. However, if there is
// a crash in the time after the user has saved the data but before the program
// has exited successfully, the data won't actually be saved to disk.
//
// It is not possible to detect writes to SRAM, as it is just a regular array in
// memory.
//
// This autosave feature is meant to prevent loss of data because of the
// mentioned problem. This is why the hash function isn't secure. It's just
// meant to detect changes in the data. Even a hash collision wouldn't be a huge
// deal. It is very unlikely for a hash collision to happen the exact time that
// a crash prevents the user from saving the data. The program shouldn't crash
// to begin with.
#define AUTOSAVE_INTERVAL_MS    (1000 * 3)

static char *sav_path;

static SDL_TimerID AutosaveTimerID;

static uint64_t oldhash;

static void UGBA_SaveFileClose(void)
{
    Debug_Log("Saving data...");

    File_Save(sav_path, MEM_SRAM, MEM_SRAM_SIZE);

    Debug_Log("Data saved!");
}

// https://en.wikipedia.org/wiki/Fowler%E2%80%93Noll%E2%80%93Vo_hash_function#FNV-1_hash
static uint64_t UGBA_FNV_1_Hash(const void *buffer, size_t size)
{
    const uint64_t FNV_offset_basis = 14695981039346656037ULL;
    const uint64_t FNV_prime = 1099511628211ULL;

    uint64_t hash = FNV_offset_basis;

    const uint8_t *bytes = buffer;

    for (size_t i = 0; i < size; i++)
    {
        uint8_t data = bytes[i];

        hash = hash * FNV_prime;
        hash = hash ^ (uint64_t)data;
    }

    return hash;
}

static Uint32 Autosave_Callback(Uint32 interval, UNUSED void *param)
{
    // Check if there have been changes to SRAM

    uint64_t hash = UGBA_FNV_1_Hash(MEM_SRAM, MEM_SRAM_SIZE);

    // If so, save the changes

    if (hash != oldhash)
    {
        oldhash = hash;
        UGBA_SaveFileClose();
    }

    return interval;
}

static void UGBA_ExitSaveFileClose(void)
{
    SDL_RemoveTimer(AutosaveTimerID);

    UGBA_SaveFileClose();

    free(sav_path);
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

        // Calculate hash of the initial state

        oldhash = UGBA_FNV_1_Hash(MEM_SRAM, MEM_SRAM_SIZE);

        // Cleanup

        free(buffer);
    }

    // Make sure that save file is written back to disk on exit

    atexit(UGBA_ExitSaveFileClose);

    // Start timer that autosaves the SRAM

    AutosaveTimerID = SDL_AddTimer(AUTOSAVE_INTERVAL_MS, Autosave_Callback,
                                   NULL);
}
