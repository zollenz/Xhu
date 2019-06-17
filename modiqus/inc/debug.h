/*
 * Copyright (C) 2019 by Martin Dejean
 *
 * This file is part of Modiqus.
 * Modiqus is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Modiqus is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Modiqus.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#ifndef DEBUG_H
#define DEBUG_H

#include "types.h"

#define MQ_LOG_LEVEL_MUTE     (1)
#define MQ_LOG_LEVEL_FATAL    (2)
#define MQ_LOG_LEVEL_ERROR    (3)
#define MQ_LOG_LEVEL_WARN     (4)
#define MQ_LOG_LEVEL_INFO     (5)
#define MQ_LOG_LEVEL_DEBUG    (6)

extern mq_s32_t mq_log_level;
extern bool mq_log_with_func_info;

inline const char* const mq_get_log_level_str(mq_s32_t log_level_value)
{
    switch (log_level_value) {
        case MQ_LOG_LEVEL_FATAL:
            return "FATAL";
        case MQ_LOG_LEVEL_ERROR:
            return "ERROR";
        case MQ_LOG_LEVEL_WARN:
            return "WARN";
        case MQ_LOG_LEVEL_INFO:
            return "INFO";
        case MQ_LOG_LEVEL_DEBUG:
            return "DEBUG";
        default:
            return "UNKNOWN";
    }
}

inline char *mq_get_filename(const char *path, char dot, char sep)
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

inline void mq_log(const char *message, mq_s32_t level,
                   const char *caller_file_path, const char *caller_func_name, mq_s32_t line)
{
    char *filename = mq_get_filename(caller_file_path, '.', '/');
    const char *log_level_str = mq_get_log_level_str(level);

    if (mq_log_level >= level) {
        if (mq_log_with_func_info) {
            printf("%s - %s.%s:%d - %s\n", log_level_str, filename, caller_func_name, line, message);
        } else {
            printf("%s - %s\n", log_level_str, message);
        }
    }
    
    free(filename);
}

inline void mq_log_csound(const char* format, va_list args)
{
    if (mq_log_level == MQ_LOG_LEVEL_DEBUG) {
        printf("[Csound] ");
        vprintf (format, args);
        
        if (strchr(format, '\n') == 0) {
            printf("\n");
        }
    }
}

#ifdef DEBUG
#define MQ_LOG_MESSAGE(level, message) \
do { mq_log(message, level, __FILE__, __func__, __LINE__); } while (0);
#else
#define MQ_LOG_MESSAGE(level, message) do {} while (0);
#endif

#define MQ_LOG_FATAL(message) MQ_LOG_MESSAGE(MQ_LOG_LEVEL_FATAL, message)
#define MQ_LOG_ERROR(message) MQ_LOG_MESSAGE(MQ_LOG_LEVEL_ERROR, message)
#define MQ_LOG_WARN(message) MQ_LOG_MESSAGE(MQ_LOG_LEVEL_WARN, message)
#define MQ_LOG_INFO(message) MQ_LOG_MESSAGE(MQ_LOG_LEVEL_INFO, message)
#define MQ_LOG_DEBUG(message) MQ_LOG_MESSAGE(MQ_LOG_LEVEL_DEBUG, message)

#endif //DEBUG_H
