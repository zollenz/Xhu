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

#ifndef TYPES_HPP
#define TYPES_HPP

#include <limits>
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
    static const F32 F_EPSILON = 0.001f;
    
#define F32_INF std::numeric_limits<F32>::infinity()
#define F64_INF std::numeric_limits<F64>::infinity()
    
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
    
    // Tuple templates
    template <typename T1, typename T2> struct Tuple2 {
        T1 value1;
        T2 value2;
        
        Tuple2(T1 v1 = NULL, T2 v2 = NULL) {
            value1 = v1;
            value2 = v2;
        }
    };
    
    template <typename T1, typename T2, typename T3> struct Tuple3 {
        T1 value1;
        T2 value2;
        T3 value3;
        
        Tuple3(T1 v1 = NULL, T2 v2 = NULL, T3 v3 = NULL) {
            value1 = v1;
            value2 = v2;
            value3 = v3;
        }
    };
    
    template <typename T1, typename T2, typename T3, typename T4> struct Tuple4 {
        T1 value1;
        T2 value2;
        T3 value3;
        T4 value4;
        
        Tuple4(T1 v1 = NULL, T2 v2 = NULL, T3 v3 = NULL, T4 v4 = NULL) {
            value1 = v1;
            value2 = v2;
            value3 = v3;
            value4 = v4;
        }
    };
    
    template <typename T1, typename T2, typename T3, typename T4, typename T5>
    struct Tuple5 {
        T1 value1;
        T2 value2;
        T3 value3;
        T4 value4;
        T5 value5;
        
        Tuple5(T1 v1 = NULL, T2 v2 = NULL, T3 v3 = NULL, T4 v4 = NULL, T5 v5 = NULL) {
            value1 = v1;
            value2 = v2;
            value3 = v3;
            value4 = v4;
            value5 = v5;
        }
    };
    
    template <typename T1, typename T2, typename T3, typename T4, typename T5,
    typename T6>
    struct Tuple6 {
        T1 value1;
        T2 value2;
        T3 value3;
        T4 value4;
        T5 value5;
        T6 value6;
        
        Tuple6(T1 v1 = NULL, T2 v2 = NULL, T3 v3 = NULL, T4 v4 = NULL, T5 v5 = NULL,
               T6 v6 = NULL) {
            value1 = v1;
            value2 = v2;
            value3 = v3;
            value4 = v4;
            value5 = v5;
            value6 = v6;
        }
    };
    
    template <typename T1, typename T2, typename T3, typename T4, typename T5,
    typename T6, typename T7>
    struct Tuple7 {
        T1 value1;
        T2 value2;
        T3 value3;
        T4 value4;
        T5 value5;
        T6 value6;
        T7 value7;
        
        Tuple7(T1 v1 = NULL, T2 v2 = NULL, T3 v3 = NULL, T4 v4 = NULL, T5 v5 = NULL,
               T6 v6 = NULL, T7 v7 = NULL) {
            value1 = v1;
            value2 = v2;
            value3 = v3;
            value4 = v4;
            value5 = v5;
            value6 = v6;
            value7 = v7;
        }
    };
    
    template <typename T1, typename T2, typename T3, typename T4, typename T5,
    typename T6, typename T7, typename T8>
    struct Tuple8 {
        T1 value1;
        T2 value2;
        T3 value3;
        T4 value4;
        T5 value5;
        T6 value6;
        T7 value7;
        T8 value8;
        
        Tuple8(T1 v1 = NULL, T2 v2 = NULL, T3 v3 = NULL, T4 v4 = NULL, T5 v5 = NULL,
               T6 v6 = NULL, T7 v7 = NULL, T8 v8 = NULL) {
            value1 = v1;
            value2 = v2;
            value3 = v3;
            value4 = v4;
            value5 = v5;
            value6 = v6;
            value7 = v7;
            value8 = v8;
        }
    };
    
    template <typename T1, typename T2, typename T3, typename T4, typename T5,
    typename T6, typename T7, typename T8, typename T9>
    struct Tuple9 {
        T1 value1;
        T2 value2;
        T3 value3;
        T4 value4;
        T5 value5;
        T6 value6;
        T7 value7;
        T8 value8;
        T9 value9;
        
        Tuple9(T1 v1 = NULL, T2 v2 = NULL, T3 v3 = NULL, T4 v4 = NULL, T5 v5 = NULL,
               T6 v6 = NULL, T7 v7 = NULL, T8 v8 = NULL, T9 v9 = NULL) {
            value1 = v1;
            value2 = v2;
            value3 = v3;
            value4 = v4;
            value5 = v5;
            value6 = v6;
            value7 = v7;
            value8 = v8;
            value9 = v9;
        }
    };
} // namespace modiqus

#endif //TYPES_HPP
