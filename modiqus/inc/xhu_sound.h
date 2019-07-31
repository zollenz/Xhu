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

#ifndef SOUND_H
#define SOUND_H

#include "xhu_defs.h"

#define XHU_MAX_NAME_SIZE (30)
#define XHU_MAX_SOUND_INSTANCE_ID (999999)

typedef enum { STOPPED, PLAYING, PAUSED, MUTED } xhu_sound_state;
typedef xhu_u32_t xhu_sound_handle_t;

EXTERN_C xhu_sound_state xhu_get_sound_state(xhu_sound_handle_t handle);
EXTERN_C xhu_sound_handle_t xhu_create_sound(void);
EXTERN_C bool xhu_is_sound_valid(xhu_sound_handle_t handle);




#endif // SOUND_H
