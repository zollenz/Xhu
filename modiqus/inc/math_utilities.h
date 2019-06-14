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

#ifndef MATH_UTILITIES_H
#define MATH_UTILITIES_H

#include <cmath>
#include <cfloat>
#include "system_utilities.h"

namespace modiqus
{
    static const F64 PI = atan(1) * 4;

    template<typename T>
    inline bool isNaN(T value)
    {
        return value != value;    
    }

    template<typename T>
    inline bool isInf(T value)
    {
        return 
            std::numeric_limits<T>::has_infinity &&
            value == std::numeric_limits<T>::infinity();
    }

    template<typename T1, typename T2>
    inline void scaleValues(std::vector<T1>* values, T2 scalar)
    {
        USize numElements = values->size();
        
        for (USize i = 0; i < numElements; i++)
        {
            values->at(i) *= scalar;
        }
    }

    template<typename T1, typename T2>
    inline void divideValuesByScalar(std::vector<T1>* values, T2 scalar)
    {
        USize numElements = values->size();
        
        for (USize i = 0; i < numElements; i++)
        {
            values->at(i) /= scalar;
        }
    }

    template<typename T>
    inline T sum(std::vector<T>* argVector)
    {
        F64 result = 0.0;
        USize numElements = argVector->size();

        for (USize i = 0; i < numElements; i++)
        {
            result += argVector->at(i);
        }

        return result;
    }

    template<typename T>
    inline T lerp(T valueA, T valueB, F32 weight)
    {
        T delta = valueB - valueA;
        
        return valueA + (delta * weight);
    }
    
    inline F32 lerp(F32 x1, F32 y1, F32 x2, F32 y2, F32 xValue)
    {
        return y1 + (xValue - x1) * ((y2 - y1) / (x2 - x1));
    }
    
    template<class T>
    inline T Lerp02(T current, T target, float amount)
    {
        if (current < target)
        {
            current += amount;
            current = (current > target) ? target : current;
        }
        else if (current > target)
        {
            current -= amount;
            current = (current < target) ? target : current;
        }
        
        return current;
    }

    template<typename T>
    inline T min(T valueA, T valueB)
    {
        return valueA < valueB ? valueA : valueB;
    }

    template<typename T>
    inline T max(T valueA, T valueB)
    {
        return valueA > valueB ? valueA : valueB;
    }

    inline F64 mean(const std::vector<F64>& data)
    {
        F64 sum = 0.0;
        USize numValues = data.size();

        for (USize i = 0; i < numValues; i++)
        {
            sum += data[i];
        }

        return sum / numValues;
    }

    inline void normalize(F32* data, const S32 size, const F32 threshold)
    {
        S32 i;
        F32 nextValue = 0.0f;
        F32 maxValue = 0.0f;
        
        for (i = 0; i < size; i++)
        {
            nextValue = std::abs(data[i]);

            if (maxValue < nextValue)
            {
                maxValue = nextValue;
            }
        }

        F32 scale = 0.0f;

        if (maxValue > 0)
        {
            scale = threshold / maxValue;
        }

        for (i = 0; i < size; i++)
        {
            data[i] *= scale;
        }
    }
    
    inline void normalize(F32List& data, const F32 threshold)
    {
        S32 dataSize = sizeToInt(data.size());

        if (dataSize == -1)
        {
            return;
        }
        
        normalize(&data[0], dataSize, threshold);
    }
    
    inline const F32 euclidianDistanceSquared(const F32* const vectorA,
                                 const F32* const vectorB,
                                 const S32 size)
    {        
        F32 singleDist = 0.0f;
        F32 accDist = 0.0f;
        
        for (S32 i = 0; i < size; i++)
        {
            singleDist = vectorA[i] - vectorB[i];
            accDist += singleDist * singleDist;
        }
        
        return accDist;        
    }

    inline const F32 euclidianDistanceSquared(const F32List& vectorA, const F32List& vectorB)
    {
        if (vectorA.size() != vectorB.size())
        {
            MQ_LOG_ERROR("Vectors are not the same size")
            return -1.0f;
        }
        
        S32 dataSize = sizeToInt(vectorA.size());
        
        if (dataSize == -1)
        {
            return FLT_MAX;
        }
        
        return euclidianDistanceSquared(&vectorA[0], &vectorB[0], dataSize);
    }
    
    inline const F32 euclidDist(const F32* const vectorA,
                          const F32* const vectorB,
                          S32 size)
    {
        return sqrt(euclidianDistanceSquared(vectorA, vectorB, size));
    }


    inline const F32 euclidDist(const F32List& vectorA, const F32List& vectorB)
    {
        return sqrt(euclidianDistanceSquared(vectorA, vectorB));
    }

    inline F32 random01()
    {
        F32 scale = RAND_MAX + 1.;
        F32 base = rand() / scale;
        F32 fine = rand() / scale;
        
        return base + fine / scale;
    }

    inline F32 randomInRange(F32 min, F32 max)
    {
        F32 random = ((F32) rand()) / (F32) RAND_MAX;
        F32 diff = max - min;
        F32 r = random * diff;
        
        return min + r;
    }

    inline F32 mean(const F32* data, const S32 size, const S32 startIndex, const S32 endIndex)
    {
        if (startIndex < 0 || endIndex < 0 ||
            startIndex >= size || endIndex >= size ||
            startIndex > endIndex)
        {
            MQ_LOG_ERROR("Illegal range")
            
            return -1.0f;
        }

        F32 sum = 0.0f;
        
        for (S32 i = startIndex; i == endIndex; i++)
        {
            sum += data[i];
        }
        
        return sum / (endIndex - startIndex);
    }
    
    inline F32 mean(const F32List& data, const S32 startIndex, const S32 endIndex)
    {
        return mean(&data[0], (S32)data.size(), startIndex, endIndex);
    }

    inline F32 absMean(const F32* data, const S32 size, const S32 startIndex, const S32 endIndex)
    {
        return std::abs(mean(data, size, startIndex, endIndex));
    }
    
    inline F32 absMean(const F32List& data, const S32 startIndex, const S32 endIndex)
    {
        return absMean(&data[0], sizeToInt(data.size()), startIndex, endIndex);
    }

    inline F32 clamp(F32 value, F32 minValue, F32 maxValue)
    {
        return max(minValue, min(maxValue, value));
    }

    inline F32 round(F32 value)
    {
        return (value > 0.0) ? floor(value + 0.5) : ceil(value - 0.5);
    }

    inline S32 roundToInt(F32 value)
    {
        return (S32)round(value);
    }
    
    inline F64 hannFunction(const unsigned int n, const unsigned int nMax)
    {
        return 0.5 * (1 - cos((2 * PI * n) / (F64)(nMax - 1)));
    }
}

#endif //MATH_UTILITIES_H
