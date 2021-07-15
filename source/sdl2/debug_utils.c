// SPDX-License-Identifier: LGPL-3.0-only
//
// Copyright (c) 2011-2015, 2019-2020 Antonio Niño Díaz

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

#define LOG_PATH "log.txt"

static FILE *f_log = NULL;
static int logging_allowed = 0;

void Debug_End(void)
{
    if (f_log != NULL)
    {
        fclose(f_log);
        f_log = NULL;
    }

    logging_allowed = 0;
}

void Debug_Init(void)
{
    logging_allowed = 1;

    atexit(Debug_End);

    // Remove previous log file if there was one
    remove(LOG_PATH);
}

void Debug_Log(const char *msg, ...)
{
    if (logging_allowed == 0)
        return;

    // If the log file isn't open, try to open it.
    if (f_log == NULL)
    {
        f_log = fopen(LOG_PATH, "w");
    }

    // If the log file is open, write to it.
    if (f_log != NULL)
    {
        va_list args;
        va_start(args, msg);
        vfprintf(f_log, msg, args);
        va_end(args);
        fputc('\n', f_log);
        fflush(f_log);
    }
}
