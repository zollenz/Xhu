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

namespace modiqus
{
    struct Table
    {
        S32 number;
        S32 start;
        S32 size;
        S32 GENRoutine;

        Table();

        void reset();
    };

    struct SampleTable : public Table
    {
        String filcod;
        F32 skiptime;
        S32 format;
        S32 channel;

        SampleTable();

        void reset();
    };

    struct ImmediateTable : public Table
    {
        S32List tableNums;

        ImmediateTable();

        void reset();
    };

    struct SegmentTable : public Table
    {
        SegmentList segments;
        
        SegmentTable();
        
        void reset();
    };

    struct CsoundState
    {
        CSOUND* csound;
        S32 compileResult;
        bool runPerformanceThread;
        bool yieldPerformance;
        bool performanceThreadYield;
    };
    
    class CsoundWrapper
    {
        
    public:
        
        void setlogLevel(S32 level) const;
        bool start(bool bundle);
        void stop();
        void setOpcodePath(String path);
        void setAudioPath(String path);
        void setCsdPath(String path);
        void getChannelControlOutput(MYFLT& value, const char *name) const;
        void setChannelControlInput(MYFLT value, const char *name) const;
        void setControlChannelInput(MYFLT value, const char *name) const;
        void sendMessage(const char* message) const;
        void sendScoreEvent(const char type, MYFLT* parameters, S32 numParameters);
        void createSampleTable(SampleTable* const table);
        void createImmediateTable(ImmediateTable* const table);
        void createSegmentTable(SegmentTable* const table);
        const S32 getTableData(const S32 tableNumber, F32List* const data);
        void setTableData(const S32 table, const F32List* const data);
        const F32 getTableValue(const S32 table, const S32 index);
        bool doesTableExist(const S32 tableNumber);
        void deleteTable(const S32 tableNumber);
        const S32 getSampleRate() const;
        const S32 getControlRate() const;
        const S32 getNumberOfControlSamples() const;
        const F32 getControlPeriod() const;
        bool isPerformanceThreadRunning() const;
        void isPerformanceThreadRunning(bool running);
        
    private:
        
        CsoundState _state;
        volatile bool _performanceThreadRunning;
//        F32 _ksmpsDuration;
        
    };
}
#endif //CSOUND_WRAPPER_H
