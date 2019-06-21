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

#ifndef TYPES_H
#define TYPES_H

#include <climits>
#include <stdint.h>
#include <stddef.h>
#include <string>
#include <vector>

#ifdef __LP64__
typedef unsigned long mq_array_size_t;
#else
typedef unsigned int mq_array_size_t;
#endif
typedef uint8_t mq_u8_t;
typedef int8_t mq_s8_t;
typedef uint16_t mq_u16_t;
typedef int16_t mq_s16_t;
typedef uint32_t mq_u32_t;
typedef int32_t mq_s32_t;
typedef uint64_t mq_u64_t;
typedef int64_t mq_s64_t;
typedef float mq_f32_t;
typedef double mq_f64_t;
typedef size_t mq_mem_size_t;
typedef std::string mq_str_t;
typedef std::vector<mq_s32_t> mq_s32_list_t;
typedef std::vector<mq_f32_t> mq_f32_list_t;

static const mq_s32_t SRATE = 44100;
static const mq_s32_t KRATE = 4410;
static const mq_s32_t KSMPS = SRATE / KRATE;
static const mq_s32_t BUFFER_SIZE = 512;
static const mq_s32_t INDEX_INVALID = -1;
static const mq_s32_t TABLE_UNDEFINED = 0;
static const char *UNDEFINED_STR = "UNDEFINED";
static const mq_s32_t UNDEFINED_INT = -1;
static const mq_s32_t HAS_DATA = 1;
static const mq_f32_t EPSILON = 0.001f;

typedef struct {
    mq_f32_t value;
    mq_u32_t length;
} mq_segment_t;

typedef std::vector<mq_segment_t> mq_segment_list_t;

#endif //TYPES_H
