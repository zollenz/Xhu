//
//  sound.c
//  modiqus
//
//  Created by Martin Dejean on 03/07/2019.
//  Copyright © 2019 Modicus. All rights reserved.
//

#include <stdbool.h>
#include "sound.h"

typedef struct {
    mq_sound_handle_t handle;
    char name[MQ_MAX_NAME_SIZE];
    mq_u32_t id;
    mq_u32_t instance_id;
    char aggregate_id;
    mq_sound_playback_state state;
} mq_sound_t;

mq_sound_t sounds[MQ_MAX_SOUND_INSTANCES];

bool is_muted(mq_sound_handle_t handle)
{
    return sounds[handle].state == MUTED;
}
