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

#ifndef XHU_CSOUND_WRAPPER_H
#define XHU_CSOUND_WRAPPER_H

#include <stdbool.h>
#include "xhu_table.h"

EXTERN_C void xhu_set_log_level(xhu_s32_t level);
EXTERN_C bool xhu_start(bool bundle);
EXTERN_C void xhu_stop(void);
EXTERN_C xhu_audio_data_t *xhu_get_channel_pointer(const char *name, xhu_s32_t flags);
EXTERN_C xhu_audio_data_t xhu_get_control_channel_value(xhu_audio_data_t *channel);
EXTERN_C void xhu_set_control_channel_value(xhu_audio_data_t value, const char *name);
EXTERN_C void xhu_send_message(const char* message);
EXTERN_C void xhu_send_score_event(const char type, xhu_audio_data_t* parameters, xhu_s32_t numParameters);
EXTERN_C const xhu_s32_t xhu_get_table_data(const xhu_s32_t tableNumber, xhu_audio_data_t* const data);
EXTERN_C void xhu_set_table_data(const xhu_s32_t table, const xhu_audio_data_t *const data, xhu_u32_t data_count);
EXTERN_C const xhu_f32_t xhu_get_table_val(const xhu_s32_t table, const xhu_s32_t index);
EXTERN_C bool xhu_table_exists(const xhu_s32_t tableNumber);
EXTERN_C void xhu_delete_table(const xhu_s32_t tableNumber);
EXTERN_C const xhu_s32_t xhu_get_sample_rate(void);
EXTERN_C const xhu_s32_t xhu_get_control_rate(void);
EXTERN_C const xhu_s32_t xhu_get_control_size(void);
EXTERN_C const xhu_f32_t xhu_get_control_period(void);
EXTERN_C bool xhu_set_global_env(const char *name, const char *value);
EXTERN_C void xhu_set_opcode_path(const char *path);
EXTERN_C void xhu_set_csd_path(const char *path);
EXTERN_C void xhu_set_audio_path(const char *path);
EXTERN_C void xhu_set_output_channel_callback(channelCallback_t callback);

#endif // XHU_CSOUND_WRAPPER_H
