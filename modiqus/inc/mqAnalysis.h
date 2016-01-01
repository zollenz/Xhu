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

#ifndef __MQ_ANALYSIS__
#define __MQ_ANALYSIS__

#include <vector>
#include <map>
#include "mq_types.h"

using std::pair;

namespace mq
{
    typedef struct
    {
        float sampleDuration;
        float samplePosition;
        int startIndex;
        int endIndex;
        int tableNum;
    } AnalysisData;

    class mqAnalysis
    {
        
    public:
        
        mqAnalysis()
        {
            _audio_vector = NULL;
        };
        
        ~mqAnalysis();
        void Clear();
        void SetAudio(F32List* audio_vector);
        void ComputeSpectralFlux(float* result);
        void DoAmplitudeAnalysis(F32List* amplitudeDelta, F32List* amplitudeMean);
        AnalysisData Analyze(int tablenum, float spectralDelta, float amplitudeDelta, int minLength, int maxLength);
        
    private:
        
        pair<float, pair<int, int> > FindSequenceIteratively(F32List* v, int startIndex, int endIndex, int minLength, int maxLength);
        float _threshold;
        F32List* _audio_vector;
        
    };
}
#endif // __MQ_ANALYSIS__
