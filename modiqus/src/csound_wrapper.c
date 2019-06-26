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


#include <time.h>
#include "debug.h"
#include "csound_wrapper.h"
#include "system_utilities.h"
#include "math_utilities.h"

//#define MACOS_BUNDLE

typedef struct {
    CSOUND* csound;
    mq_s32_t compile_result;
    bool run_performance_thread;
    bool pause_csound_thread;
    bool csound_thread_paused;
} mq_csound_state_t;

mq_csound_state_t _mq_csound_state;
volatile bool _mq_perf_thread_running;

mq_s32_t mq_log_level = MQ_LOG_LEVEL_DEBUG;
bool mq_log_with_func_info = false;

static const char *mq_executable_path;
static char mq_opcode_path[PATH_MAX];
static char mq_csd_path[PATH_MAX];
static char mq_audio_path[PATH_MAX];

static void mq_msg_callback(CSOUND *csound, mq_s32_t attr, const char *format, va_list args)
{
    mq_log_csound(format, args);
    return;
}

static void mq_set_executable_path()
{
#ifdef __APPLE__
    char full_path[PATH_MAX];
    uint32_t size = sizeof(full_path);
    char log_message[100];
    
    if (_NSGetExecutablePath(full_path, &size) != 0) {
        full_path[0] = '\0';
        sprintf(log_message, "Buffer too small; need size %u", size);
        MQ_LOG_ERROR(log_message)
    }
    
    mq_executable_path = mq_get_folder_path(full_path);
    sprintf(log_message, "Executable path is %s", mq_executable_path);
    MQ_LOG_DEBUG(log_message)
#else
    //    char temp[PATH_MAX];
    //    return (getcwd(temp, PATH_MAX) ? mq_str(temp) : mq_str(""));
    mq_executable_path = "";
#endif
}


static void mq_print_csound_return_code(const char *function_name, mq_s32_t return_code)
{
#ifdef DEBUG
    const char *return_code_str;
    
    if (return_code == CSOUND_EXITJMP_SUCCESS) {
        /* terminated, but not error (e.g. --help, -U, or exitnow) */
        return_code_str = "early return";
        return_code = 0;
    } else if (return_code > 0 && strncmp(function_name, "csoundPerform", 13) == 0) {
        /* for csoundPerformKsmps and csoundPerformBuffer only */
        return_code_str = "end of score or MIDI file";
        return_code = 0;
    } else {
        switch (return_code) {
            case CSOUND_SUCCESS:
                return_code_str = "success";
                break;
            case CSOUND_ERROR:
                return_code_str = "error";
                break;
            case CSOUND_INITIALIZATION:
                return_code_str = "initialization error";
                break;
            case CSOUND_PERFORMANCE:
                return_code_str = "performance error";
                break;
            case CSOUND_MEMORY:
                return_code_str = "memory allocation error";
                break;
            case CSOUND_SIGNAL:
                return_code_str = "caught SIGINT or SIGTERM";
                break;
            default:
                return_code_str = "error";
                break;
        }
    }
    
    char log_message[100];
    sprintf(log_message, "Csound function '%s' returned %d (%s)", function_name, return_code, return_code_str);
    MQ_LOG_DEBUG(log_message);
#endif
}

uintptr_t csound_thread(void* data)
{
    mq_csound_state_t* state = (mq_csound_state_t*)data;
    
    if (state->compile_result == CSOUND_SUCCESS) {
        _mq_perf_thread_running = true;
        MQ_LOG_DEBUG("Csound performance thread created")
        
        while (true) {
            // Yield for non-reentrant API functions
            if (state->pause_csound_thread) {
                state->csound_thread_paused = true;
                MQ_LOG_DEBUG("CSOUND THREAD YIELD")
            }
            
            while (state->pause_csound_thread);
            
            state->csound_thread_paused = false;
            
            if (csoundPerformKsmps(state->csound) != 0 || !state->run_performance_thread) {
                break;
            }
        }
        
        _mq_perf_thread_running = false;
        MQ_LOG_DEBUG("Csound performance loop stopped")
        csoundDestroy(state->csound);
        MQ_LOG_DEBUG("Csound instance destroyed")
    }
    
    return 1;
}

void mq_set_opcode_path(const char *path)
{
#ifdef MACOS_BUNDLE
    sprintf(mq_opcode_path, "%s%s", mq_executable_path, "/../Frameworks");
#else
    sprintf(mq_opcode_path, "%s%s", mq_executable_path, "/lib");
#endif
    mq_set_global_env("OPCODE6DIR64", mq_opcode_path);
}

