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

#ifndef __MQ_UTILS_SYSTEM_H__
#define __MQ_UTILS_SYSTEM_H__

#include <mach-o/dyld.h>
#include <sstream>
#include <stdexcept>
#include <sys/time.h>
#include "mq_debug.h"

namespace mq
{
    template<class T>
    static mq_str toString(const T& input)
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
    static const char* toCString(const T& input)
    {
        std::ostringstream stream;
        stream << input;
        
        return stream.str().c_str();
    }
    
    template<class T>
    static T fromString(const mq_str& input)
    {
        std::istringstream stream(input);
        T output;
        stream >> output;
        
        return output;
    }
    
    template<class T>
    static T fromString(const char* input)
    {
        std::istringstream stream(input);
        T output;
        stream >> output;
        
        return output;
    }
    
    static U32 sizeToUnsignedInt(USize value)
    {
        if(value > UINT_MAX) {
            MQ_LOG(MQ_LOG_ERROR, "Value is bigger than UINT_MAX. Return value will not be correct.")
        }
        
        return static_cast<U32>(value);
    }
    
    static const S32 sizeToInt(const USize value)
    {
        if (value > std::numeric_limits<S32>::max()) {
            MQ_LOG(MQ_LOG_ERROR, "Size is bigger than INT_MAX. Return value will not be correct.");
        }
        
        return static_cast<S32>(value);
    }
    
    static const S32 longToInt(const S64 value)
    {
        if (value < std::numeric_limits<S32>::max()) {
            return static_cast<S32>(value);
        } else {
            MQ_LOG(MQ_LOG_ERROR, "Size is bigger than INT_MAX")
            return -1;
        }
    }

    static void randomSeed()
    {
        timeval time;
        gettimeofday(&time,NULL);
        srand((U32)(time.tv_sec * 1000) + (time.tv_usec / 1000));
    }
    
    static void pause(USize seconds)
    {
        clock_t goal = clock() + seconds * CLOCKS_PER_SEC;
        while (goal > clock());
    }
    
    template<typename T1, typename T2>
    static T2* mapGet(const T1& key, std::map<T1,T2>& map)
    {
        T2* elmPtr = NULL;
        
        try {
            elmPtr = &map.at(key);
        } catch (const std::out_of_range& oor) {
            MQ_LOG(MQ_LOG_ERROR, "Out of Range error: " + mq_str(oor.what()) + ", key: " + toString(key))
            MQ_LOG(MQ_LOG_ERROR, "Returning NULL pointer")
        }
        
        return elmPtr;
    }
    
    template<typename T1, typename T2>
    static T2* mapInsert(std::map<T1,T2>& map, typename std::pair<T1,T2>& mapPair)
    {
        std::pair<typename std::map<T1,T2>::iterator, bool> retVal = map.insert(mapPair);
        
        if (!retVal.second) {
            MQ_LOG(MQ_LOG_WARN, "Element with key " + toString<T1>(mapPair.first) + " already exists.")
            return &retVal.first->second;
        }
        
        return mapGet(mapPair.first, map);
    }

    static mq_str getExecutablePath()
    {
#ifdef __APPLE__
        char path[PATH_MAX];
        uint32_t size = sizeof(path);
        if (_NSGetExecutablePath(path, &size) == 0) {
//            MQ_LOG(MQ_LOG_DBG, "Executable path is " + mq_str(path))   
        } else {
            MQ_LOG(MQ_LOG_ERROR, "Buffer too small; need size " + toString<S32>(size))
        }

        return mq_str(path);
#else
    //    char temp[PATH_MAX];
    //    return (getcwd(temp, PATH_MAX) ? mq_str(temp) : mq_str(""));
        return "";
#endif
    }
    
    static mq_str getBundleContentsPath()
    {
        mq_str path = getExecutablePath();
        mq_str contentsPath = mq_str(path);
        USize slashIdx = contentsPath.rfind("/");
        contentsPath = contentsPath.substr(0, slashIdx - 1);
        slashIdx = contentsPath.rfind("/");
        contentsPath = contentsPath.substr(0, slashIdx - 1);        
        return contentsPath;
    }

    static mq_str getBundleFrameworksPath()
    {
        mq_str contentsPath = getBundleContentsPath();
        return contentsPath + "/Frameworks";
    }

    static mq_str getBundleResourcesPath()
    {
        mq_str contentsPath = getBundleContentsPath();
        return contentsPath + "/Resources";
    }
    
    static mq_str getConfigPath()
    {
        mq_str configPath = "";
        mq_str contentsPath = getBundleContentsPath();
        USize slashIdx = contentsPath.rfind("/");
        configPath = contentsPath.substr(0, slashIdx - 1);
        slashIdx = configPath.rfind("/");
        configPath = configPath.substr(0, slashIdx);
        configPath += "/config";
        
        return configPath;
    }
    
//    static mq_str getAudioPath()
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
    
    static S32 findListIndex(mq_str str, const mq_str* strList, S32 size)
    {
        S32 index = -1;
        
        for (S32 i = 0; i < size; i++) {
            if (str.compare(strList[i]) == 0) {
                index = i;
                break;
            }
        }
        
        if (index == -1) {
            MQ_LOG(MQ_LOG_ERROR, "Index of string '" + str + "' not found");
        }
        
        return index;
    }

    static S32 findListIndex(mq_str str, const char** strList, S32 size)
    {
        S32 index = -1;
        
        for (S32 i = 0; i < size; i++) {
            if (str.compare(strList[i]) == 0) {
                index = i;
                break;
            }
        }
        
        if (index == -1) {
            MQ_LOG(MQ_LOG_ERROR, "Index of string '" + str + "' not found");
        }
        
        return index;
    }

    static S32 findListIndex(const mq_str& str, const StringList& strList)
    {
        S32 index = -1;
        USize numElements = strList.size();
        
        for (S32 i = 0; i < numElements; i++) {
            if (str.compare(strList[i]) == 0) {
                index = i;
                break;
            }
        }
        
        if (index == -1) {
            MQ_LOG(MQ_LOG_ERROR, "Index of string '" + str + "' not found");
        }
        
        return index;
    }
    
    static bool floatEquality(const F32 value1, const F32 value2)
    {
        const F32 difference = std::abs(value1 - value2);
        
        return difference < F_EPSILON;
    }
    
    static long getTimeStampMs()
    {
        clock_t timestamp = clock() * CLK_TCK;
        
        return (long)(timestamp / 1000);
    };
}

#endif //__MQ_UTILS_SYSTEM_H__
