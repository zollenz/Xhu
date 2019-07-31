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

#include <time.h>
#include "xhu_debug.h"
#include "xhu_csound_wrapper.h"
#include "xhu_system_utilities.h"
#include "xhu_math_utilities.h"

//#define MACOS_BUNDLE

typedef struct {
    CSOUND* csound;
    xhu_s32_t compile_result;
    bool run_performance_thread;
    bool pause_csound_thread;
    bool csound_thread_paused;
} xhu_csound_state_t;

xhu_csound_state_t _xhu_csound_state;
volatile bool _xhu_perf_thread_running;

xhu_s32_t xhu_log_level = XHU_LOG_LEVEL_DEBUG;
bool xhu_log_with_func_info = false;

static const char *xhu_executable_path;
static char xhu_opcode_path[PATH_MAX];
static char xhu_csd_path[PATH_MAX];
static char xhu_audio_path[PATH_MAX];

static void xhu_msg_callback(CSOUND *csound, xhu_s32_t attr, const char *format, va_list args)
{
    xhu_log_csound(format, args);
    return;
}

static void xhu_set_executable_path()
{
#ifdef __APPLE__
    char full_path[PATH_MAX];
    uint32_t size = sizeof(full_path);
    
    if (_NSGetExecutablePath(full_path, &size) != 0) {
        full_path[0] = '\0';
        XHU_LOG_ERROR("Buffer too small; need size %u", size)
    }
    
    xhu_executable_path = xhu_get_folder_path(full_path);
    XHU_LOG_DEBUG("Executable path is %s", xhu_executable_path)
#else
    //    char temp[PATH_MAX];
    //    return (getcwd(temp, PATH_MAX) ? xhu_str(temp) : xhu_str(""));
    xhu_executable_path = "";
#endif
}


static void xhu_print_csound_return_code(const char *function_name, xhu_s32_t return_code)
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
    
    XHU_LOG_DEBUG("Csound function '%s' returned %d (%s)", function_name, return_code, return_code_str);
#endif
}

uintptr_t csound_thread(void* data)
{
    xhu_csound_state_t* state = (xhu_csound_state_t*)data;
    
    if (state->compile_result == CSOUND_SUCCESS) {
        _xhu_perf_thread_running = true;
        XHU_LOG_DEBUG("Csound performance thread created")
        
        while (true) {
            // Yield for non-reentrant API functions
            if (state->pause_csound_thread) {
                state->csound_thread_paused = true;
                XHU_LOG_DEBUG("CSOUND THREAD YIELD")
            }
            
            while (state->pause_csound_thread);
            
            state->csound_thread_paused = false;
            
            if (csoundPerformKsmps(state->csound) != 0 || !state->run_performance_thread) {
                break;
            }
        }
        
        _xhu_perf_thread_running = false;
        XHU_LOG_DEBUG("Csound performance loop stopped")
        csoundDestroy(state->csound);
        XHU_LOG_DEBUG("Csound instance destroyed")
    }
    
    return 1;
}

void xhu_set_opcode_path(const char *path)
{
#ifdef MACOS_BUNDLE
    sprintf(xhu_opcode_path, "%s%s", xhu_executable_path, "/../Frameworks");
#else
    sprintf(xhu_opcode_path, "%s%s", xhu_executable_path, "/lib");
#endif
    xhu_set_global_env("OPCODE6DIR64", xhu_opcode_path);
}

void xhu_set_audio_path(const char *path)
{
    const char *suffix = "/Resources/audio";
#ifdef MACOS_BUNDLE
    sprintf(xhu_audio_path, "%s%s%s", xhu_executable_path, "/..", suffix);
#else
    sprintf(xhu_audio_path, "%s%s", xhu_executable_path, suffix);
#endif
    xhu_set_global_env("SSDIR", xhu_audio_path);
}

void xhu_set_csd_path(const char *path)
{
    const char *suffix = "/Resources/csound/xhu.csd";
#ifdef MACOS_BUNDLE
    sprintf(xhu_csd_path, "%s%s%s", xhu_executable_path, "/../", suffix);
#else
    sprintf(xhu_csd_path, "%s%s", xhu_executable_path, suffix);
#endif
}

