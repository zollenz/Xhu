/*
 * Copyright (C) 2018 by Martin Dejean
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

#ifndef DEBUG_HPP
#define DEBUG_HPP

#include <iostream>
#include "types.hpp"

#define MQ_LOG_LEVEL_MUTE     (1)
#define MQ_LOG_LEVEL_FATAL    (2)
#define MQ_LOG_LEVEL_ERROR    (3)
#define MQ_LOG_LEVEL_WARN     (4)
#define MQ_LOG_LEVEL_INFO     (5)
#define MQ_LOG_LEVEL_DEBUG    (6)

namespace modiqus
{
    extern S32 log_level;
    
    inline const char* const get_log_level_name(S32 logLevel)
    {
        switch (logLevel)
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

    template<class T>
    inline void log(T input, S32 level, const char* file, S32 line, const char* func)
    {
        String shortFile = String(file);
        USize lastDot = shortFile.rfind(".");
        shortFile = shortFile.substr(0, lastDot);
        
        if (log_level >= level)
        {
            std::cout << "[" << get_log_level_name(level) << "." << shortFile << "." << func
                      << "." << line << "] " << (input) << std::endl;
        }
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
}

#define __TRUNC_FILE__ (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)

#ifdef DEBUG
#define MQ_LOG(level, input) \
do { modiqus::log(input, level, __TRUNC_FILE__, __LINE__, __func__); } while (0);
#else
#define MQ_LOG(level, input) do {} while (0);
#endif

#define MQ_LOG_FATAL(input) MQ_LOG(MQ_LOG_LEVEL_FATAL, input)
#define MQ_LOG_ERROR(input) MQ_LOG(MQ_LOG_LEVEL_ERROR, input)
#define MQ_LOG_WARN(input) MQ_LOG(MQ_LOG_LEVEL_WARN, input)
#define MQ_LOG_INFO(input) MQ_LOG(MQ_LOG_LEVEL_INFO, input)
#define MQ_LOG_DEBUG(input) MQ_LOG(MQ_LOG_LEVEL_DEBUG, input)

#endif //DEBUG_HPP
