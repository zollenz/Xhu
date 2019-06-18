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

#include "math_utilities.h"
#include "system_utilities.h"
#include "types.h"
#include "csound_wrapper.h"

mq_s32_t mq_log_level = MQ_LOG_LEVEL_DEBUG;
bool mq_log_with_func_info = false;

static void mq_msg_callback(CSOUND *csound, mq_s32_t attr, const char *format, va_list args)
{
    mq_log_csound(format, args);
    return;
}

void mq_print_csound_return_code(const char *function_name, mq_s32_t return_code)
{
#ifdef DEBUG
    const char *return_code_str;
    
    if (return_code == CSOUND_EXITJMP_SUCCESS)
    {
        /* terminated, but not error (e.g. --help, -U, or exitnow) */
        return_code_str = "early return";
        return_code = 0;
    }
    else if (return_code > 0 && strncmp(function_name, "csoundPerform", 13) == 0)
    {
        /* for csoundPerformKsmps and csoundPerformBuffer only */
        return_code_str = "end of score or MIDI file";
        return_code = 0;
    }
    else
    {
        switch (return_code)
        {
            case CSOUND_SUCCESS:
            {
                return_code_str = "success";
                break;
            }
            case CSOUND_ERROR:
            {
                return_code_str = "error";
                break;
            }
            case CSOUND_INITIALIZATION:
            {
                return_code_str = "initialization error";
                break;
            }
            case CSOUND_PERFORMANCE:
            {
                return_code_str = "performance error";
                break;
            }
            case CSOUND_MEMORY:
            {
                return_code_str = "memory allocation error";
                break;
            }
            case CSOUND_SIGNAL:
            {
                return_code_str = "caught SIGINT or SIGTERM";
                break;
            }
            default:
            {
                return_code_str = "error";
                break;
            }
        }
    }
    
    char log_message[100];
    sprintf(log_message, "Csound function '%s' returned %d (%s)", function_name, return_code, return_code_str);
    MQ_LOG_DEBUG(log_message);
#endif
}

uintptr_t performance_routine(void* data)
{
    mq_csound_state_t* state = static_cast<mq_csound_state_t*>(data);
    CsoundWrapper* core = static_cast<CsoundWrapper*>(csoundGetHostData(state->csound));

    if (state->compile_result == CSOUND_SUCCESS)
    {
        core->set_perf_thread_running(true);
        MQ_LOG_DEBUG("Csound performance thread created")
        
        while (true)
        {
            // Yield for non-reentrant API functions
            if (state->yield_performance_thread)
            {
                state->performance_thread_yield = true;
                MQ_LOG_DEBUG("CSOUND THREAD YIELD")
            }
            
            while (state->yield_performance_thread);
            
            state->performance_thread_yield = false;
            
            if (csoundPerformKsmps(state->csound) != 0 || !state->run_performance_thread)
            {
                break;
            }
        }
        
        core->set_perf_thread_running(false);
        MQ_LOG_DEBUG("Csound performance loop stopped")
        csoundDestroy(state->csound);
        MQ_LOG_DEBUG("Csound instance destroyed")
    }
    
    return 1;
}

