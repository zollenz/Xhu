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

#include "mq_types.h"
#include "mqConfiguration.h"

using namespace mq;

const S32 SOUND_PARAM_COUNT = 4;

const String mqMapping::TypeNames[5] =
{
    "Constant",
    "Segment",
    "Range",
    "Mask",
    UNDEFINED_STR
};

mqTable::mqTable() :
number(TABLE_UNDEFINED),
start(0),
size(0),
GENRoutine(0)
{}

void mqTable::Reset()
{
    number = TABLE_UNDEFINED;
    start = 0;
    size = 0;
    GENRoutine = 0;
}

mqSampleTable::mqSampleTable() :
filcod(""),
skiptime(0.0f),
format(0),
channel(0)
{
    GENRoutine = 1;
}

void mqSampleTable::Reset()
{
    mqTable::Reset();
    GENRoutine = 1;
    filcod = "";
    skiptime = 0.0f;
    format = 0;
    channel = 0;
}

mqImmediateTable::mqImmediateTable()
{
    GENRoutine = -2;
}

void mqImmediateTable::Reset()
{
    mqTable::Reset();
    GENRoutine = -2;
    tableNums.clear();
}

mqSegmentTable::mqSegmentTable()
{
    size = TABLE_SIZE_MEDIUM;
    GENRoutine = -7;
    segments.reserve(MAX_SEGMENTS);
}

void mqSegmentTable::Reset()
{
    mqTable::Reset();
    size = TABLE_SIZE_MEDIUM;
    GENRoutine = -7;
    segments.clear();
}

mqModifier::mqModifier() : controlValue(0.0f) {}

void mqModifier::Reset()
{
    controlValue = 0.0f;
    minTable.Reset();
    maxTable.Reset();
}

mqMapping::mqMapping() :
type(UNDEFINED),
controlParam(NULL)
{
    morphMinTableTable.size = MAX_MAPPINGS;
    morphMaxTableTable.size = MAX_MAPPINGS;
    morphIntraTableTable.size = 2;
    modifiers.reserve(MAX_MAPPINGS);
}

void mqMapping::Reset()
{
    type = UNDEFINED;
    controlParam = NULL;
    morphMinTable.Reset();
    morphMaxTable.Reset();
    morphIntraTable.Reset();
    morphMinTableTable.Reset();
    morphMaxTableTable.Reset();
    morphIntraTableTable.Reset();
    morphMinTableTable.size = MAX_MAPPINGS;
    morphMaxTableTable.size = MAX_MAPPINGS;
    morphIntraTableTable.size = 2;
    modifiers.clear();
}

mqSound::mqSound() : name(UNDEFINED_STR), grainStart(-1.0f), grainEnd(-1.0f), grainDuration(-1)
{
    mappings.resize(SOUND_PARAM_COUNT);
}

void mqSound::Reset()
{
    name = UNDEFINED_STR;
    grainStart = -1.0f;
    grainEnd = -1.0f;
    grainDuration = -1;
    grainWaveTable.Reset();
    
    for (S32 i = 0; i < SOUND_PARAM_COUNT; i++) {
        mappings[i].Reset();
    }
}

mqControlParam::mqControlParam() :
name(UNDEFINED_STR),
min(0.0f),
max(1.0f),
value(0.5f)
{}

void mqControlParam::Reset()
{
    min = 0.0f;
    max = 1.0f;
    value = 0.5f;
}

mqConfiguration::mqConfiguration()
{
    name = UNDEFINED_STR;
    baseTableNumber = TABLE_UNDEFINED;
}

void mqConfiguration::Reset()
{
    name = UNDEFINED_STR;
    sounds.clear();
    controlParams.clear();
    baseTableNumber = TABLE_UNDEFINED;    
}
