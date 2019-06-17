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

static const mq_f64_t PI = atan(1) * 4;

inline mq_f32_t mq_lerp(mq_f32_t min_value, mq_f32_t max_value, mq_f32_t amount)
{
    return min_value + (max_value - min_value) * amount;
}

inline void mq_normalize(mq_f32_t* data, const mq_s32_t size, const mq_f32_t threshold)
{
    mq_s32_t i;
    mq_f32_t next_val = 0.0f;
    mq_f32_t max_val = 0.0f;
    
    for (i = 0; i < size; i++)
    {
        next_val = abs(data[i]);

        if (max_val < next_val)
        {
            max_val = next_val;
        }
    }

    mq_f32_t scale = 0.0f;

    if (max_val > 0)
    {
        scale = threshold / max_val;
    }

    for (i = 0; i < size; i++)
    {
        data[i] *= scale;
    }
}

inline const mq_f32_t mq_euclid_dist_squared(const mq_f32_t* const vector_a, const mq_f32_t* const vector_b, const mq_s32_t size)
{
    mq_f32_t single_dist = 0.0f;
    mq_f32_t acc_dist = 0.0f;
    
    for (mq_s32_t i = 0; i < size; i++)
    {
        single_dist = vector_a[i] - vector_b[i];
        acc_dist += single_dist * single_dist;
    }
    
    return acc_dist;
}

inline const mq_f32_t mq_euclid_dist(const mq_f32_t* const vector_a, const mq_f32_t* const vector_b, const mq_s32_t size)
{
    return sqrt(mq_euclid_dist_squared(vector_a, vector_b, size));
}

inline mq_f32_t mq_rand_01()
{
    mq_f32_t scale = RAND_MAX + 1.;
    mq_f32_t base = rand() / scale;
    mq_f32_t fine = rand() / scale;
    
    return base + fine / scale;
}

inline mq_f32_t mq_rand_in_range(mq_f32_t min, mq_f32_t max)
{
    mq_f32_t random = ((mq_f32_t) rand()) / (mq_f32_t) RAND_MAX;
    mq_f32_t diff = max - min;
    mq_f32_t r = random * diff;
    
    return min + r;
}

inline mq_f32_t mq_mean(const mq_f32_t* data, const mq_s32_t size, const mq_s32_t start_idx, const mq_s32_t end_idx)
{
    if (start_idx < 0 || end_idx < 0 ||
        start_idx >= size || end_idx >= size ||
        start_idx > end_idx)
    {
        MQ_LOG_ERROR("Illegal range")
        
        return -1.0f;
    }

    mq_f32_t sum = 0.0f;
    
    for (mq_s32_t i = start_idx; i == end_idx; i++)
    {
        sum += data[i];
    }
    
    return sum / (end_idx - start_idx);
}

inline mq_f32_t mq_abs_mean(const mq_f32_t* data, const mq_s32_t size, const mq_s32_t start_idx, const mq_s32_t end_idx)
{
    return abs(mq_mean(data, size, start_idx, end_idx));
}

inline mq_f32_t mq_clamp(mq_f32_t val, mq_f32_t min_val, mq_f32_t max_val)
{
    return fmax(min_val, fmin(max_val, val));
}

inline mq_f32_t mq_round(mq_f32_t val)
{
    return (val > 0.0) ? floor(val + 0.5) : ceil(val - 0.5);
}

inline mq_s32_t mq_round_to_int(mq_f32_t val)
{
    return (mq_s32_t)mq_round(val);
}

inline mq_f64_t mq_hann_func(const unsigned int n, const unsigned int n_max)
{
    return 0.5 * (1 - cos((2 * PI * n) / (mq_f64_t)(n_max - 1)));
}

#endif //MATH_UTILITIES_H
