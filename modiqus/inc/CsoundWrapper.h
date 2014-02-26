/*
 * Copyright (C) 2013 by Martin Dejean
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

#ifndef __CSOUND_WRAPPER_H__
#define __CSOUND_WRAPPER_H__

#include "csound.h"
#include "Configuration.h"

namespace modiqus
{
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
        bool start();
        
        void stop();
        
        void getChannelControlOutput(MYFLT& value, const char *name) const;
        
        void setChannelControlInput(MYFLT value, const char *name) const;
        
        void sendMessage(const char* message) const;
        
        void sendScoreEvent(const char type, MYFLT* parameters, S32 numParameters);
                
        void createSampleTable(mqSampleTable& table);
        
        void createImmediateTable(mqImmediateTable& table);
        
        void createSegmentTable(mqSegmentTable& table);
        
        const S32 getTableData(const S32 tableNumber, F32List* const data);
        
        void setTableData(const S32 table, const F32List* const data);
        
        const F32 getTableValue(const S32 table, const S32 index);

        bool tableExists(S32 tableNumber);
        
        void deleteTable(const S32 tableNumber);
        
        const S32 getSampleRate() const;
        
        const S32 getControlRate() const;
        
        const S32 getNumberOfControlSamples() const;
        
        const F32 getControlPeriodDuration() const;
        
        bool performanceThreadRunning() const;
        
        void performanceThreadRunning(bool running);
        
    private:
        String _csdPath;
        CsoundState _state;
        volatile bool _performanceThreadRunning;
        F32 _ksmpsDuration;
    };
}
#endif //__CSOUND_WRAPPER_H__
