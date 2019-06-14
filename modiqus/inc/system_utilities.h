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

namespace modiqus
{
    template<class T>
    inline String toString(const T& input)
    {
        std::ostringstream stream;
        stream << input;
        
        //        if (stream.fail()) {
        //            std::runtime_error e(numberAsString);
        //            throw e;
        //        }
        
        return stream.str();
    }
    
    template<class T>
    inline const char* toCString(const T& input)
    {
        std::ostringstream stream;
        stream << input;
        
        return stream.str().c_str();
    }
    
    template<class T>
    inline T fromString(const String& input)
    {
        std::istringstream stream(input);
        T output;
        stream >> output;
        
        return output;
    }
    
    template<class T>
    inline T fromString(const char* input)
    {
        std::istringstream stream(input);
        T output;
        stream >> output;
        
        return output;
    }
   
    inline const S32 sizeToInt(const USize value)
    {
        if (value <= INT_MAX)
        {
            return static_cast<S32>(value);
        }

        MQ_LOG_ERROR("Size is bigger than INT_MAX. Returning -1");
        return -1;
    }
    
    inline void pause(USize seconds)
    {
        clock_t goal = clock() + seconds * CLOCKS_PER_SEC;
        while (goal > clock());
    }
    
    template<typename T1, typename T2>
    inline T2* mapGet(const T1& key, std::map<T1,T2>& map)
    {
        T2* elmPtr = NULL;
        
        try
        {
            elmPtr = &map.at(key);
        }
        catch (const std::out_of_range& oor)
        {
            MQ_LOG_ERROR("Out of Range error: " + String(oor.what()) + ", key: " + toString(key))
            MQ_LOG_ERROR("Returning NULL pointer")
        }
        
        return elmPtr;
    }
    
    template<typename T1, typename T2>
    inline T2* mapInsert(std::map<T1,T2>& map, typename std::pair<T1,T2>& mapPair)
    {
        std::pair<typename std::map<T1,T2>::iterator, bool> retVal = map.insert(mapPair);
        
        if (!retVal.second)
        {
            MQ_LOG_WARN("Element with key " + toString<T1>(mapPair.first) + " already exists.")
            
            return &retVal.first->second;
        }
        
        return mapGet(mapPair.first, map);
    }

    inline String getExecutablePath()
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

        return String(path);
#else
    //    char temp[PATH_MAX];
    //    return (getcwd(temp, PATH_MAX) ? mq_str(temp) : mq_str(""));
        return "";
#endif
    }
    
    inline String getBundleContentsPath()
    {
        String path = getExecutablePath();
        String contentsPath = String(path);
        USize slashIdx = contentsPath.rfind("/");
        contentsPath = contentsPath.substr(0, slashIdx - 1);
        slashIdx = contentsPath.rfind("/");
        contentsPath = contentsPath.substr(0, slashIdx - 1);
        
        return contentsPath;
    }

    inline String getBundleFrameworksPath()
    {
        String contentsPath = getBundleContentsPath();
        
        return contentsPath + "/Frameworks";
    }

    inline String getBundleResourcesPath()
    {
        String contentsPath = getBundleContentsPath();
        
        return contentsPath + "/Resources";
    }
    
    inline String getConfigPath()
    {
        String configPath = "";
        String contentsPath = getBundleContentsPath();
        USize slashIdx = contentsPath.rfind("/");
        
        configPath = contentsPath.substr(0, slashIdx - 1);
        slashIdx = configPath.rfind("/");
        configPath = configPath.substr(0, slashIdx);
        configPath += "/config";
        
        return configPath;
    }
    
//    inline mq_str getAudioPath()
//    {
//        mq_str audioPath = "";
//        mq_str contentsPath = getBundleContentsPath();
//        USize slashIdx = contentsPath.rfind("/");
//        audioPath = contentsPath.substr(0, slashIdx - 1);
//        slashIdx = audioPath.rfind("/");
//        audioPath = audioPath.substr(0, slashIdx);
//        audioPath += "/audio";
//        
//        return audioPath;
//    }
    
    inline bool floatEquality(const F32 value1, const F32 value2)
    {
        const F32 difference = std::abs(value1 - value2);
        
        return difference < EPSILON;
    }
    
    inline long getTimeStampMs()
    {
        clock_t timestamp = clock() * CLK_TCK;
        
        return (long)(timestamp / 1000);
    }
}

#endif //SYSTEM_UTILITIES_H
