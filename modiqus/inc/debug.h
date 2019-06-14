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

#include <iostream>
#include "types.h"

#define MQ_LOG_LEVEL_MUTE     (1)
#define MQ_LOG_LEVEL_FATAL    (2)
#define MQ_LOG_LEVEL_ERROR    (3)
#define MQ_LOG_LEVEL_WARN     (4)
#define MQ_LOG_LEVEL_INFO     (5)
#define MQ_LOG_LEVEL_DEBUG    (6)

extern modiqus::S32 log_level;

inline const char* const mq_get_log_level_str(modiqus::S32 log_level_value)
{
    switch (log_level_value)
    {
        case MQ_LOG_LEVEL_FATAL:
        {
            return "FATAL";
        }
        case MQ_LOG_LEVEL_ERROR:
        {
            return "ERROR";
        }
        case MQ_LOG_LEVEL_WARN:
        {
            return "WARN";
        }
        case MQ_LOG_LEVEL_INFO:
        {
            return "INFO";
        }
        case MQ_LOG_LEVEL_DEBUG:
        {
            return "DEBUG";
        }
        default:
        {
            return "";
        }
    }
}

inline char *mq_get_filename(const char *mystr, char dot, char sep) {
    char *retstr, *lastdot, *lastsep;
    
    // Error checks and allocate string.
    
    if (mystr == NULL) {
        return NULL;
    }
    
    if ((retstr = (char*)malloc(strlen(mystr) + 1)) == NULL) {
        return NULL;
    }
    
    // Make a copy and find the relevant characters.
    
    strcpy (retstr, mystr);
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
    
    // Return the modified string.
    
    return retstr;
}

inline void mq_log(const char *message, modiqus::S32 level, const char *file, modiqus::S32 line, const char *func)
{
    char *filename = mq_get_filename(file, '.', '/');
    
    if (log_level >= level)
    {
        std::cout << "[" << mq_get_log_level_str(level) << " | " << filename << "." << func
                  << " | " << line << "] " << message << std::endl;
    }
    
    free(filename);
}

inline void log_csound(const char* format, va_list args)
{
    if (log_level > MQ_LOG_LEVEL_MUTE)
    {
        printf("[Csound] ");
        vprintf (format, args);
        
        if (strchr(format, '\n') == 0)
        {
            printf("\n");
        }
    }
}

#define __TRUNC_FILE__ (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)

#ifdef DEBUG
#define MQ_LOG_MESSAGE(level, message) \
do { mq_log(message, level, __TRUNC_FILE__, __LINE__, __func__); } while (0);
#else
#define MQ_LOG_MESSAGE(level, message) do {} while (0);
#endif

#define MQ_LOG_FATAL(message) MQ_LOG_MESSAGE(MQ_LOG_LEVEL_FATAL, message)
#define MQ_LOG_ERROR(message) MQ_LOG_MESSAGE(MQ_LOG_LEVEL_ERROR, message)
#define MQ_LOG_WARN(message) MQ_LOG_MESSAGE(MQ_LOG_LEVEL_WARN, message)
#define MQ_LOG_INFO(message) MQ_LOG_MESSAGE(MQ_LOG_LEVEL_INFO, message)
#define MQ_LOG_DEBUG(message) MQ_LOG_MESSAGE(MQ_LOG_LEVEL_DEBUG, message)

#endif //DEBUG_H
