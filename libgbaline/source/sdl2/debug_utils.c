// SPDX-License-Identifier: LGPL-3.0-only
//
// Copyright (c) 2011-2015, 2019-2020 Antonio Niño Díaz

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

#define LOG_PATH "log.txt"

static FILE *f_log;
static int log_file_opened = 0;

void Debug_End(void)
{
    if (log_file_opened)
        fclose(f_log);

    log_file_opened = 0;
}

void Debug_Init(void)
{
    log_file_opened = 0;
    atexit(Debug_End);

    remove(LOG_PATH);
}

void Debug_Log(const char *msg, ...)
{
    if (log_file_opened == 0)
    {
        f_log = fopen(LOG_PATH, "w");
        if (f_log)
            log_file_opened = 1;
    }

    if (log_file_opened)
    {
        va_list args;
        va_start(args, msg);
        vfprintf(f_log, msg, args);
        va_end(args);
        fputc('\n', f_log);
        fflush(f_log);
    }
}
