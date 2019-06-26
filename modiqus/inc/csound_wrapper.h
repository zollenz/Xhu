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

#ifndef CSOUND_WRAPPER_H
#define CSOUND_WRAPPER_H

#include "table.h"

EXTERN_C void mq_set_log_level(mq_s32_t level);
EXTERN_C bool mq_start(bool bundle);
EXTERN_C void mq_stop(void);
EXTERN_C void mq_get_chn_ctrl_output(MYFLT* value, const char *name);
EXTERN_C void mq_set_chn_ctrl_value(MYFLT value, const char *name);
EXTERN_C void mq_send_message(const char* message);
EXTERN_C void mq_send_score_event(const char type, MYFLT* parameters, mq_s32_t numParameters);
EXTERN_C void mq_create_sample_table(mq_sample_table_t* const table);
EXTERN_C void mq_create_immediate_table(mq_immediate_table_t* const table);
EXTERN_C void mq_create_segment_table(mq_segment_table_t* const table);
EXTERN_C const mq_s32_t mq_get_table_data(const mq_s32_t tableNumber, mq_float_t* const data);
EXTERN_C void mq_set_table_data(const mq_s32_t table, const mq_float_t *const data, mq_u32_t data_count);
EXTERN_C const mq_f32_t mq_get_table_val(const mq_s32_t table, const mq_s32_t index);
EXTERN_C bool mq_table_exists(const mq_s32_t tableNumber);
EXTERN_C void mq_delete_table(const mq_s32_t tableNumber);
EXTERN_C const mq_s32_t mq_get_sample_rate(void);
EXTERN_C const mq_s32_t mq_get_control_rate(void);
EXTERN_C const mq_s32_t mq_get_control_size(void);
EXTERN_C const mq_f32_t mq_get_control_period(void);
EXTERN_C bool mq_set_global_env(const char *name, const char *value);
EXTERN_C void mq_set_opcode_path(const char *path);
EXTERN_C void mq_set_csd_path(const char *path);
EXTERN_C void mq_set_audio_path(const char *path);

#endif // CSOUND_WRAPPER_H
