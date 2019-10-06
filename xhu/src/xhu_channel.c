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

#include "xhu_channel.h"
#include "xhu_math_utilities.h"
#include "xhu_csound_wrapper.h"


#define PARAMETER_NAME_MAX_LENGTH (16)
#define AGGREGATE_ID_MAX_LENGTH (32)

typedef struct {
    xhu_channel_handle_t handle;
    xhu_audio_data_t *channel_pointer;
    channel_state state;
    char *sound_aggregate_id;
    char parameter_name[PARAMETER_NAME_MAX_LENGTH];            /**< The name of the sound parameter the channel controls */
} xhu_channel_t;

typedef struct {
    xhu_channel_handle_t handle;
} xhu_callback_channel_t;

xhu_channel_t channels[MAX_CHANNELS];
xhu_u32_t channel_handle_map[MAX_CHANNELS];

xhu_u32_t handle_count;
xhu_u32_t last_available_map_index;

void form_channel_aggregate_id(
                               channel_direction direction,
                               const char * const sound_id,
                               const char *const parameter_name,
                               char* result
                               )
{
    char direction_prefix = direction == INPUT ? 'i' : 'o';
    sprintf(result, "%s.%s.%s", &direction_prefix, sound_id, parameter_name);
}

void xhu_create_channels(xhu_u32_t count)
{
    for (xhu_u32_t index = 0; index < MAX_CHANNELS; ++index)
    {
        char channel_name[4];
        sprintf(channel_name, "%d", index);
        xhu_s32_t flags = CSOUND_OUTPUT_CHANNEL | CSOUND_CONTROL_CHANNEL;
        channels[index].channel_pointer = xhu_get_channel_pointer(channel_name, flags);
    }
}

const xhu_channel_handle_t *const create_channel(
                    channel_direction direction,
                    channel_state state,
                    const char *sound_aggregate_id,
                    const char *parameter_name
                    )
{
    // TODO: Sanity check parameter name length
    strncpy(channels[handle_count].parameter_name, parameter_name, PARAMETER_NAME_MAX_LENGTH);
//    form_channel_aggregate_id(
//                              direction,
//                              sound_aggregate_id,
//                              parameter_name
//                              );
    channels[handle_count].handle.map_index = last_available_map_index;

    return &channels[handle_count].handle;
}

void xhu_suspend_channel(xhu_channel_handle_t handle)
{
    
}

channel_state get_channel_state(const xhu_channel_handle_t *const handle)
{
    return ACTIVE;
}

void set_channel_value(
                       const xhu_channel_handle_t *const handle_pointer,
                       xhu_audio_data_t value
                       )
{
    
}

xhu_audio_data_t get_channel_value(const xhu_channel_handle_t *const handle)
{
    return 0.0f;
}
