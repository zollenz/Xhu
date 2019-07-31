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

#include "xhu.h"

void on_exit(void)
{
    puts ("Goodbye, cruel world....");
}

void callback(CSOUND *csound,
              const char *channelName,
              void *channelValuePtr,
              const void *channelType)
{
    const char *value = (const char *) channelValuePtr;
    
    printf("Sound ended : %s - %s\n", channelName, value);
}

int main(int argc, const char * argv[])
{
    atexit(on_exit);
    xhu_log_level = XHU_LOG_LEVEL_DEBUG;
    xhu_log_with_func_info = false;

    if (!xhu_start(false))
    {
        XHU_LOG_FATAL("Xhu engine failed initialization")
        xhu_stop();
        exit(EXIT_FAILURE);
    }
    else
    {
        XHU_LOG_INFO("Xhu engine initialized")
    }
    
    xhu_pause(2);
    
    xhu_sound_handle_t handle = xhu_create_sound();
    
//    xhu_create_channels(0);

    xhu_set_output_channel_callback(callback);
    xhu_set_control_channel_value(0.4f, "i.1.000000.pitch");
    xhu_send_message("i1 0 4");
    xhu_pause(2);
    
    xhu_s32_t flags = CSOUND_OUTPUT_CHANNEL | CSOUND_CONTROL_CHANNEL;
    xhu_audio_data_t *output_channel_pointer = xhu_get_channel_pointer("o.1.000000.pitch", flags);
    xhu_audio_data_t value = xhu_get_control_channel_value(output_channel_pointer);
    XHU_LOG_INFO("Received value %f from channel o.1.000000.pitch", value)
    xhu_set_control_channel_value(0.3f, "i.1.000000.pitch");
    
    xhu_pause(3);
    xhu_stop();
    
    return 0;
}

