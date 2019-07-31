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

#ifndef TABLE_H
#define TABLE_H

#include "xhu_defs.h"

typedef struct {
    xhu_u32_t number;
    xhu_f32_t start;
    xhu_u32_t size;
    xhu_u32_t gen_routine;
} xhu_base_table_t;

typedef struct {
    xhu_base_table_t base;
    const char *filename;
    xhu_f32_t skip_time;
    xhu_u32_t format;
    xhu_u32_t channel;
} xhu_sample_table_t;

typedef struct {
    xhu_base_table_t base;
    xhu_f32_t *values;
    xhu_u32_t value_count;
} xhu_immediate_table_t;

typedef struct {
    xhu_f32_t value;
    xhu_u32_t length;
} xhu_segment_t;

typedef struct {
    xhu_base_table_t base;
    xhu_segment_t *segments;
    xhu_u32_t segment_count;
} xhu_segment_table_t;

EXTERN_C void xhu_create_sample_table(xhu_sample_table_t* const table);
EXTERN_C void xhu_create_immediate_table(xhu_immediate_table_t* const table);
EXTERN_C void xhu_create_segment_table(xhu_segment_table_t* const table);

#endif /* TABLE_H */
