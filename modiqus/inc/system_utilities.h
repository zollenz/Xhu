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

#ifndef SYSTEM_UTILITIES_H
#define SYSTEM_UTILITIES_H

#ifdef __APPLE__
#include <mach-o/dyld.h>
#endif

#include <sstream>
#include "debug.h"

template<class T>
inline mq_str_t mq_to_string(const T& input)
{
    std::ostringstream stream;
    stream << input;
    
    //        if (stream.fail()) {
    //            std::runtime_error e(numberAsString);
    //            throw e;
    //        }
    
    return stream.str();
}

inline const mq_s32_t mq_size_to_int(const mq_list_size_t value)
{
    if (value <= INT_MAX)
    {
        return static_cast<mq_s32_t>(value);
    }

    MQ_LOG_ERROR("Size is bigger than INT_MAX. Returning -1");
    return -1;
}

inline void mq_pause(mq_list_size_t seconds)
{
    clock_t goal = clock() + seconds * CLOCKS_PER_SEC;
    while (goal > clock());
}

inline mq_str_t mq_get_executable_path()
{
#ifdef __APPLE__
    char path[PATH_MAX];
    uint32_t size = sizeof(path);
    char log_message[100];

    if (_NSGetExecutablePath(path, &size) == 0)
    {
        sprintf(log_message, "Executable path is %s", path);
        MQ_LOG_DEBUG(log_message)
    }
    else
    {
        sprintf(log_message, "Buffer too small; need size %u", size);
        MQ_LOG_ERROR(log_message)
    }

    return mq_str_t(path);
#else
//    char temp[PATH_MAX];
//    return (getcwd(temp, PATH_MAX) ? mq_str(temp) : mq_str(""));
    return "";
#endif
}

inline mq_str_t mq_get_bundle_contents_path()
{
    mq_str_t executable_path = mq_get_executable_path();
    mq_str_t contents_path = mq_str_t(executable_path);
    mq_list_size_t slash_index = contents_path.rfind("/");
    contents_path = contents_path.substr(0, slash_index - 1);
    slash_index = contents_path.rfind("/");
    contents_path = contents_path.substr(0, slash_index - 1);
    
    return contents_path;
}

inline bool mq_approximately_equals(const mq_f32_t value_1, const mq_f32_t value_2)
{
    const mq_f32_t difference = std::abs(value_1 - value_2);
    
    return difference < EPSILON;
}

inline long mq_get_timestamp_ms()
{
    clock_t timestamp = clock() * CLK_TCK;
    
    return (long)(timestamp / 1000);
}


#endif //SYSTEM_UTILITIES_H