void mq_set_audio_path(const char *path)
{
    const char *suffix = "/Resources/audio";
#ifdef MACOS_BUNDLE
    sprintf(mq_audio_path, "%s%s%s", mq_executable_path, "/..", suffix);
#else
    sprintf(mq_audio_path, "%s%s", mq_executable_path, suffix);
#endif
    mq_set_global_env("SSDIR", mq_audio_path);
}

void mq_set_csd_path(const char *path)
{
    const char *suffix = "/Resources/csound/modiqus.csd";
#ifdef MACOS_BUNDLE
    sprintf(mq_csd_path, "%s%s%s", mq_executable_path, "/../", suffix);
#else
    sprintf(mq_csd_path, "%s%s", mq_executable_path, suffix);
#endif
}

bool mq_start(bool bundle)
{
    srand((unsigned)time(NULL));
    
    // Set Csound environment variables
    mq_set_executable_path();
    mq_set_opcode_path(NULL);
    mq_set_csd_path(NULL);
    mq_set_audio_path(NULL);
    
    // Create Csound instance
    _mq_csound_state.csound = csoundCreate(NULL);
    
    if (_mq_csound_state.csound == NULL) {
        MQ_LOG_FATAL( "Csound instance creation failed")
        return false;
    }
    
    _mq_perf_thread_running = false;
    
    _mq_csound_state.compile_result = CSOUND_ERROR;
    _mq_csound_state.run_performance_thread = false;
    
    csoundSetMessageCallback(_mq_csound_state.csound, mq_msg_callback);
    
    // Compile orchestra file
    mq_s32_t cSoundArgsCount = 2;
    char* cSoundArgs[cSoundArgsCount];
    cSoundArgs[0] = "csound";
    char temp[strlen(mq_csd_path) + 1];
    strcpy(temp, mq_csd_path);
    cSoundArgs[1] = temp;
    _mq_csound_state.compile_result = csoundCompile(_mq_csound_state.csound, cSoundArgsCount, cSoundArgs);
    mq_print_csound_return_code("cSoundCompile", _mq_csound_state.compile_result);
    
    if (_mq_csound_state.compile_result != CSOUND_SUCCESS) {
        MQ_LOG_FATAL( "Csound .csd compilation failed")
        
        return false;
    }
    
    // Start performance thread
    _mq_csound_state.run_performance_thread = true;
    _mq_csound_state.pause_csound_thread = false;
    _mq_csound_state.csound_thread_paused = false;
    
    if (csoundCreateThread(csound_thread, (void *)&_mq_csound_state) == NULL) {
        MQ_LOG_FATAL( "Csound performance thread creation failed")
        
        return false;
    }
    
    // Wait for performance thread
    while (true) {
        if (_mq_perf_thread_running) {
            break;
        }
        mq_pause(1);
    }
    
    // Everything is OK...
    MQ_LOG_DEBUG("Csound wrapper initialized and performance thread running")
    
    return true;
}

void mq_stop(void)
{
    _mq_csound_state.run_performance_thread = false;
}

void mq_set_log_level(mq_s32_t level)
{
    mq_log_level = level;
}

void mq_get_chn_ctrl_output(MYFLT *value, const char *name)
{
    MYFLT *chnPtr = NULL;
    mq_s32_t chnType = CSOUND_OUTPUT_CHANNEL | CSOUND_CONTROL_CHANNEL;
    mq_s32_t result = csoundGetChannelPtr(_mq_csound_state.csound, &chnPtr, name, chnType);
    
    if (result == CSOUND_SUCCESS) {
        *value = *chnPtr;
        char log_message[100];
        sprintf(log_message, "Value %f received from channel %s", *value, name);
        MQ_LOG_DEBUG(log_message)
    } else {
        mq_print_csound_return_code("csoundGetChannelPtr", result);
    }
}

void mq_set_chn_ctrl_value(MYFLT value, const char *name)
{
    MYFLT *chnPtr = NULL;
    mq_s32_t chnType = CSOUND_INPUT_CHANNEL | CSOUND_CONTROL_CHANNEL;
    mq_s32_t result = csoundGetChannelPtr(_mq_csound_state.csound, &chnPtr, name, chnType);
    
    if (result == CSOUND_SUCCESS)
    {
        *chnPtr = (MYFLT)value;
        char log_message[100];
        sprintf(log_message, "Value %f sent to channel %s", value, name);
        MQ_LOG_DEBUG(log_message)
    }
    else
    {
        mq_print_csound_return_code("csoundGetChannelPtr", result);
    }
}

