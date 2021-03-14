// SPDX-License-Identifier: LGPL-3.0-only
//
// Copyright (c) 2021 Antonio Niño Díaz

#include <stdio.h>
#include <stdlib.h>

#include "debug_utils.h"

void File_Load(const char *filename, void **buffer, size_t *size_)
{
    FILE *f = fopen(filename, "rb");
    size_t size;

    *buffer = NULL;
    if (size_)
        *size_ = 0;

    if (f == NULL)
    {
        Debug_Log("File couldn't be opened: %s", filename);
        return;
    }

    fseek(f, 0, SEEK_END);
    size = ftell(f);
    if (size_)
        *size_ = size;

    if (size == 0)
    {
        Debug_Log("File size is 0: %s", filename);
        fclose(f);
        return;
    }

    rewind(f);
    *buffer = malloc(size);
    if (*buffer == NULL)
    {
        Debug_Log("Not enought memory to load file: %s", filename);
        fclose(f);
        return;
    }

    if (fread(*buffer, size, 1, f) != 1)
    {
        Debug_Log("Error while reading file: %s", filename);
        fclose(f);
        free(*buffer);
        return;
    }

    fclose(f);
}

void File_Save(const char *filename, void *buffer, size_t size)
{
    FILE *f = fopen(filename, "wb");

    if (f == NULL)
    {
        Debug_Log("File couldn't be opened: %s", filename);
        return;
    }

    if (fwrite(buffer, size, 1, f) != 1)
        Debug_Log("Error while writing file: %s", filename);

    fclose(f);
}
