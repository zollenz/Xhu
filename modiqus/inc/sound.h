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

#ifndef SOUND_H
#define SOUND_H

#include "defs.h"

#define MQ_MAX_NAME_SIZE (30)
#define MQ_MAX_SOUND_INSTANCE_ID (999999)

typedef enum { STOPPED, PLAYING, PAUSED, MUTED } mq_sound_playback_state;
typedef mq_u32_t mq_sound_handle_t;

EXTERN_C bool get_state(mq_sound_handle_t handle);


#endif // SOUND_H