void mq_send_message(const char* message)
{
    char log_message[100];
    sprintf(log_message, "Sending message to Csound:\n%s", message);
    MQ_LOG_DEBUG(log_message);
    csoundInputMessage(_mq_csound_state.csound, message);
}

void mq_send_score_event(const char type, MYFLT* parameters, mq_s32_t numParameters)
{
    mq_s32_t result = csoundScoreEvent(_mq_csound_state.csound, type, parameters, numParameters);
    
    if(result == CSOUND_SUCCESS)
    {
        MQ_LOG_DEBUG("Sent score event");
    }
    else
    {
        mq_print_csound_return_code("csoundScoreEvent", result);
    }
}

const mq_s32_t mq_get_sample_rate(void)
{
    return mq_round_to_int(csoundGetSr(_mq_csound_state.csound));
}

const mq_s32_t mq_get_control_rate(void)
{
    return csoundGetKr(_mq_csound_state.csound);
}

const mq_s32_t mq_get_control_size(void)
{
    return csoundGetKsmps(_mq_csound_state.csound);
}

const mq_f32_t mq_get_control_period(void)
{
    return 1.0f / mq_get_sample_rate() * mq_get_control_size(); // ksmps duration
}

void mq_create_sample_table(mq_sample_table_t* const table)
{
    char message[128];
    sprintf(
            message,
            "f %u 0 0 %u \"%s\" %f %u %u",
            table->base.number,
            table->base.gen_routine,
            table->filename,
            table->skip_time,
            table->format,
            table->channel
            );
    mq_send_message(message);
    
    while (!mq_table_exists(table->base.number)); // TODO: replace with safer solution
}

void mq_create_immediate_table(mq_immediate_table_t* const table)
{
    if (table->value_count > table->base.size)
    {
        MQ_LOG_ERROR("Value count can not exceed table size for immediate table.")
        return;
    }
    
    mq_u32_t values_string_size = 2 * table->value_count + 1;
    char values_string[values_string_size];
    
    for (mq_u32_t i = 0; i < table->value_count; i += 2)
    {
        values_string[i] = ' ';
        values_string[i + 1] = table->values[i];
    }
    
    values_string[values_string_size] = '\0';
    
    char message[values_string_size + 64];
    
    sprintf(
            message,
            "f %u 0 0 %u %s",
            table->base.number,
            table->base.gen_routine,
            values_string
            );
    
    mq_send_message(message);
    
    while (!mq_table_exists(table->base.number)); // TODO: replace with safer solution
}

void mq_create_segment_table(mq_segment_table_t* const table)
{
    //    mq_u32_t segments_string_size = 4 * table->segment_count + 1;
    //    char segments_string[segments_string_size];
    //    mq_f32_t total_length = 0;
    //
    //    for (mq_u32_t i = 0; i < table->segment_count; ++i)
    //    {
    //        segments_string[i] = " ";
    //        char value_string[1079];
    //        segments_string[i] = "";
    //        segments_string[i] = " ";
    //        segments_string[i] = "";
    //        total_length += table->segments[i].length;
    //    }
    //
    //    if (total_length < table->base.size)
    //    {
    //        MQ_LOG_WARN("Segment length sum is less than table size. Padding table end with zeros.");
    //    }
    //
    //    if (total_length > table->base.size)
    //    {
    //        MQ_LOG_WARN("Segment length sum is bigger than table size. Excess segments will not be included.");
    //    }
    //
    //    for (mq_u32_t i = 0; i < numSegments; i++)
    //    {
    //        message += " " + mq_to_string<mq_f32_t>(table->segments.at(i).value);
    //        message += " " + mq_to_string<mq_f32_t>(table->segments.at(i).length);
    //    }
    //
    //
    ////    mq_str_t message = "f " + mq_to_string<mq_s32_t>(table->number);
    ////    message += " 0 " + mq_to_string<mq_s32_t>(table->size) + " -7";
    ////
    ////    for (mq_s32_t i = 0; i < numSegments; i++)
    ////    {
    ////        message += " " + mq_to_string<mq_f32_t>(table->segments.at(i).value);
    ////        message += " " + mq_to_string<mq_f32_t>(table->segments.at(i).length);
    ////    }
    //
    //    mq_send_message(message);
    //
    //    while (!mq_table_exists(table->number));
}

