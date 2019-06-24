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

#include "csound.h"
#include "table.h"

void mq_set_log_level(mq_s32_t level);
bool mq_start(bool bundle);
void mq_stop();
void mq_get_chn_ctrl_output(MYFLT& value, const char *name);
void mq_set_chn_ctrl_value(MYFLT value, const char *name);
void mq_send_message(const char* message);
void mq_send_score_event(const char type, MYFLT* parameters, mq_s32_t numParameters);
void mq_create_sample_table(mq_sample_table_t* const table);
void mq_create_immediate_table(mq_immediate_table_t* const table);
void mq_create_segment_table(mq_segment_table_t* const table);
const mq_s32_t mq_get_table_data(const mq_s32_t tableNumber, mq_float_t* const data);
void mq_set_table_data(const mq_s32_t table, const mq_float_t *const data, mq_u32_t data_count);
const mq_f32_t mq_get_table_val(const mq_s32_t table, const mq_s32_t index);
bool mq_table_exists(const mq_s32_t tableNumber);
void mq_delete_table(const mq_s32_t tableNumber);
const mq_s32_t mq_get_sample_rate();
const mq_s32_t mq_get_control_rate();
const mq_s32_t mq_get_control_size();
const mq_f32_t mq_get_control_period();
bool mq_set_global_env(const char *name, const char *value);
void mq_set_opcode_path(const char *path);
void mq_set_csd_path(const char *path);
void mq_set_audio_path(const char *path);
#endif // CSOUND_WRAPPER_H
