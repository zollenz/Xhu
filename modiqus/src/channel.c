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

#include "csound_wrapper.h"
#include "channel.h"
#include "math_utilities.h"

#define PARAMETER_NAME_MAX_LENGTH (16)
#define AGGREGATE_ID_MAX_LENGTH (32)

typedef struct {
    mq_channel_handle_t handle;
    channel_direction direction;        /**< The type of the channel as known to Csound */
    channel_state state;
    char *sound_aggregate_id;
    char parameter_name[PARAMETER_NAME_MAX_LENGTH];            /**< The name of the sound parameter the channel controls */
    char aggregate_id[AGGREGATE_ID_MAX_LENGTH];              /**< The name of the channel as known to Csound */
} mq_channel_t;

mq_channel_t channels[MAX_CHANNELS];
mq_u32_t channel_handle_map[MAX_CHANNELS];

mq_u32_t handle_count;
mq_u32_t last_available_map_index;

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

const mq_channel_handle_t *const create_channel(
                    channel_direction direction,
                    channel_state state,
                    const char *sound_aggregate_id,
                    const char *parameter_name
                    )
{
    // TODO: Sanity check parameter name length
    channels[handle_count].direction = direction;
    strncpy(channels[handle_count].parameter_name, parameter_name, PARAMETER_NAME_MAX_LENGTH);
    form_channel_aggregate_id(
                              direction,
                              sound_aggregate_id,
                              parameter_name,
                              channels[handle_count].aggregate_id
                              );
    channels[handle_count].handle.map_index = last_available_map_index;
    channels[handle_count].handle.hash = hash(channels[handle_count].aggregate_id);

    return &channels[handle_count].handle;
}
void delete_channel(void)
{
}

void suspend_channel(void)
{
    
}

channel_state get_channel_state(const mq_channel_handle_t *const handle)
{
    return ACTIVE;
}

void set_channel_value(
                       const mq_channel_handle_t *const handle_pointer,
                       mq_audio_data_t value
                       )
{
    
}

mq_audio_data_t get_channel_value(const mq_channel_handle_t *const handle)
{
    return 0.0f;
}
