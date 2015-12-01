/*
 * Copyright (C) 2015 by Martin Dejean
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

#ifndef __MQ_DEBUG_H__
#define __MQ_DEBUG_H__

#define LOG_MUTE     (1)
#define LOG_FATAL    (2)
#define LOG_ERROR    (3)
#define LOG_WARN     (4)
#define LOG_INFO     (5)
#define LOG_DBG      (6)

#define __TRUNC_FILE__ (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)

#ifdef DEBUG
#define MQ_LOG(level, input) \
do { mq::Trace(input, level, __TRUNC_FILE__, __LINE__, __func__); } while (0);
#else
#define MQ_LOG(level, input) do {} while (0);
#endif

#include <iostream>
#include "mq_types.h"

namespace mq
{
    extern S32 dbgLevel;
    
    inline const char* const LogLevelName(S32 logLevel)
    {
        switch (logLevel) {
            case LOG_FATAL:
                return "FATAL";
            case LOG_ERROR:
                return "ERROR";
            case LOG_WARN:
                return "WARN";
            case LOG_INFO:
                return "INFO";
            case LOG_DBG:
                return "DEBUG";
            default:
                return "";
        }
    }

    template<class T>
    inline void Trace(T input, S32 level, const char* file, S32 line, const char* func)
    {
        String shortFile = String(file);
        USize lastDot = shortFile.rfind(".");
        shortFile = shortFile.substr(0, lastDot);
        
        if (level <= dbgLevel) {
            std::cout << "[" << LogLevelName(level) << "][" << shortFile << "(" << line << ")]"
            << "[" << func << "] "
            << (input) << std::endl;
        }
    }
}

#endif //__MQ_DEBUG_H__