bool CsoundWrapper::mq_start(bool bundle)
{
    srand((unsigned)time(NULL));
    
    // Set Csound environment variables
    mq_str_t path = mq_get_executable_path();
    mq_list_size_t lastSlashIndex = path.rfind("/");

    path = path.substr(0, lastSlashIndex);

    if (bundle)
    {
        path += "/..";
    }

    mq_str_t opcodePath = path;

    if (bundle)
    {
        opcodePath += "/Frameworks";
    }
    else
    {
        opcodePath += "/lib";
    }

    mq_str_t audioPath = path + "/Resources/audio";
    mq_str_t csdPath = path + "/Resources/csound/modiqus.csd";

    char log_message_1[100];
    sprintf(log_message_1, "Csound opcode directory: %s", opcodePath.c_str());
    MQ_LOG_DEBUG(log_message_1)
    char log_message_2[100];
    sprintf(log_message_2, "Csound audio directory: %s", audioPath.c_str());
    MQ_LOG_DEBUG(log_message_2)
    char log_message_3[100];
    sprintf(log_message_3, "Csound orchestra file path: %s", csdPath.c_str());
    MQ_LOG_DEBUG(log_message_3)

    mq_s32_t callResult = CSOUND_ERROR;
    callResult = csoundSetGlobalEnv("OPCODE6DIR64", opcodePath.c_str());
    mq_print_csound_return_code("csoundSetGlobalEnv", callResult);

    if (callResult != CSOUND_SUCCESS)
    {
        MQ_LOG_FATAL( "Csound initialization of environment variable 'OPCODE6DIR' failed")

        return false;
    }

    callResult = csoundSetGlobalEnv("SSDIR", audioPath.c_str());
    mq_print_csound_return_code("csoundSetGlobalEnv", callResult);

    if (callResult != CSOUND_SUCCESS)
    {
        MQ_LOG_FATAL( "Csound initialization of environment variable 'SSDIR' failed")

        return false;
    }

    // Create Csound instance
    _mq_csound_state.csound = csoundCreate(this);

    if (_mq_csound_state.csound == NULL)
    {
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
    cSoundArgs[0] = const_cast<char *>("csound");
    char temp[csdPath.size() + 1];
    strcpy(temp, csdPath.c_str());
    cSoundArgs[1] = temp;
    _mq_csound_state.compile_result = csoundCompile(_mq_csound_state.csound, cSoundArgsCount, cSoundArgs);
    mq_print_csound_return_code("cSoundCompile", _mq_csound_state.compile_result);

    if (_mq_csound_state.compile_result != CSOUND_SUCCESS)
    {
        MQ_LOG_FATAL( "Csound .csd compilation failed")

        return false;
    }


    // Start performance thread
    _mq_csound_state.run_performance_thread = true;
    _mq_csound_state.yield_performance_thread = false;
    _mq_csound_state.performance_thread_yield = false;

    if (csoundCreateThread(performance_routine, (void *)&_mq_csound_state) == NULL)
    {
        MQ_LOG_FATAL( "Csound performance thread creation failed")

        return false;
    }

    // Wait for performance thread
    while (true)
    {
        if (_mq_perf_thread_running) {
            break;
        }

        mq_pause(1);
    }

    // Everything is OK...
    MQ_LOG_DEBUG("Csound wrapper initialized and performance thread running")
    
    return true;
}

void CsoundWrapper::mq_stop()
{
    _mq_csound_state.run_performance_thread = false;
}

void CsoundWrapper::mq_set_log_level(mq_s32_t level) const
{
    mq_log_level = level;
}

void CsoundWrapper::mq_set_opcode_path(mq_str_t path)
{
    
}

void CsoundWrapper::mq_set_audio_path(mq_str_t path)
{
    
}

void CsoundWrapper::mq_set_csd_path(mq_str_t path)
{
    
}

void CsoundWrapper::mq_get_chn_ctrl_output(MYFLT& value, const char *name) const
{
    MYFLT* chnPtr = NULL;
    mq_s32_t chnType = CSOUND_OUTPUT_CHANNEL | CSOUND_CONTROL_CHANNEL;
    mq_s32_t result = csoundGetChannelPtr(_mq_csound_state.csound, &chnPtr, name, chnType);
    
    if (result == CSOUND_SUCCESS)
    {
        value = *chnPtr;
        char log_message[100];
        sprintf(log_message, "Value %f received from channel %s", value, name);
        MQ_LOG_DEBUG(log_message)
    }
    else
    {
        mq_print_csound_return_code("csoundGetChannelPtr", result);
    }
}

void CsoundWrapper::mq_set_chn_ctrl_value(MYFLT value, const char *name) const
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

void CsoundWrapper::mq_send_message(const char* message) const
{
    char log_message[100];
    sprintf(log_message, "Sending message to Csound:\n%s", message);
    MQ_LOG_DEBUG(log_message);
	csoundInputMessage(_mq_csound_state.csound, message);
}

void CsoundWrapper::mq_send_score_event(const char type, MYFLT* parameters, mq_s32_t numParameters)
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

const mq_s32_t CsoundWrapper::mq_get_sample_rate() const
{
    return mq_round_to_int(csoundGetSr(_mq_csound_state.csound));
}

const mq_s32_t CsoundWrapper::mq_get_control_rate() const
{
    return csoundGetKr(_mq_csound_state.csound);
}

const mq_s32_t CsoundWrapper::mq_get_control_size() const
{
    return csoundGetKsmps(_mq_csound_state.csound);
}

const mq_f32_t CsoundWrapper::mq_get_control_period() const
{
    return 1.0f / mq_get_sample_rate() * mq_get_control_size(); // ksmps duration
}

void CsoundWrapper::mq_create_sample_table(SampleTable* const table)
{
    mq_str_t message = "f " + mq_to_string<mq_s32_t>(table->number) + " 0 0 ";
    message += mq_to_string<mq_s32_t>(table->GENRoutine) + " \"" + table->filcod + "\" ";
    message += mq_to_string<mq_f32_t>(table->skiptime) + " ";
    message += mq_to_string<mq_s32_t>(table->format) + " ";
    message += mq_to_string<mq_s32_t>(table->channel);
	mq_send_message(message.c_str());
    
    while (!mq_table_exists(table->number));
}

void CsoundWrapper::mq_create_immediate_table(ImmediateTable* const table)
{
    mq_list_size_t numTables = table->tableNums.size();
    mq_str_t message = "f " + mq_to_string<mq_s32_t>(table->number) + " 0 ";
    message += mq_to_string<mq_s32_t>(table->size)  + " -2";

    for (mq_list_size_t i = 0; i < numTables; i++)
    {
        message += " " + mq_to_string<mq_s32_t>(table->tableNums[i]);
    }
        
	mq_send_message(message.c_str());
    
    while (!mq_table_exists(table->number)); // TODO: replace with safer solution
}

void CsoundWrapper::mq_create_segment_table(SegmentTable* const table)
{    
    mq_list_size_t numSegments = table->segments.size();

	// Check segment/tablesize integrity
    mq_f32_t totalLength = 0;

	for (mq_list_size_t i = 0; i < numSegments; i++)
    {
		totalLength += table->segments.at(i).length;
	}
    
    if (totalLength < table->size)
    {
        MQ_LOG_WARN("Segment length sum is less than table size. Padding table end with zeros.");
    }
    
    if (totalLength > table->size)
    {
        MQ_LOG_WARN("Segment length sum is bigger than table size. Excess segments will not be included.");
    }
    
	//Create score event:
    mq_str_t message = "f " + mq_to_string<mq_s32_t>(table->number);
    message += " 0 " + mq_to_string<mq_s32_t>(table->size) + " -7";
    
    for (mq_s32_t i = 0; i < numSegments; i++)
    {
        message += " " + mq_to_string<mq_f32_t>(table->segments.at(i).value);
        message += " " + mq_to_string<mq_f32_t>(table->segments.at(i).length);
    }
	
	mq_send_message(message.c_str());
    
    while (!mq_table_exists(table->number));
}

const mq_s32_t CsoundWrapper::mq_get_table_data(const mq_s32_t tableNumber, mq_f32_list_t* const data)
{
    mq_s32_t length = -1;
    
    if (tableNumber == 0)
    {
        MQ_LOG_ERROR("Could not retrieve data. Table is undefined.")
        
        return length;
    }
    
    if (data == NULL)
    {
        MQ_LOG_ERROR("NULL arg pointer, could not return data.")
        
        return length;
    }
    
    _mq_csound_state.yield_performance_thread = true;

    while (!_mq_csound_state.performance_thread_yield);
    
    char log_message[100];
    sprintf(log_message, "Retrieving data for table %d", tableNumber);
    MQ_LOG_DEBUG(log_message);

    MYFLT* tempDataPtr = NULL;
    length = csoundGetTable(_mq_csound_state.csound, &tempDataPtr, tableNumber);
    
    if (length >= 0 && tempDataPtr != NULL)
    {
        data->clear();
        data->resize(length);
        
        for (mq_s32_t i = 0; i < length; i++)
        {
            data->at(i) = tempDataPtr[i];
            //        samples[i] = tempSamples[i] / CSOUND_0DBFS;
            //        MQ_LOG_DEBUG("samples[" + toString<S32>(i) + "] = " + toString<F32>(samples[i]));
        }
    }
#ifdef DEBUG
    else
    {
        char log_message[100];
        sprintf(log_message, "Could not retrieve data for table %d. Table does not exist.", tableNumber);
        MQ_LOG_ERROR(log_message)
    }
#endif
    
    _mq_csound_state.yield_performance_thread = false;
    
    return length;
}

void CsoundWrapper::mq_set_table_data(const mq_s32_t table, const mq_f32_list_t* const data)
{
    if (table == TABLE_UNDEFINED)
    {
        MQ_LOG_DEBUG("Table is undefined.");
        return;
    }
    
    _mq_csound_state.yield_performance_thread = true;
    
    while (!_mq_csound_state.performance_thread_yield);
    
    mq_list_size_t dataSize = data->size();
    
    for (mq_s32_t i = 0; i < dataSize; i++)
    {
        csoundTableSet(_mq_csound_state.csound, table, i, data->at(i));
    }
    
    _mq_csound_state.yield_performance_thread = false;
}

const mq_f32_t CsoundWrapper::mq_get_table_val(const mq_s32_t tableNumber, const mq_s32_t index)
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
    
    _mq_csound_state.yield_performance_thread = true;
    
    while (!_mq_csound_state.performance_thread_yield);
    
    value = csoundTableGet(_mq_csound_state.csound, tableNumber, index);
    _mq_csound_state.yield_performance_thread = false;
    
    return value;
}

