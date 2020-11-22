// SPDX-License-Identifier: LGPL-3.0-only
//
// Copyright (c) 2011-2015, 2019-2020 Antonio Niño Díaz

#ifndef PNG_UTILS_H__
#define PNG_UTILS_H__

// Buffer is 32 bit (RGBA) / 24 bit (RGB), returns 0 on success
int Save_PNG(const char *filename, unsigned char *buffer,
             int width, int height, int is_rgba);

// Buffer is 32 bit (RGBA), returns 0 on success
int Read_PNG(const char *filename, unsigned char **_buffer,
             int *_width, int *_height);

#endif // PNG_UTILS_H__
