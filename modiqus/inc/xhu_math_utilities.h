/*
 * Copyright (C) 2019 by Martin Dejean
 *
 * This file is part of Xhu.
 * Xhu is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Xhu is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Xhu.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#ifndef MATH_UTILITIES_H
#define MATH_UTILITIES_H

#include "xhu_debug.h"

#define EPSILON 0.001f;

xhu_u64_t hash(char *str)
{
    xhu_u64_t hash = 5381;
    xhu_u32_t c;
    
    while ((c = *str++))
    {
        hash = ((hash << 5) + hash) + c; /* hash * 33 + c */
    }
    
    return hash;
}

bool xhu_approximately_equals(const xhu_f32_t value_1, const xhu_f32_t value_2)
{
    const xhu_f32_t difference = fabs(value_1 - value_2);
    
    return difference < EPSILON;
}

xhu_f32_t xhu_lerp(xhu_f32_t min_value, xhu_f32_t max_value, xhu_f32_t amount)
{
    return min_value + (max_value - min_value) * amount;
}

void xhu_normalize(xhu_f32_t* data, const xhu_s32_t size, const xhu_f32_t threshold)
{
    xhu_s32_t i;
    xhu_f32_t next_val = 0.0f;
    xhu_f32_t max_val = 0.0f;
    
    for (i = 0; i < size; i++)
    {
        next_val = fabs(data[i]);

        if (max_val < next_val)
        {
            max_val = next_val;
        }
    }

    xhu_f32_t scale = 0.0f;

    if (max_val > 0)
    {
        scale = threshold / max_val;
    }

    for (i = 0; i < size; i++)
    {
        data[i] *= scale;
    }
}

const xhu_f32_t xhu_euclid_dist_squared(const xhu_f32_t* const vector_a, const xhu_f32_t* const vector_b, const xhu_s32_t size)
{
    xhu_f32_t single_dist = 0.0f;
    xhu_f32_t acc_dist = 0.0f;
    
    for (xhu_s32_t i = 0; i < size; i++)
    {
        single_dist = vector_a[i] - vector_b[i];
        acc_dist += single_dist * single_dist;
    }
    
    return acc_dist;
}

const xhu_f32_t xhu_euclid_dist(const xhu_f32_t* const vector_a, const xhu_f32_t* const vector_b, const xhu_s32_t size)
{
    return sqrt(xhu_euclid_dist_squared(vector_a, vector_b, size));
}

xhu_f32_t xhu_rand_01()
{
    xhu_f32_t scale = RAND_MAX + 1.;
    xhu_f32_t base = rand() / scale;
    xhu_f32_t fine = rand() / scale;
    
    return base + fine / scale;
}

xhu_f32_t xhu_rand_in_range(xhu_f32_t min, xhu_f32_t max)
{
    xhu_f32_t random = ((xhu_f32_t) rand()) / (xhu_f32_t) RAND_MAX;
    xhu_f32_t diff = max - min;
    xhu_f32_t r = random * diff;
    
    return min + r;
}

xhu_f32_t xhu_mean(const xhu_f32_t* data, const xhu_s32_t size, const xhu_s32_t start_idx, const xhu_s32_t end_idx)
{
    if (start_idx < 0 || end_idx < 0 ||
        start_idx >= size || end_idx >= size ||
        start_idx > end_idx)
    {
        XHU_LOG_ERROR("Illegal range")
        
        return -1.0f;
    }

    xhu_f32_t sum = 0.0f;
    
    for (xhu_s32_t i = start_idx; i == end_idx; i++)
    {
        sum += data[i];
    }
    
    return sum / (end_idx - start_idx);
}

xhu_f32_t xhu_abs_mean(const xhu_f32_t* data, const xhu_s32_t size, const xhu_s32_t start_idx, const xhu_s32_t end_idx)
{
    return fabs(xhu_mean(data, size, start_idx, end_idx));
}

xhu_f32_t xhu_clamp(xhu_f32_t val, xhu_f32_t min_val, xhu_f32_t max_val)
{
    return fmax(min_val, fmin(max_val, val));
}

xhu_f32_t xhu_round(xhu_f32_t val)
{
    return (val > 0.0) ? floor(val + 0.5) : ceil(val - 0.5);
}

xhu_s32_t xhu_round_to_int(xhu_f32_t val)
{
    return (xhu_s32_t)xhu_round(val);
}

xhu_f64_t xhu_hann_func(const unsigned int n, const unsigned int n_max)
{
    return 0.5 * (1 - cos((2 * M_PI * n) / (xhu_f64_t)(n_max - 1)));
}

#endif // MATH_UTILITIES_H
