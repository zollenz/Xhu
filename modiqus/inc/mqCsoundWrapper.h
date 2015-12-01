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

#ifndef __MQ_CSOUND_WRAPPER_H__
#define __MQ_CSOUND_WRAPPER_H__

#include "csound.h"
#include "mqConfiguration.h"

namespace mq
{
    struct CsoundState
    {
        CSOUND* csound;
        S32 compileResult;
        bool runPerformanceThread;
        bool yieldPerformance;
        bool performanceThreadYield;
    };
    
    class mqCsoundWrapper
    {
    public:        
        bool Start();
        void Stop();
        void GetChannelControlOutput(MYFLT& value, const char *name) const;
        void SetChannelControlInput(MYFLT value, const char *name) const;
        void SetControlChannelInput(MYFLT value, const char *name) const;
        void SendMessage(const char* message) const;
        void SendScoreEvent(const char type, MYFLT* parameters, S32 numParameters);
        void CreateSampleTable(mqSampleTable& table);
        void CreateImmediateTable(mqImmediateTable& table);
        void CreateSegmentTable(mqSegmentTable& table);
        const S32 GetTableData(const S32 tableNumber, F32List* const data);
        void SetTableData(const S32 table, const F32List* const data);
        const F32 GetTableValue(const S32 table, const S32 index);
        bool DoesTableExist(S32 tableNumber);
        void DeleteTable(const S32 tableNumber);
        const S32 GetSampleRate() const;
        const S32 GetControlRate() const;
        const S32 GetNumberOfControlSamples() const;
        const F32 GetControlPeriodDuration() const;
        bool IsPerformanceThreadRunning() const;
        void IsPerformanceThreadRunning(bool running);
    private:
        CsoundState _state;
        volatile bool _performanceThreadRunning;
        F32 _ksmpsDuration;
    };
}
#endif //__MQ_CSOUND_WRAPPER_H__
