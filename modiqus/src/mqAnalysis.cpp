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

#include <cmath>
#include "mq_utils_math.h"
#include "mqAnalysis.h"

using namespace mq;

mqAnalysis::~mqAnalysis()
{
    if (_audio_vector != NULL)
    {
        delete _audio_vector;
    }
}

void mqAnalysis::SetAudio(F32List* audio_vector)
{
    if (_audio_vector)
    {
        delete _audio_vector;
    }
    //Normalize(audio_vector, 1);
    _audio_vector = audio_vector;
}

//F32List* mqAnalysis::DoSpectralFlux()
//{
//    F32List* result = new F32List();
//    //TODO: reserve the result vector size.
//    F32List* subVector = new F32List();
//    F32List* spectrum1 = new F32List();
//    F32List* spectrum2 = new F32List();
//    F32List* temporary;
//
//    for (unsigned int index = 0; index < _audio_vector->size(); index += _windowSize) {
//        int upper = Min<int>(_audio_vector->size(), index + _windowSize);
//        getSubVector<float>(_audio_vector, subVector, index, upper);
//        computeFFT(subVector, spectrum1);
//        //TODO: Normalize the spectrum!
//        Normalize(spectrum1, 1);
//
//        if (!spectrum2->empty()) {
//            //Compute euclidean distance:
//            result->push_back(MathUtilities::EuclideanDistance(spectrum1, spectrum2));
//        }
//        temporary = spectrum2;
//        spectrum2 = spectrum1;
//        spectrum1 = temporary;
//    }
//
//    if (subVector != NULL) {
//        delete subVector;
//    }
//
//    if (spectrum1 != NULL) {
//        delete spectrum1;
//    }
//
//    if (spectrum2 != NULL) {
//        delete spectrum2;
//    }
//
//    return result;
//}

mqAnalysis::ComputeSpectralFlux(float* result)
{
    float* result[8];
    
    return 0;
}

void mqAnalysis::DoAmplitudeAnalysis(F32List* amplitudeDelta, F32List* amplitudeMean)
{
    if (amplitudeDelta == NULL)
    {
        amplitudeDelta = new F32List();
    }
    
    if (amplitudeMean == NULL)
    {
        amplitudeMean = new F32List();
    }
    
    amplitudeDelta->resize(0);
    amplitudeMean->resize(0);
    
    float mean1 = 0;
    float mean2 = -1;
    
    for (unsigned int index = 0; index < _audio_vector->size(); index += _windowSize)
    {
        int upper = min<int>((unsigned int)_audio_vector->size(), index + _windowSize);
        mean1 = AbsMean(_audio_vector, index, upper);
        
        if (mean2 >= 0)
        {
            amplitudeMean->push_back(mean2);
            amplitudeDelta->push_back(abs(mean2 - mean1));
        }
        
        mean2 = mean1;
    }
}

void mqAnalysis::Clear()
{
    if (_audio_vector != NULL)
    {
        delete _audio_vector;
    }
}

float mqAnalysis::AbsMean(F32List* v, int startIndex, int endIndex)
{
    if (endIndex == 0)
    {
        endIndex = v->size();
    }
    
    if (startIndex > endIndex)
    {
        printf("Fatal mistake. Start index is higher than end index.\n");
        exit(EXIT_FAILURE);
    }
    
    float mean = 0.0f;
    
    for (unsigned int i = startIndex; i < endIndex; i++)
    {
        mean += abs(v->at(i));
    }
    
    mean = mean / (endIndex - startIndex);
    
    return mean;
}

