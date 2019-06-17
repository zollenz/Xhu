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

#ifndef CSOUND_WRAPPER_H
#define CSOUND_WRAPPER_H

#include "csound.h"

struct Table
{
    mq_s32_t number;
    mq_s32_t start;
    mq_s32_t size;
    mq_s32_t GENRoutine;

    Table();

    void reset();
};

struct SampleTable : public Table
{
    mq_str_t filcod;
    mq_f32_t skiptime;
    mq_s32_t format;
    mq_s32_t channel;

    SampleTable();

    void reset();
};

struct ImmediateTable : public Table
{
    mq_s32_list_t tableNums;

    ImmediateTable();

    void reset();
};

struct SegmentTable : public Table
{
    mq_segment_list_t segments;
    
    SegmentTable();
    
    void reset();
};

typedef struct {
    CSOUND* csound;
    mq_s32_t compileResult;
    bool runPerformanceThread;
    bool yieldPerformance;
    bool performanceThreadYield;
} mq_csound_state_t;

class CsoundWrapper
{
public:
    
    void setlogLevel(mq_s32_t level) const;
    bool start(bool bundle);
    void stop();
    void setOpcodePath(mq_str_t path);
    void setAudioPath(mq_str_t path);
    void setCsdPath(mq_str_t path);
    void getChannelControlOutput(MYFLT& value, const char *name) const;
    void setChannelControlInput(MYFLT value, const char *name) const;
    void setControlChannelInput(MYFLT value, const char *name) const;
    void sendMessage(const char* message) const;
    void sendScoreEvent(const char type, MYFLT* parameters, mq_s32_t numParameters);
    void createSampleTable(SampleTable* const table);
    void createImmediateTable(ImmediateTable* const table);
    void createSegmentTable(SegmentTable* const table);
    const mq_s32_t getTableData(const mq_s32_t tableNumber, mq_f32_list_t* const data);
    void setTableData(const mq_s32_t table, const mq_f32_list_t* const data);
    const mq_f32_t getTableValue(const mq_s32_t table, const mq_s32_t index);
    bool doesTableExist(const mq_s32_t tableNumber);
    void deleteTable(const mq_s32_t tableNumber);
    const mq_s32_t getSampleRate() const;
    const mq_s32_t getControlRate() const;
    const mq_s32_t getNumberOfControlSamples() const;
    const mq_f32_t getControlPeriod() const;
    bool isPerformanceThreadRunning() const;
    void isPerformanceThreadRunning(bool running);
    
private:
    
    mq_csound_state_t _state;
    volatile bool _performanceThreadRunning;
//        F32 _ksmpsDuration;
    
};
#endif //CSOUND_WRAPPER_H
