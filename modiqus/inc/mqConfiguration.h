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

#ifndef __MQ_CONFIGURATION_H__
#define __MQ_CONFIGURATION_H__

#include "mq_types.h"

namespace mq
{
    static const S32 MAX_MAPPINGS = 8;
    static const S32 MAX_SEGMENTS = 8;
    static const S32 TABLE_SIZE_TINY = 128;
    static const S32 TABLE_SIZE_SMALL = 256;
    static const S32 TABLE_SIZE_MEDIUM = 512;
    static const S32 TABLE_SIZE_LARGE = 1025;
        
    struct mqTable
    {
        S32 number;
        S32 start;
        S32 size;
        S32 GENRoutine;

        mqTable();
        
        void reset();
    };
    
    struct mqSampleTable : public mqTable
    {
        mq_str filcod;
        F32 skiptime;
        S32 format;
        S32 channel;
        
        mqSampleTable();
        
        void reset();
    };
    
    struct mqImmediateTable : public mqTable
    {
        S32List tableNums;
        
        mqImmediateTable();
        
        void reset();
    };
    
    struct mqSegmentTable : public mqTable
    {
        SegmentList segments;
        
        mqSegmentTable();
        
        void reset();
    };
    
    struct mqControlParam
    {
        mq_str name;
        F32 min;
        F32 max;
        F32 value;
        
        mqControlParam();
        
        void reset();
    };
    
    struct mqModifier
    {
        F32 controlValue;
        mqSegmentTable minTable;
        mqSegmentTable maxTable;

        mqModifier();
        
        void reset();
    };
    
    struct mqMapping
    {
        static const mq_str TypeNames[5];
        
        enum Type
        {
            CONSTANT,
            SEGMENT,
            RANGE,
            MASK,
            UNDEFINED,
            TYPE_COUNT
        };
        
        mqMapping::Type type;
        mqControlParam* controlParam;
        mqSegmentTable morphMinTable;
        mqSegmentTable morphMaxTable;
        mqSegmentTable morphIntraTable;
        mqImmediateTable morphMinTableTable;
        mqImmediateTable morphMaxTableTable;
        mqImmediateTable morphIntraTableTable;

        std::vector<mqModifier> modifiers;
        
        mqMapping();
        
        void reset();
    };
    
    struct mqSound
    {
        mq_str name;
        F32 grainStart;
        F32 grainEnd;
        S32 grainDuration;
        mqSampleTable grainWaveTable;
        std::vector<mqMapping> mappings;
        
        mqSound();
        
        void reset();
    };
        
    typedef std::map<mq_str, mqSound> mqSoundMap;
    typedef std::pair<mq_str, mqSound> mqSoundMapPair;
    typedef std::map<mq_str, mqControlParam> mqControlParamMap;
    typedef std::pair<mq_str, mqControlParam> mqControlParamMapPair;
    
    struct mqConfiguration
    {
        mq_str name;
        mqSoundMap sounds;
        mqControlParamMap controlParams;
        U32 baseTableNumber;
        
        mqConfiguration();
        
        void reset();
    };
}
#endif //__MQ_CONFIGURATION_H__
