//
//  sound.c
//  xhu
//
//  Created by Martin Dejean on 03/07/2019.
//  Copyright © 2019 Modicus. All rights reserved.
//

#include <stdbool.h>
#include "xhu_sound.h"
#include "xhu_channel.h"

typedef struct {
    xhu_sound_handle_t handle;
    char name[XHU_MAX_NAME_SIZE];
    xhu_u32_t id;
    xhu_u32_t instance_id;
    char aggregate_id;
    xhu_sound_state state;
    xhu_channel_handle_t *channel_handles;
} xhu_sound_t;

xhu_sound_t sounds[XHU_MAX_VOICES];
xhu_sound_handle_t sound_map[XHU_MAX_VOICES];
xhu_sound_handle_t stopped_sound_stack[XHU_MAX_VOICES];

xhu_u32_t last_stopped;

xhu_sound_handle_t xhu_create_sound(void)
{
    xhu_sound_handle_t handle = stopped_sound_stack[last_stopped];
    // TODO: Sanity check handle
    stopped_sound_stack[last_stopped] = INT_MAX;
    --last_stopped;
    return handle;
}

xhu_sound_state xhu_get_sound_state(xhu_sound_handle_t handle)
{
    return sounds[handle].state;
}

bool xhu_is_sound_valid(xhu_sound_handle_t handle)
{
    return handle != INT_MAX;
}