const mq_s32_t mq_get_table_data(const mq_s32_t table_id, mq_float_t *data)
{
    mq_s32_t length = -1;
    
    if (table_id == 0)
    {
        MQ_LOG_ERROR("Table ID is undefined.")
        
        return length;
    }
    
    if (data == NULL)
    {
        MQ_LOG_ERROR("Data pointer is NULL.")
        
        return length;
    }
    
    _mq_csound_state.pause_csound_thread = true;
    
    while (!_mq_csound_state.csound_thread_paused);
    
    length = csoundGetTable(_mq_csound_state.csound, &data, table_id);
    
    if (length >= 0)
    {
        char log_message[100];
        sprintf(log_message, "Got data for table %d", table_id);
        MQ_LOG_DEBUG(log_message);
    }
    else
    {
        char log_message[100];
        sprintf(log_message, "Could not get data for table %d. Table does not exist.", table_id);
        MQ_LOG_ERROR(log_message)
    }
    
    _mq_csound_state.pause_csound_thread = false;
    
    return length;
}

void mq_set_table_data(const mq_s32_t table, const mq_float_t *const data, mq_u32_t data_count)
{
    if (table == TABLE_UNDEFINED)
    {
        MQ_LOG_DEBUG("Table is undefined.");
        return;
    }
    
    _mq_csound_state.pause_csound_thread = true;
    
    while (!_mq_csound_state.csound_thread_paused);
    
    for (mq_u32_t i = 0; i < data_count; ++i)
    {
        csoundTableSet(_mq_csound_state.csound, table, i, data[i]);
    }
    
    _mq_csound_state.pause_csound_thread = false;
}

const mq_f32_t mq_get_table_val(const mq_s32_t tableNumber, const mq_s32_t index)
{
    mq_f32_t value = NAN;
    
    if (tableNumber <= 0)
    {
        MQ_LOG_ERROR("Could not retrieve data. Invalid table number.")
        
        return value;
    }
    
    if (index < 0)
    {
        MQ_LOG_ERROR("Could not retrieve data. Index is negative.")
        
        return value;
    }
    
    _mq_csound_state.pause_csound_thread = true;
    
    while (!_mq_csound_state.csound_thread_paused);
    
    value = csoundTableGet(_mq_csound_state.csound, tableNumber, index);
    _mq_csound_state.pause_csound_thread = false;
    
    return value;
}

bool mq_table_exists(mq_s32_t tableNumber)
{
    bool exists = false;
    
    if (tableNumber == 0)
    {
        MQ_LOG_ERROR("Could not retrieve data. Table is undefined.")
        
        return exists;
    }
    
    mq_s32_t length = -1;
    MYFLT *tablePtr = NULL;
    _mq_csound_state.pause_csound_thread = true;
    
    while (!_mq_csound_state.csound_thread_paused);
    
    length = csoundGetTable(_mq_csound_state.csound, &tablePtr, tableNumber);
    _mq_csound_state.pause_csound_thread = false;
    exists = length > 0 && tablePtr != NULL;
    
    char message[100];
    
    if (exists)
    {
        sprintf(message, "Table %d exists.", tableNumber);
        MQ_LOG_DEBUG(message)
    }
    else
    {
        sprintf(message, "Table %d does not exist.", tableNumber);
        MQ_LOG_DEBUG(message)
    }
    
    return exists;
}

void mq_delete_table(const mq_s32_t table_id)
{
    if (table_id == TABLE_UNDEFINED)
    {
        MQ_LOG_DEBUG("Table is undefined.")
        
        return;
    }
    
    if (mq_table_exists(table_id)) {
        char message[50];
        sprintf(message, "f -%d 0", table_id);
        mq_send_message(message);
        
        char log_message[100];
        sprintf(log_message, "Deleted table %d", table_id);
        MQ_LOG_DEBUG(log_message)
    }
    else
    {
        char log_message[100];
        sprintf(log_message, "Table %d does not exist.", table_id);
        MQ_LOG_DEBUG(log_message)
    }
}

bool mq_set_global_env(const char *name, const char *value)
{
    mq_s32_t status = csoundSetGlobalEnv(name, value);
#ifdef DEBUG
    mq_print_csound_return_code("csoundSetGlobalEnv", status);
#endif
    
    bool success = status == CSOUND_SUCCESS;
    
    if (!success)
    {
        char log_message_error[512];
        sprintf(log_message_error, "Setting environment variable '%s' to value '%s' failed", name, value);
        MQ_LOG_FATAL(log_message_error);
    }
    
    char log_message[512];
    sprintf(log_message, "Set environment variable '%s' to value '%s'", name, value);
    MQ_LOG_DEBUG(log_message);
    
    return success;
}
