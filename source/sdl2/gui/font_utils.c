// SPDX-License-Identifier: LGPL-3.0-only
//
// Copyright (c) 2011-2015, 2019-2021, Antonio Niño Díaz

#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "font_utils.h"

//------------------------------------------------------------------------------

#define FONT_CHARS_IN_ROW    (32)
#define FONT_CHARS_IN_COLUMN (8)

extern const uint8_t fnt_data[]; // in font_data.c

//------------------------------------------------------------------------------

int FU_Print(unsigned char *buffer, int bufw, int bufh, int tx, int ty,
             const char *txt, ...)
{
    char txtbuffer[300];

    va_list args;
    va_start(args, txt);
    int ret = vsnprintf(txtbuffer, sizeof(txtbuffer), txt, args);
    va_end(args);

    txtbuffer[sizeof(txtbuffer) - 1] = '\0';

    if ((tx < 0) || (ty < 0))
        return 0;
    if ((ty + FONT_HEIGHT) > bufh)
        return 0; // Multiline is not supported

    int i = 0;
    while (1)
    {
        char c = txtbuffer[i++];

        if (c == '\0')
            break;

        if ((tx + FONT_WIDTH) > bufw)
            break;

        int texx = (c % FONT_CHARS_IN_ROW) * FONT_WIDTH;
        int texy = (c / FONT_CHARS_IN_ROW) * FONT_HEIGHT;

        int tex_offset = (texy * (FONT_CHARS_IN_ROW * FONT_WIDTH * 4))
                         + texx * 4;             // Font is 4 bytes per pixel
        int buf_offset = ((ty * bufw) + tx) * 3; // Buffer is 3 bytes per pixel

        for (int y = 0; y < FONT_HEIGHT; y++)
        {
            unsigned char *bufcopy = &(buffer[buf_offset]);
            const uint8_t *texcopy = &(fnt_data[tex_offset]);

            for (int x = 0; x < FONT_WIDTH; x++)
            {
                *bufcopy++ = *texcopy++;
                *bufcopy++ = *texcopy++;
                *bufcopy++ = *texcopy++;
                texcopy++;
            }
            tex_offset += (FONT_CHARS_IN_ROW * FONT_WIDTH * 4);
            buf_offset += bufw * 3;
        }
        tx += FONT_WIDTH;
    }

    return ret;
}

int FU_PrintColor(unsigned char *buffer, int bufw, int bufh, int tx, int ty,
                  uint32_t color, const char *txt, ...)
{
    char txtbuffer[300];

    va_list args;
    va_start(args, txt);
    int ret = vsnprintf(txtbuffer, sizeof(txtbuffer), txt, args);
    va_end(args);

    txtbuffer[sizeof(txtbuffer) - 1] = '\0';

    if ((tx < 0) || (ty < 0))
        return 0;
    if ((ty + FONT_HEIGHT) > bufh)
        return 0; // Multiline is not supported

    int i = 0;
    while (1)
    {
        char c = txtbuffer[i++];

        if (c == '\0')
            break;

        if ((tx + FONT_WIDTH) > bufw)
            break;

        int texx = (c % FONT_CHARS_IN_ROW) * FONT_WIDTH;
        int texy = (c / FONT_CHARS_IN_ROW) * FONT_HEIGHT;

        int tex_offset = (texy * (FONT_CHARS_IN_ROW * FONT_WIDTH * 4))
                         + texx * 4;             // Font is 4 bytes per pixel
        int buf_offset = ((ty * bufw) + tx) * 3; // Buffer is 3 bytes per pixel

        for (int y = 0; y < FONT_HEIGHT; y++)
        {
            unsigned char *bufcopy = &(buffer[buf_offset]);
            const uint8_t *texcopy = &(fnt_data[tex_offset]);

            for (int x = 0; x < FONT_WIDTH; x++)
            {
                uint32_t r = (uint8_t)(*texcopy++);
                uint32_t g = (uint8_t)(*texcopy++);
                uint32_t b = (uint8_t)(*texcopy++);

                *bufcopy++ = (r * (color & 0xFF)) >> 8;
                *bufcopy++ = (g * ((color >> 8) & 0xFF)) >> 8;
                *bufcopy++ = (b * ((color >> 16) & 0xFF)) >> 8;

                texcopy++; // Skip alpha
            }
            tex_offset += FONT_CHARS_IN_ROW * FONT_WIDTH * 4;
            buf_offset += bufw * 3;
        }
        tx += FONT_WIDTH;
    }

    return ret;
}

int FU_PrintChar(unsigned char *buffer, int bufw, int bufh, int tx, int ty,
                 unsigned char c, int color)
{
    if ((tx < 0) || (ty < 0))
        return 0;
    if ((ty + FONT_HEIGHT) > bufh)
        return 0;

    if (c == '\0')
        return 0;

    if ((tx + FONT_WIDTH) > bufw)
        return 0;

    int texx = (c % FONT_CHARS_IN_ROW) * FONT_WIDTH;
    int texy = (c / FONT_CHARS_IN_ROW) * FONT_HEIGHT;

    // Font is 4 bytes per pixel
    int tex_offset = (texy * (FONT_CHARS_IN_ROW * FONT_WIDTH * 4)) + texx * 4;
    int buf_offset = ((ty * bufw) + tx) * 3; // Buffer is 3 bytes per pixel

    for (int y = 0; y < FONT_HEIGHT; y++)
    {
        unsigned char *bufcopy = &(buffer[buf_offset]);
        const uint8_t *texcopy = &(fnt_data[tex_offset]);

        for (int x = 0; x < FONT_WIDTH; x++)
        {
            int r = (uint8_t)(*texcopy++);
            int g = (uint8_t)(*texcopy++);
            int b = (uint8_t)(*texcopy++);

            *bufcopy++ = (r * (color & 0xFF)) >> 8;
            *bufcopy++ = (g * ((color >> 8) & 0xFF)) >> 8;
            *bufcopy++ = (b * ((color >> 16) & 0xFF)) >> 8;

            texcopy++; // Skip alpha
        }

        tex_offset += (FONT_CHARS_IN_ROW * FONT_WIDTH * 4);
        buf_offset += bufw * 3;
    }

    return 1;
}
