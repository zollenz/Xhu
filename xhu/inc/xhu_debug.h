/*
 * Copyright (C) 2019 by Martin Dejean
 *
 * This file is part of Xhu.
 * Xhu is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Xhu is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Xhu.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#ifndef DEBUG_H
#define DEBUG_H

#include <stdbool.h>
#include "xhu_defs.h"

#define XHU_LOG_LEVEL_MUTE     (1)
#define XHU_LOG_LEVEL_FATAL    (2)
#define XHU_LOG_LEVEL_ERROR    (3)
#define XHU_LOG_LEVEL_WARN     (4)
#define XHU_LOG_LEVEL_INFO     (5)
#define XHU_LOG_LEVEL_DEBUG    (6)

extern xhu_s32_t xhu_log_level;
extern bool xhu_log_with_func_info;

static inline const char *const xhu_get_log_level_str(xhu_s32_t log_level_value)
{
    switch (log_level_value) {
        case XHU_LOG_LEVEL_FATAL:
            return "FATAL";
        case XHU_LOG_LEVEL_ERROR:
            return "ERROR";
        case XHU_LOG_LEVEL_WARN:
            return "WARN";
        case XHU_LOG_LEVEL_INFO:
            return "INFO";
        case XHU_LOG_LEVEL_DEBUG:
            return "DEBUG";
        default:
            return "UNKNOWN";
    }
}


static inline char *xhu_get_filename(const char *path, char dot, char sep)
{
    char *retstr, *lastdot, *lastsep;
    
    if (path == NULL) {
        return NULL;
    }
    
    const char *filename = strrchr(path, sep) ? strrchr(path, sep) + 1 : path;
    
    if ((retstr = (char*)malloc(strlen(filename) + 1)) == NULL) {
        return NULL;
    }
    
    strcpy (retstr, filename);
    lastdot = strrchr (retstr, dot);
    lastsep = (sep == 0) ? NULL : strrchr (retstr, sep);
    
    // If it has an extension separator.
    if (lastdot != NULL) {
        // and it's before the extenstion separator.
        if (lastsep != NULL) {
            if (lastsep < lastdot) {
                // then remove it.
                
                *lastdot = '\0';
            }
        } else {
            // Has extension separator with no path separator.
            *lastdot = '\0';
        }
    }
    
    return retstr;
}

static inline void xhu_log(const char *message, xhu_s32_t level,
                   const char *caller_file_path, const char *caller_func_name, xhu_s32_t line)
{
    char *filename = xhu_get_filename(caller_file_path, '.', '/');
    const char *log_level_str = xhu_get_log_level_str(level);

    if (xhu_log_level >= level) {
        if (xhu_log_with_func_info) {
            printf("%s - %s.%s:%d - %s\n", log_level_str, filename, caller_func_name, line, message);
        } else {
            printf("%s - %s\n", log_level_str, message);
        }
    }
    
    free(filename);
}

static inline void xhu_log_csound(const char* format, va_list args)
{
    if (xhu_log_level == XHU_LOG_LEVEL_DEBUG) {
        printf("[Csound] ");
        vprintf (format, args);
        
        if (strchr(format, '\n') == 0) {
            printf("\n");
        }
    }
}

#ifdef DEBUG
#define XHU_LOG_MESSAGE(level, message) \
do { xhu_log(message, level, __FILE__, __func__, __LINE__); } while (0);
#else
#define XHU_LOG_MESSAGE(level, message) do {} while (0);
#endif

#define XHU_LOG_FATAL(message) XHU_LOG_MESSAGE(XHU_LOG_LEVEL_FATAL, message)
#define XHU_LOG_ERROR(message) XHU_LOG_MESSAGE(XHU_LOG_LEVEL_ERROR, message)
#define XHU_LOG_WARN(message) XHU_LOG_MESSAGE(XHU_LOG_LEVEL_WARN, message)
#define XHU_LOG_INFO(message) XHU_LOG_MESSAGE(XHU_LOG_LEVEL_INFO, message)
#define XHU_LOG_DEBUG(message) XHU_LOG_MESSAGE(XHU_LOG_LEVEL_DEBUG, message)

#endif // DEBUG_H