bool xhu_start(bool bundle)
{
    srand((unsigned)time(NULL));
    
    // Set Csound environment variables
    xhu_set_executable_path();
    xhu_set_opcode_path(NULL);
    xhu_set_csd_path(NULL);
    xhu_set_audio_path(NULL);
    
    // Create Csound instance
    _xhu_csound_state.csound = csoundCreate(NULL);
    
    if (_xhu_csound_state.csound == NULL) {
        XHU_LOG_FATAL( "Csound instance creation failed")
        return false;
    }
    
    _xhu_perf_thread_running = false;
    
    _xhu_csound_state.compile_result = CSOUND_ERROR;
    _xhu_csound_state.run_performance_thread = false;
    
    csoundSetMessageCallback(_xhu_csound_state.csound, xhu_msg_callback);
    
    // Compile orchestra file
    xhu_s32_t cSoundArgsCount = 2;
    char* cSoundArgs[cSoundArgsCount];
    cSoundArgs[0] = "csound";
    char temp[strlen(xhu_csd_path) + 1];
    strcpy(temp, xhu_csd_path);
    cSoundArgs[1] = temp;
    _xhu_csound_state.compile_result = csoundCompile(_xhu_csound_state.csound, cSoundArgsCount, cSoundArgs);
    xhu_print_csound_return_code("cSoundCompile", _xhu_csound_state.compile_result);
    
    if (_xhu_csound_state.compile_result != CSOUND_SUCCESS) {
        XHU_LOG_FATAL( "Csound .csd compilation failed")
        
        return false;
    }
    
    // Start performance thread
    _xhu_csound_state.run_performance_thread = true;
    _xhu_csound_state.pause_csound_thread = false;
    _xhu_csound_state.csound_thread_paused = false;
    
    if (csoundCreateThread(csound_thread, (void *)&_xhu_csound_state) == NULL) {
        XHU_LOG_FATAL( "Csound performance thread creation failed")
        
        return false;
    }
    
    // Wait for performance thread
    while (true) {
        if (_xhu_perf_thread_running) {
            break;
        }
        xhu_pause(1);
    }
    
    // Everything is OK...
    XHU_LOG_DEBUG("Csound wrapper initialized and performance thread running")
    
    return true;
}

void xhu_stop(void)
{
    _xhu_csound_state.run_performance_thread = false;
}

void xhu_set_log_level(xhu_s32_t level)
{
    xhu_log_level = level;
}

xhu_audio_data_t *xhu_get_channel_pointer(const char *name, xhu_s32_t flags)
{
    xhu_audio_data_t *channel;
    xhu_s32_t result = csoundGetChannelPtr(_xhu_csound_state.csound, &channel, name, flags);
    
    if (result == CSOUND_SUCCESS) {
        XHU_LOG_DEBUG("Got pointer to channel %s", name)
        return channel;
    }
    
    xhu_print_csound_return_code("csoundGetChannelPtr", result);
    XHU_LOG_ERROR("Could not get pointer to channel %s", name)
    return NULL;
}

xhu_audio_data_t xhu_get_control_channel_value(xhu_audio_data_t *channel)
{
    return *channel;
}

void xhu_set_control_channel_value(xhu_audio_data_t value, const char *name)
{
    xhu_audio_data_t *chnPtr = NULL;
    xhu_s32_t chnType = CSOUND_INPUT_CHANNEL | CSOUND_CONTROL_CHANNEL;
    xhu_s32_t result = csoundGetChannelPtr(_xhu_csound_state.csound, &chnPtr, name, chnType);
    
    if (result == CSOUND_SUCCESS) {
        *chnPtr = (xhu_audio_data_t)value;
        XHU_LOG_DEBUG("Value %f sent to channel %s", value, name)
    } else {
        xhu_print_csound_return_code("csoundGetChannelPtr", result);
    }
}

void xhu_send_message(const char* message)
{
    XHU_LOG_DEBUG("Sending message to Csound:\n%s", message);
    csoundInputMessage(_xhu_csound_state.csound, message);
}

void xhu_send_score_event(const char type, xhu_audio_data_t* parameters, xhu_s32_t numParameters)
{
    xhu_s32_t result = csoundScoreEvent(_xhu_csound_state.csound, type, parameters, numParameters);
    
    if(result == CSOUND_SUCCESS) {
        XHU_LOG_DEBUG("Sent score event");
    } else {
        xhu_print_csound_return_code("csoundScoreEvent", result);
    }
}

const xhu_s32_t xhu_get_sample_rate(void)
{
    return xhu_round_to_int(csoundGetSr(_xhu_csound_state.csound));
}

const xhu_s32_t xhu_get_control_rate(void)
{
    return csoundGetKr(_xhu_csound_state.csound);
}

const xhu_s32_t xhu_get_control_size(void)
{
    return csoundGetKsmps(_xhu_csound_state.csound);
}

