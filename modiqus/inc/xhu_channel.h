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

#ifndef XHU_CHANNEL_H
#define XHU_CHANNEL_H

#include <stdbool.h>
#include "xhu_defs.h"

#define MAX_CHANNELS (100)

typedef struct {
    xhu_u32_t map_index;
    xhu_u64_t hash;
} xhu_channel_handle_t;

typedef enum
{
    INPUT,
    OUTPUT
} channel_direction;

typedef enum
{
    ACTIVE,
    SUSPENDED
} channel_state;

EXTERN_C xhu_audio_data_t *xhu_get_channel_pointer(const char *name, xhu_s32_t flags);
EXTERN_C xhu_audio_data_t xhu_get_control_channel_value(xhu_audio_data_t *channel);
EXTERN_C void xhu_set_control_channel_value(xhu_audio_data_t value, const char *name);
EXTERN_C void xhu_create_channels(xhu_u32_t count);
EXTERN_C const xhu_channel_handle_t *const create_channel(
                                                         channel_direction direction,
                                                         channel_state state,
                                                         const char *sound_aggregate_id,
                                                         const char *parameter_name
                                                         );
EXTERN_C void xhu_suspend_channel(xhu_channel_handle_t handle);
EXTERN_C channel_state get_channel_state(const xhu_channel_handle_t *const handle);
EXTERN_C void set_channel_value(const xhu_channel_handle_t *const handle_pointer, xhu_audio_data_t value);
EXTERN_C xhu_audio_data_t get_channel_value(const xhu_channel_handle_t *const handle);

#endif // XHU_CHANNEL_H