bool CsoundWrapper::mq_table_exists(mq_s32_t tableNumber)
{
    bool exists = false;
    
    if (tableNumber == 0)
    {
        MQ_LOG_ERROR("Could not retrieve data. Table is undefined.")
        
        return exists;
    }

    mq_s32_t length = -1;
    MYFLT* tablePtr = NULL;    
    _mq_csound_state.yield_performance_thread = true;
    
    while (!_mq_csound_state.performance_thread_yield);
    
    length = csoundGetTable(_mq_csound_state.csound, &tablePtr, tableNumber);
    _mq_csound_state.yield_performance_thread = false;
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

void CsoundWrapper::mq_delete_table(const mq_s32_t tableNum)
{
    if (tableNum == TABLE_UNDEFINED)
    {
        MQ_LOG_DEBUG("Table is undefined.")
        
        return;
    }
    
    if (mq_table_exists(tableNum)) {
        char log_message[100];
        sprintf(log_message, "Deleting table %d", tableNum);
        MQ_LOG_DEBUG(log_message)
        
        mq_str_t message = "f -" + mq_to_string<mq_s32_t>(tableNum) + " 0";
        mq_send_message(message.c_str());
    }
    else
    {
        char log_message[100];
        sprintf(log_message, "Table %d does not exist.", tableNum);
        MQ_LOG_DEBUG(log_message)
    }
}

bool CsoundWrapper::mq_get_perf_thread_running() const
{
    return _mq_perf_thread_running;
}

void CsoundWrapper::set_perf_thread_running(bool running)
{
    _mq_perf_thread_running = running;
}
