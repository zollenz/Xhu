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

#ifndef DEFS_H
#define DEFS_H

#include <stdbool.h>
#include "csound.h"

#ifdef __cplusplus
#define EXTERN_C extern "C"
#else
#define EXTERN_C
#endif

#define UNDEFINED_STRING "UNDEFINED"
#define TABLE_UNDEFINED (0)

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
typedef MYFLT mq_float_t;
typedef size_t mq_mem_size_t;

#endif // DEFS_H
