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

#ifndef TABLE_H
#define TABLE_H

typedef struct {
    mq_u32_t number;
    mq_f32_t start;
    mq_u32_t size;
    mq_u32_t gen_routine;
} mq_base_table_t;

typedef struct {
    mq_base_table_t base;
    const char *filename;
    mq_f32_t skip_time;
    mq_u32_t format;
    mq_u32_t channel;
} mq_sample_table_t;

typedef struct {
    mq_base_table_t base;
    mq_f32_t *values;
    mq_u32_t value_count;
} mq_immediate_table_t;

typedef struct {
    mq_f32_t value;
    mq_u32_t length;
} mq_segment_t;

typedef struct {
    mq_base_table_t base;
    mq_segment_t *segments;
    mq_u32_t segment_count;
} mq_segment_table_t;

#endif /* TABLE_H */
