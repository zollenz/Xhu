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

#ifndef TYPES_H
#define TYPES_H

#include <climits>
#include <map>
#include <string>
#include <vector>

namespace modiqus {
#ifdef __LP64__
    typedef unsigned long USize;
#else
    typedef unsigned int USize;
#endif
    typedef uint8_t U8;
    typedef int8_t S8;
    typedef uint16_t U16;
    typedef int16_t S16;
    typedef uint32_t U32;
    typedef int32_t S32;
    typedef uint64_t U64;
    typedef int64_t S64;
    typedef float F32;
    typedef double F64;
    typedef U8 Byte;
    typedef size_t MemSize;
    typedef wchar_t WChar;
    typedef std::string String;
    typedef std::vector<String> StringList;
    typedef std::vector<S32> S32List;
    typedef std::vector<F32> F32List;
    
    static const S32 SRATE = 44100;
    static const S32 KRATE = 4410;
    static const S32 KSMPS = SRATE / KRATE;
    static const S32 BUFFER_SIZE = 512;
    static const S32 INDEX_INVALID = -1;
    static const S32 TABLE_UNDEFINED = 0;
    static const String UNDEFINED_STR = "UNDEFINED";
    static const S32 UNDEFINED_INT = -1;
    static const S32 HAS_DATA = 1;
    static const F32 EPSILON = 0.001f;
    
    struct Vector2 {
        S32 x;
        S32 y;
        
        Vector2(S32 _x = 0, S32 _y = 0) : x(_x), y(_y) {}
    };
    
    struct Segment {
        F32 value;
        F32 length;
        
        Segment(F32 _value = 0.0f, F32 _length = 0.0f)
        : value(_value), length(_length) {}
    };
    
    typedef std::vector<Vector2> Vector2List;
    typedef std::vector<Segment> SegmentList;
    
    struct SoundInfo {
        String sourceType;
        String sourceName;
        String sourceEvent;
        S32 soundInstance;
        String soundInstanceString;
        String soundCompleteName;
        
        SoundInfo(const String &sourcetype_ = UNDEFINED_STR,
                    const String &sourceName_ = UNDEFINED_STR,
                    const String &sourceEvent_ = UNDEFINED_STR,
                    const S32 soundInstance_ = UNDEFINED_INT,
                    const String &soundInstanceString_ = UNDEFINED_STR,
                    const String &soundCompleteName_ = UNDEFINED_STR) :
        sourceType(sourcetype_),
        sourceName(sourceName_),
        sourceEvent(sourceEvent_),
        soundInstance(soundInstance_),
        soundInstanceString(soundInstanceString_),
        soundCompleteName(soundCompleteName_) {}
        
        void reset() {
            sourceType = UNDEFINED_STR;
            sourceName = UNDEFINED_STR;
            sourceEvent = UNDEFINED_STR;
            soundInstance = UNDEFINED_INT;
            soundInstanceString = UNDEFINED_STR;
            soundCompleteName = UNDEFINED_STR;
        }
    };
    
    struct ParamUpdate {
        String name;
        F32 value;
        
        ParamUpdate(const String &n = UNDEFINED_STR, const F32 v = 0.0f) : name(n), value(v) {}
    };
} // namespace modiqus

#endif //TYPES_H
