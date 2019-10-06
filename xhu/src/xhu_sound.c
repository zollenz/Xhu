//
//  sound.c
//  xhu
//
//  Created by Martin Dejean on 03/07/2019.
//  Copyright © 2019 Modicus. All rights reserved.
//

#include <stdbool.h>
#include "xhu_debug.h"
#include "xhu_sound.h"
#include "xhu_channel.h"

#define INVALID_SOUND_HANDLE (INT_MAX)

typedef struct {
    xhu_sound_handle_t handle;
    xhu_u32_t id;
    xhu_u32_t instance_id;
    char aggregate_id[10];
    char name[XHU_MAX_NAME_SIZE];
    xhu_sound_state state;
    xhu_channel_handle_t *channel_handles; // TODO: Fixed array length?
} xhu_sound_t;

xhu_sound_t sounds[XHU_MAX_VOICES];
xhu_sound_handle_t handles[XHU_MAX_VOICES] = {};
xhu_sound_handle_t stopped_handles[XHU_MAX_VOICES] = {};
xhu_u32_t last_instance_ids[XHU_MAX_SOUND_ID] = {};
xhu_u32_t last_stopped;

xhu_u32_t xhu_get_next_instance_id(const xhu_u32_t sound_id)
{
    if (sound_id > XHU_MAX_SOUND_ID)
    {
        XHU_LOG_ERROR("Sound ID %s is higher than the max allowed.")
    }
    
    if (last_instance_ids[sound_id] == 0)
    {
        return 0;
    }

    last_instance_ids[sound_id] = ++last_instance_ids[sound_id] % XHU_MAX_SOUND_ID;
    
    return last_instance_ids[sound_id];
}

void xhu_initialize_sound_management()
{
        
}

xhu_sound_handle_t xhu_initialize_sound(const xhu_u32_t sound_id, const char *const name)
{
    if (sound_id > XHU_MAX_SOUND_ID)
    {
        XHU_LOG_ERROR("Sound ID %s is higher than the max allowed.")
        return INVALID_SOUND_HANDLE;
    }

    // TODO: Truncate and warn instead?
    if (strlen(name) > XHU_MAX_NAME_SIZE)
    {
        XHU_LOG_ERROR("Sound name %s is longer than the max allowed.")
        return INVALID_SOUND_HANDLE;
    }
    
    xhu_sound_handle_t handle = stopped_handles[last_stopped];
    
    if (handle > XHU_MAX_VOICES)
    {
        XHU_LOG_ERROR("Sound handle %d is higher than the max allowed.")
        return INVALID_SOUND_HANDLE;
    }
    
    if (sounds[handle].state != STOPPED)
    {
        XHU_LOG_ERROR("Sound handle %d is not available for use.")
        return INVALID_SOUND_HANDLE;
    }
    
    stopped_handles[last_stopped] = 0;
    --last_stopped;
    
    sounds[handle].handle = handle;
    sounds[handle].id = sound_id;
    
    xhu_u32_t instance_id = xhu_get_next_instance_id(sound_id);
    
    sounds[handle].instance_id = instance_id;
    sprintf(sounds[handle].aggregate_id, "%d.%d", sound_id, instance_id);
    
    XHU_LOG_DEBUG("Initialized sound %s", sounds[handle].aggregate_id)
    
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

void xhu_play_sound(xhu_sound_handle_t handle)
{
    
}

void xhu_stop_sound(xhu_sound_handle_t handle)
{
    // Stop the playing sound
    // TODO...
    // Update the stack of available sounds
    ++last_stopped;
    stopped_handles[last_stopped] = handle;
}


