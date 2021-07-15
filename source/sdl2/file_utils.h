// SPDX-License-Identifier: LGPL-3.0-only
//
// Copyright (c) 2021 Antonio Niño Díaz

#ifndef SDL2_FILE_H__
#define SDL2_FILE_H__

#include <stddef.h>

void File_Load(const char *filename, void **buffer, size_t *size_);
void File_Save(const char *filename, void *buffer, size_t size);

#endif // SDL2_FILE_H__
