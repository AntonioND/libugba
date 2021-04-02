// SPDX-License-Identifier: LGPL-3.0-only
//
// Copyright (c) 2011-2015, 2019-2021, Antonio Niño Díaz

#ifndef SDL2_GUI_DEBUGGER_FONT_UTILS_H__
#define SDL2_GUI_DEBUGGER_FONT_UTILS_H__

#include <stdint.h>

//------------------------------------------------------------------------------

#define CHR_MUSIC           14
#define STR_MUSIC           "\x0E"

#define CHR_ARROW_LEFT      16
#define STR_ARROW_LEFT      "\x10"
#define CHR_ARROW_RIGHT     17
#define STR_ARROW_RIGHT     "\x11"

#define CHR_SLIM_ARROW_UP   24
#define STR_SLIM_ARROW_UP   "\x18"
#define CHR_SLIM_ARROW_DOWN 25
#define STR_SLIM_ARROW_DOWN "\x19"

#define CHR_ARROW_UP        30
#define STR_ARROW_UP        "\x1E"
#define CHR_ARROW_DOWN      31
#define STR_ARROW_DOWN      "\x1F"

#define CHR_ARING_MINUS     134
#define STR_ARING_MINUS     "\x86"

#define CHR_AACUTE_MINUS    160
#define STR_AACUTE_MINUS    "\xA0"
#define CHR_IACUTE_MINUS    161
#define STR_IACUTE_MINUS    "\xA1"
#define CHR_OACUTE_MINUS    162
#define STR_OACUTE_MINUS    "\xA2"
#define CHR_UACUTE_MINUS    163
#define STR_UACUTE_MINUS    "\xA3"
#define CHR_NTILDE_MINUS    164
#define STR_NTILDE_MINUS    "\xA4"
#define CHR_NTILDE_MAYUS    165
#define STR_NTILDE_MAYUS    "\xA5"

#define CHR_SHADED_LIGHT    176
#define STR_SHADED_LIGHT    "\xB0"
#define CHR_SHADED_MEDIUM   177
#define STRR_SHADED_MEDIUM  "\xB1"
#define CHR_SHADED_DARK     178
#define STR_SHADED_DARK     "\xB2"

#define CHR_DBLBARS_UDL     185
#define STR_DBLBARS_UDL     "\xB9"
#define CHR_DBLBARS_UD      186
#define STR_DBLBARS_UD      "\xBA"
#define CHR_DBLBARS_DL      187
#define STR_DBLBARS_DL      "\xBB"
#define CHR_DBLBARS_UL      188
#define STR_DBLBARS_UL      "\xBC"

#define CHR_BARS_DL         191
#define STR_BARS_DL         "\xBF"

#define CHR_BARS_UR         192
#define STR_BARS_UR         "\xC0"
#define CHR_BARS_URL        193
#define STR_BARS_URL        "\xC1"
#define CHR_BARS_RDL        194
#define STR_BARS_RDL        "\xC2"
#define CHR_BARS_URD        195
#define STR_BARS_URD        "\xC3"
#define CHR_BARS_RL         196
#define STR_BARS_RL         "\xC4"
#define CHR_BARS_URDL       197
#define STR_BARS_URDL       "\xC5"

#define CHR_DBLBARS_UR      200
#define STR_DBLBARS_UR      "\xC8"
#define CHR_DBLBARS_RD      201
#define STR_DBLBARS_RD      "\xC9"
#define CHR_DBLBARS_URL     202
#define STR_DBLBARS_URL     "\xCA"
#define CHR_DBLBARS_RDL     203
#define STR_DBLBARS_RDL     "\xCB"
#define CHR_DBLBARS_URD     204
#define STR_DBLBARS_URD     "\xCC"
#define CHR_DBLBARS_RL      205
#define STR_DBLBARS_RL      "\xCD"
#define CHR_DBLBARS_URDL    206
#define STR_DBLBARS_URDL    "\xCE"

#define CHR_BARS_LU         217
#define STR_BARS_LU         "\xD9"
#define CHR_BARS_RD         218
#define STR_BARS_RD         "\xDA"

#define CHR_SQUAREBLACK_MID 254
#define STR_SQUAREBLACK_MID "\xFE"

//------------------------------------------------------------------------------

#define FONT_WIDTH          (7)
#define FONT_HEIGHT         (12)

int FU_Print(unsigned char *buffer, int bufw, int bufh, int tx, int ty,
             const char *txt, ...);
int FU_PrintChar(unsigned char *buffer, int bufw, int bufh, int tx, int ty,
                 unsigned char c, int color);
int FU_PrintColor(unsigned char *buffer, int bufw, int bufh, int tx, int ty,
                  uint32_t color, const char *txt, ...);

#endif // SDL2_GUI_DEBUGGER_FONT_UTILS_H__