const xhu_f32_t xhu_get_control_period(void)
{
    return 1.0f / xhu_get_sample_rate() * xhu_get_control_size(); // ksmps duration
}

const xhu_s32_t xhu_get_table_data(const xhu_s32_t table_id, xhu_audio_data_t *data)
{
    xhu_s32_t length = -1;
    
    if (table_id == 0)
    {
        XHU_LOG_ERROR("Table ID is undefined.")
        
        return length;
    }
    
    if (data == NULL)
    {
        XHU_LOG_ERROR("Data pointer is NULL.")
        
        return length;
    }
    
    _xhu_csound_state.pause_csound_thread = true;
    
    while (!_xhu_csound_state.csound_thread_paused);
    
    length = csoundGetTable(_xhu_csound_state.csound, &data, table_id);
    
    if (length >= 0) {
        XHU_LOG_DEBUG("Got data for table %d", table_id);
    } else {
        XHU_LOG_ERROR("Could not get data for table %d. Table does not exist.", table_id)
    }
    
    _xhu_csound_state.pause_csound_thread = false;
    
    return length;
}

void xhu_set_table_data(const xhu_s32_t table, const xhu_audio_data_t *const data, xhu_u32_t data_count)
{
    if (table == TABLE_UNDEFINED) {
        XHU_LOG_DEBUG("Table is undefined.");
        return;
    }
    
    _xhu_csound_state.pause_csound_thread = true;
    
    while (!_xhu_csound_state.csound_thread_paused);
    
    for (xhu_u32_t i = 0; i < data_count; ++i) {
        csoundTableSet(_xhu_csound_state.csound, table, i, data[i]);
    }
    
    _xhu_csound_state.pause_csound_thread = false;
}

const xhu_f32_t xhu_get_table_val(const xhu_s32_t tableNumber, const xhu_s32_t index)
{
    xhu_f32_t value = NAN;
    
    if (tableNumber <= 0) {
        XHU_LOG_ERROR("Could not retrieve data. Invalid table number.")
        
        return value;
    }
    
    if (index < 0) {
        XHU_LOG_ERROR("Could not retrieve data. Index is negative.")
        
        return value;
    }
    
    _xhu_csound_state.pause_csound_thread = true;
    
    while (!_xhu_csound_state.csound_thread_paused);
    
    value = csoundTableGet(_xhu_csound_state.csound, tableNumber, index);
    _xhu_csound_state.pause_csound_thread = false;
    
    return value;
}

bool xhu_table_exists(xhu_s32_t tableNumber)
{
    bool exists = false;
    
    if (tableNumber == 0) {
        XHU_LOG_ERROR("Could not retrieve data. Table is undefined.")
        
        return exists;
    }
    
    xhu_s32_t length = -1;
    xhu_audio_data_t *tablePtr = NULL;
    _xhu_csound_state.pause_csound_thread = true;
    
    while (!_xhu_csound_state.csound_thread_paused);
    
    length = csoundGetTable(_xhu_csound_state.csound, &tablePtr, tableNumber);
    _xhu_csound_state.pause_csound_thread = false;
    exists = length > 0 && tablePtr != NULL;
    
    if (exists) {
        XHU_LOG_DEBUG("Table %d exists.", tableNumber)
    } else {
        XHU_LOG_DEBUG("Table %d does not exist.", tableNumber)
    }
    
    return exists;
}

void xhu_delete_table(const xhu_s32_t table_id)
{
    if (table_id == TABLE_UNDEFINED) {
        XHU_LOG_DEBUG("Table is undefined.")
        
        return;
    }
    
    if (xhu_table_exists(table_id)) {
        char message[50];
        sprintf(message, "f -%d 0", table_id);
        xhu_send_message(message);
        
        XHU_LOG_DEBUG("Deleted table %d", table_id)
    } else {
        XHU_LOG_DEBUG("Table %d does not exist.", table_id)
    }
}

bool xhu_set_global_env(const char *name, const char *value)
{
    xhu_s32_t status = csoundSetGlobalEnv(name, value);
#ifdef DEBUG
    xhu_print_csound_return_code("csoundSetGlobalEnv", status);
#endif
    
    bool success = status == CSOUND_SUCCESS;
    
    if (!success) {
        XHU_LOG_FATAL("Setting environment variable '%s' to value '%s' failed", name, value);
    }
    
    XHU_LOG_DEBUG("Set environment variable '%s' to value '%s'", name, value);
    
    return success;
}

void xhu_set_output_channel_callback(channelCallback_t callback)
{
    csoundSetOutputChannelCallback(_xhu_csound_state.csound, callback);
}
