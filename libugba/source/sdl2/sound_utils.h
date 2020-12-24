// SPDX-License-Identifier: LGPL-3.0-only
//
// Copyright (c) 2011-2015, 2019-2020 Antonio Niño Díaz

#ifndef SDL2_SOUND_UTILS_H__
#define SDL2_SOUND_UTILS_H__

#include <stdint.h>

void Sound_Init(void);

void Sound_SendSamples(int16_t *buffer, int len);

int Sound_IsBufferOverThreshold(void);

void Sound_Enable(void);
void Sound_Disable(void);

#endif // SDL2_SOUND_UTILS_H__
