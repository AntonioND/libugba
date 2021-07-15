// SPDX-License-Identifier: LGPL-3.0-only
//
// Copyright (c) 2020 Antonio Niño Díaz

#ifndef SDL2_WAV_UTILS_H__
#define SDL2_WAV_UTILS_H__

#include <stddef.h>
#include <stdint.h>

void WAV_FileStart(const char *path, uint32_t sample_rate);
void WAV_FileEnd(void);

int WAV_FileIsOpen(void);

void WAV_FileStream(int16_t *buffer, size_t size);

#endif // SDL2_WAV_UTILS_H__
