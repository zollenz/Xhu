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

#include "modiqus.h"

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
    mq_log_level = MQ_LOG_LEVEL_DEBUG;
    mq_log_with_func_info = false;

    if (!mq_start(false))
    {
        MQ_LOG_FATAL("Modiqus engine failed initialization")
        mq_stop();
        exit(EXIT_FAILURE);
    }
    else
    {
        MQ_LOG_INFO("Modiqus engine initialized")
    }
    
    mq_pause(2);

    
//    mq_create_channels(0);

    mq_set_output_channel_callback(callback);
    mq_set_control_channel_value(0.4f, "i.1.000000.pitch");
    mq_send_message("i1 0 4");
    mq_pause(2);
    
    mq_s32_t flags = CSOUND_OUTPUT_CHANNEL | CSOUND_CONTROL_CHANNEL;
    mq_audio_data_t *output_channel_pointer = mq_get_channel_pointer("o.1.000000.pitch", flags);
    mq_audio_data_t value = mq_get_control_channel_value(output_channel_pointer);
    char log_message[100];
    sprintf(log_message, "Received value %f from channel o.1.000000.pitch", value);
    MQ_LOG_INFO(log_message)
    mq_set_control_channel_value(0.3f, "i.1.000000.pitch");
    
    mq_pause(3);
    mq_stop();
    
    return 0;
}