AnalysisData mqAnalysis::Analyze(int tablenum, float spectralDeltaW, float amplitudeDeltaW, int minLength, int maxLength)
{
    //    _audio_vector = theAudio.GetFunctionTable(tablenum);
//    F32List* spectralFlux = ComputeSpectralFlux();
//    F32List *amplitudeDelta = new F32List(), *amplitudeMean = new F32List();
//    DoAmplitudeAnalysis(amplitudeDelta, amplitudeMean);
//    printf("Spectral flux size: %d\nAmplitude mean size: %d\nAmplitude Delta size: %d\n",
//           (int)spectralFlux->size(), (int)amplitudeMean->size(), (int)amplitudeDelta->size());
//    F32List *analysis = new F32List(spectralFlux->size());
//    
//    for (unsigned int i = 0; i < spectralFlux->size(); i++) {
//        
//        if (amplitudeMean->at(i) < .001f) {
//            //We have found silence!
//            analysis->at(i) = 100000;
//        } else {
//            analysis->at(i) = (spectralFlux->at(i) * spectralDeltaW) + (amplitudeDelta->at(i) * amplitudeDeltaW);
//        }
//    }
//    
//    delete spectralFlux;
//    delete amplitudeDelta;
//    delete amplitudeMean;
//    pair<float, pair<int, int> > sequence = FindSequenceIteratively(analysis, 0, analysis->size(), minLength, maxLength);
//    printf("Iterative analysis method yields, start index %d and end index %d.\n",
//           sequence.second.first, sequence.second.second);
//    
//    //Get the start and end points:
//    int startIndex = sequence.second.first * _windowSize;
//    int endIndex = (sequence.second.second * _windowSize);
//    if (endIndex > _audio_vector->size()) {
//        endIndex = sizeToInt(_audio_vector->size());
//    }
//    
//    //Calculate duration in milliseconds:
//    int samples = endIndex - startIndex;
//    float milliseconds = (float)samples / 44.1f;
//    printf("Milliseconds: %f", milliseconds);
    AnalysisData result;
//    result.startIndex = startIndex;
//    result.endIndex = endIndex;
//    result.tableNum = tablenum;
//    result.sampleDuration = milliseconds;
//    result.samplePosition = (float)startIndex / (float)_audio_vector->size();
//    delete _audio_vector;
//    
    return result;
}

//	- runs in O(n^2)
pair<float, pair<int, int> > mqAnalysis::FindSequenceIteratively(F32List* v, int startIndex, int endIndex, int minLength, int maxLength)
{
//    int lengths = maxLength - minLength;
//    std::vector<pair<float, float> > sequences;
//    std::vector<pair<int, int> > indices;
//    float mean = 0;
//    float max = 0;
//    int nextIndex = 0;
//    int length = 0;
//    for (unsigned int l = 0; l < lengths; l++) {
//        //Current length:
//        length = l + minLength;
//        for (unsigned int i = startIndex; i < endIndex - (length - 1); i++) {
//            if (i == startIndex) {
//                //First loop:
//                for (unsigned int u = i; u < (length + startIndex); u++) {
//                    
//                    if (max < v->at(u)){
//                        max = v->at(u);
//                    }
//                    mean += v->at(u);
//                }
//            } else {
//                //Consecutive loops:
//                nextIndex = (i + length) - 1;
//                mean += v->at(nextIndex) - v->at(i - 1);
//                
//                if (max == v->at(i - 1) ) {
//                    //In the worst case, this could make the algorithm run in O(n^2)
//                    max = 0;
//                    
//                    for (unsigned int u = i; u < length; u++) {
//                        if (max < v->at(u)){
//                            max = v->at(u);
//                        }
//                    }
//                }
//            }
//            
//            pair<float, float> evaluation = std::make_pair(mean / (float)length, max);
//            sequences.push_back(evaluation);
//            indices.push_back(std::make_pair(i, i + length));
//        }
//    }
//    
//    pair<float, unsigned int> smallestmean(1000.0f, 0), smallestpeak(1000.0f, 0);
//    
//    //	Init values:
//    for (unsigned int i = 0; i < sequences.size(); i++) {
//        if (smallestmean.first > sequences[i].first) {
//            smallestmean.first = sequences[i].first;
//            smallestmean.second = i;
//        } else if (smallestmean.first == sequences[i].first) {
//            if (sequences[smallestmean.second].second > sequences[i].second) {
//                smallestmean.first = sequences[i].first;
//                smallestmean.second = i;
//            }
//        }
//    }
//    
//    return std::make_pair(smallestmean.first, indices[smallestmean.second]);
}