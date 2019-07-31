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

#ifndef DEFS_H
#define DEFS_H

#include <stdint.h>
#include "csound.h"

#ifdef __cplusplus
#define EXTERN_C extern "C"
#else
#define EXTERN_C
#endif

#define XHU_MAX_VOICES (100)

#define UNDEFINED_STRING "UNDEFINED"
#define TABLE_UNDEFINED (0)

#ifdef __LP64__
typedef unsigned long xhu_array_size_t;
#else
typedef unsigned int xhu_array_size_t;
#endif
typedef uint8_t xhu_u8_t;
typedef int8_t xhu_s8_t;
typedef uint16_t xhu_u16_t;
typedef int16_t xhu_s16_t;
typedef uint32_t xhu_u32_t;
typedef int32_t xhu_s32_t;
typedef uint64_t xhu_u64_t;
typedef int64_t xhu_s64_t;
typedef float xhu_f32_t;
typedef double xhu_f64_t;
typedef MYFLT xhu_audio_data_t;
typedef size_t xhu_mem_size_t;

#endif // DEFS_H
