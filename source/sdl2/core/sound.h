// SPDX-License-Identifier: LGPL-3.0-only
//
// Copyright (c) 2020-2021 Antonio Niño Díaz

#ifndef SDL2_SOUND_H__
#define SDL2_SOUND_H__

void Sound_Handle_VBL(void);
void Sound_Initialize(void);

int Sound_PSG_GetChannelVolume(int channel);
volatile uint16_t *UGBA_MemWaveRamTwoBanks(void);

#endif // SDL2_SOUND_H__
