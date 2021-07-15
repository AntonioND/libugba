// SPDX-License-Identifier: LGPL-3.0-only
//
// Copyright (c) 2011-2015, 2019-2021, Antonio Niño Díaz

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "config.h"
#include "file_utils.h"
#include "input_utils.h"

#define CONFIG_FILE_NAME "config.ini"

// Default values
global_config GlobalConfig = {
    .screen_size = 3,

    .volume = 100,
    .channel_flags = 0x3F,
    .sound_mute = 0,
};

#define CFG_SCREEN_SIZE "screen_size"
// unsigned integer ( "1" - "5" )

#define CFG_SND_CHN_ENABLE "channels_enabled"
// "#3F" 3F = flags

#define CFG_SND_VOLUME "volume"
// "#VV" 00h - 80h

#define CFG_SND_MUTE "sound_mute"
// "true" - "false"

//---------------------------------------------------------------------

void Config_Save(void)
{
    FILE *f = fopen(CONFIG_FILE_NAME, "wb");
    if (f == NULL)
        return;

    fprintf(f, "[General]\n");
    fprintf(f, CFG_SCREEN_SIZE "=%d\n", GlobalConfig.screen_size);
    fprintf(f, "\n");

    fprintf(f, "[Sound]\n");
    fprintf(f, CFG_SND_CHN_ENABLE "=#%02X\n", GlobalConfig.channel_flags);
    fprintf(f, CFG_SND_VOLUME "=%d\n", GlobalConfig.volume);
    fprintf(f, CFG_SND_MUTE "=%s\n", GlobalConfig.sound_mute ? "true" : "false");
    fprintf(f, "\n");

    fprintf(f, "[Controls]\n");

    int controller = Input_PlayerGetController();
    int save_keys = 0;

    if (controller != -1)
    {
        // If this is a joystick
        char *controllername = Input_GetJoystickName(controller);
        if (strlen(controllername) > 0)
        {
            fprintf(f, "P1_Controller=[%s]\n", controllername);
            save_keys = 1;
        }
        else
        {
            // If there is a problem when getting the controller name, don't
            // save the input configuration.
            save_keys = 0;
        }
    }
    else
    {
        // If this is the keyboard
        fprintf(f, "P1_Controller=[Keyboard]\n");
        save_keys = 1;
    }

    if (save_keys)
    {
        for (int key = 0; key < P_NUM_KEYS; key++)
        {
            SDL_KeyCode code = Input_ControlsGetKey(key);
            fprintf(f, "P1_%s=%d\n", GBKeyNames[key], code);
        }
    }

    fclose(f);
}

static uint64_t asciihex_to_int(const char *text)
{
    uint64_t value = 0, i = 0;

    while (1)
    {
        char char_ = toupper(text[i++]);
        if (char_ == '\0') // End of string
            return value;
        else if (char_ >= '0' && char_ <= '9')
            value = (value * 16) + (char_ - '0');
        else if (char_ >= 'a' && char_ <= 'f')
            value = (value * 16) + (char_ - 'a' + 10);
        else if (char_ >= 'A' && char_ <= 'F')
            value = (value * 16) + (char_ - 'A' + 10);
        else
            return 0xFFFFFFFFFFFFFFFFULL;
    }
}

void Config_Load(void)
{
    const char *tmp;

    // Load configuration file and add a null terminator at the end

    char *ini;
    size_t size;
    File_Load(CONFIG_FILE_NAME, (void *)&ini, &size);
    if (ini == NULL)
        return;

    char *new_ini = realloc(ini, size + 1);
    if (new_ini == NULL)
    {
        free(ini);
        return;
    }

    ini = new_ini;
    ini[size] = '\0';

    // General options

    tmp = strstr(ini, CFG_SCREEN_SIZE);
    if (tmp)
    {
        tmp += strlen(CFG_SCREEN_SIZE) + 1;
        GlobalConfig.screen_size = atoi(tmp);
        if (GlobalConfig.screen_size > 4)
            GlobalConfig.screen_size = 4;
        else if (GlobalConfig.screen_size < 2)
            GlobalConfig.screen_size = 2;
    }

    // Sound options

    tmp = strstr(ini, CFG_SND_CHN_ENABLE);
    if (tmp)
    {
        tmp += strlen(CFG_SND_CHN_ENABLE) + 1;
        if (*tmp == '#')
        {
            tmp++;
            char aux[3];
            aux[0] = *tmp++;
            aux[1] = *tmp;
            aux[2] = '\0';
            GlobalConfig.channel_flags = asciihex_to_int(aux);
        }
    }

    tmp = strstr(ini, CFG_SND_VOLUME);
    if (tmp)
    {
        tmp += strlen(CFG_SND_VOLUME) + 1;
        GlobalConfig.volume = atoi(tmp);
    }

    tmp = strstr(ini, CFG_SND_MUTE);
    if (tmp)
    {
        tmp += strlen(CFG_SND_MUTE) + 1;
        if (strncmp(tmp, "true", strlen("true")) == 0)
            GlobalConfig.sound_mute = 1;
        else
            GlobalConfig.sound_mute = 0;
    }

    // Input options

    tmp = strstr(ini, "P1_Controller=[");
    if (tmp)
    {
        tmp += strlen("P1_Controller=[");

        char temp_str[64];
        strncpy(temp_str, tmp, sizeof(temp_str));
        temp_str[sizeof(temp_str) - 1] = '\0';

        for (size_t i = 0; i < sizeof(temp_str); i++)
        {
            if (temp_str[i] == ']')
                temp_str[i] = '\0';
        }
        // temp_str now has the name of the controller

        int index = Input_GetJoystickFromName(temp_str);
        if (index == -2)
        {
            // Couldn't find the name, don't load the controller
            // configuration. Just leave the default configuration.
        }
        else
        {
            Input_PlayerSetController(index);

            // Now, read keys
            for (int key = 0; key < P_NUM_KEYS; key++)
            {
                snprintf(temp_str, sizeof(temp_str), "P1_%s=", GBKeyNames[key]);
                tmp = strstr(ini, temp_str);
                if (tmp)
                {
                    tmp += strlen(temp_str);

                    int btn;
                    if (sscanf(tmp, "%d", &btn) == 1)
                    {
                        Input_ControlsSetKey(key, btn);
                    }
                }
            }
        }
    }

    free(ini);
}
