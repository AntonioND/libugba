// SPDX-License-Identifier: LGPL-3.0-only
//
// Copyright (c) 2011-2015, 2019-2021, Antonio Niño Díaz

#ifndef SDL2_CONFIG_H__
#define SDL2_CONFIG_H__

typedef struct
{
    // General options
    // ---------------

    int screen_size;

    // Sound
    //-----

    int volume;
    int channel_flags;
    int sound_mute;

    // The input configuration is in input_utils.c

} global_config;

extern global_config GlobalConfig;

void Config_Save(void);
void Config_Load(void);

#endif // SDL2_CONFIG_H__
