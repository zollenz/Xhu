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

#ifndef INSTRUMENT_H
#define INSTRUMENT_H

#define MQ_MAX_INSTANCE_NUMBER (999999)
#define MQ_MAX_INSTANCES_PER_INSTRUMENT (10)

typedef struct {
    const char* name;
    mq_u32_t instanceNumber;
    mq_u32_t instrument_number;
    bool active;
} mq_instrument_instance_t;

#endif // INSTRUMENT_H
