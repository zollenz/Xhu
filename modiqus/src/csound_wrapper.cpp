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

using namespace modiqus;

modiqus::S32 mq_log_level = MQ_LOG_LEVEL_DEBUG;
bool mq_log_with_func_info = false;

// Non-member functions
static void message_callback(CSOUND *csound, S32 attr, const char *format, va_list args)
{
    mq_log_csound(format, args);
    return;
}

void mq_print_csound_return_code(const char *function_name, S32 return_code)
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
    CsoundState* state = static_cast<CsoundState*>(data);
    CsoundWrapper* core = static_cast<CsoundWrapper*>(csoundGetHostData(state->csound));

    if (state->compileResult == CSOUND_SUCCESS)
    {
        core->isPerformanceThreadRunning(true);
        MQ_LOG_DEBUG("Csound performance thread created")
        
        while (true)
        {
            // Yield for non-reentrant API functions
            if (state->yieldPerformance)
            {
                state->performanceThreadYield = true;
                MQ_LOG_DEBUG("CSOUND THREAD YIELD")
            }
            
            while (state->yieldPerformance);
            
            state->performanceThreadYield = false;
            
            if (csoundPerformKsmps(state->csound) != 0 || !state->runPerformanceThread)
            {
                break;
            }
        }
        
        core->isPerformanceThreadRunning(false);
        MQ_LOG_DEBUG("Csound performance loop stopped")
        csoundDestroy(state->csound);
        MQ_LOG_DEBUG("Csound instance destroyed")
    }
    
    return 1;
}

void onCreateSampleTable(void*, const S32 tableNumber, void* context)
{
    CsoundWrapper* wrapper = static_cast<CsoundWrapper*>(context);
    wrapper->deleteTable(tableNumber);
}

void onCreateImmediateTable(void*, const S32 tableNumber, void* context)
{
    CsoundWrapper* wrapper = static_cast<CsoundWrapper*>(context);
    wrapper->deleteTable(tableNumber);
}

void onCreateSegmentTable(void*, const S32 tableNumber, void* context)
{
    CsoundWrapper* wrapper = static_cast<CsoundWrapper*>(context);
    wrapper->deleteTable(tableNumber);
}

void onDeleteTable(void*, const S32 tableNumber, void* context)
{
    CsoundWrapper* wrapper = static_cast<CsoundWrapper*>(context);
    wrapper->deleteTable(tableNumber);
}

bool CsoundWrapper::start(bool bundle)
{
    srand((unsigned)time(NULL));
    
    // Set Csound environment variables
    String path = mq_get_executable_path();
    USize lastSlashIndex = path.rfind("/");

    path = path.substr(0, lastSlashIndex);

    if (bundle)
    {
        path += "/..";
    }

    String opcodePath = path;

    if (bundle)
    {
        opcodePath += "/Frameworks";
    }
    else
    {
        opcodePath += "/lib";
    }

    String audioPath = path + "/Resources/audio";
    String csdPath = path + "/Resources/csound/modiqus.csd";

    char log_message_1[100];
    sprintf(log_message_1, "Csound opcode directory: %s", opcodePath.c_str());
    MQ_LOG_DEBUG(log_message_1)
    char log_message_2[100];
    sprintf(log_message_2, "Csound audio directory: %s", audioPath.c_str());
    MQ_LOG_DEBUG(log_message_2)
    char log_message_3[100];
    sprintf(log_message_3, "Csound orchestra file path: %s", csdPath.c_str());
    MQ_LOG_DEBUG(log_message_3)

    S32 callResult = CSOUND_ERROR;
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
    _state.csound = csoundCreate(this);

    if (_state.csound == NULL)
    {
        MQ_LOG_FATAL( "Csound instance creation failed")

        return false;
    }

    _performanceThreadRunning = false;

    _state.compileResult = CSOUND_ERROR;
    _state.runPerformanceThread = false;

    csoundSetMessageCallback(_state.csound, message_callback);

    // Compile orchestra file
    S32 cSoundArgsCount = 2;
    char* cSoundArgs[cSoundArgsCount];
    cSoundArgs[0] = const_cast<char *>("csound");
    char temp[csdPath.size() + 1];
    strcpy(temp, csdPath.c_str());
    cSoundArgs[1] = temp;
    _state.compileResult = csoundCompile(_state.csound, cSoundArgsCount, cSoundArgs);
    mq_print_csound_return_code("cSoundCompile", _state.compileResult);

    if (_state.compileResult != CSOUND_SUCCESS)
    {
        MQ_LOG_FATAL( "Csound .csd compilation failed")

        return false;
    }


    // Start performance thread
    _state.runPerformanceThread = true;
    _state.yieldPerformance = false;
    _state.performanceThreadYield = false;

    if (csoundCreateThread(performance_routine, (void *)&_state) == NULL)
    {
        MQ_LOG_FATAL( "Csound performance thread creation failed")

        return false;
    }

    // Wait for performance thread
    while (true)
    {
        if (_performanceThreadRunning) {
            break;
        }

        mq_pause(1);
    }

    // Everything is OK...
    MQ_LOG_DEBUG("Csound wrapper initialized and performance thread running")
    
    return true;
}

void CsoundWrapper::stop()
{
    _state.runPerformanceThread = false;
}

void CsoundWrapper::setlogLevel(S32 level) const
{
    mq_log_level = level;
}

void CsoundWrapper::setOpcodePath(String path)
{
    
}

void CsoundWrapper::setAudioPath(String path)
{
    
}

void CsoundWrapper::setCsdPath(String path)
{
    
}

void CsoundWrapper::getChannelControlOutput(MYFLT& value, const char *name) const
{
    MYFLT* chnPtr = NULL;
    S32 chnType = CSOUND_OUTPUT_CHANNEL | CSOUND_CONTROL_CHANNEL;
    S32 result = csoundGetChannelPtr(_state.csound, &chnPtr, name, chnType);
    
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

void CsoundWrapper::setChannelControlInput(MYFLT value, const char *name) const
{
    MYFLT *chnPtr = NULL;
    S32 chnType = CSOUND_INPUT_CHANNEL | CSOUND_CONTROL_CHANNEL;
    S32 result = csoundGetChannelPtr(_state.csound, &chnPtr, name, chnType);
    
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

void CsoundWrapper::setControlChannelInput(MYFLT value, const char *name) const
{
    csoundSetControlChannel(_state.csound, name, value);
    char log_message[100];
    sprintf(log_message, "Value %f sent to channel %s", value, name);
    MQ_LOG_DEBUG(log_message)}

void CsoundWrapper::sendMessage(const char* message) const
{
    char log_message[100];
    sprintf(log_message, "Sending message to Csound:\n%s", message);
    MQ_LOG_DEBUG(log_message);
	csoundInputMessage(_state.csound, message);
}

void CsoundWrapper::sendScoreEvent(const char type, MYFLT* parameters, S32 numParameters)
{
    S32 result = csoundScoreEvent(_state.csound, type, parameters, numParameters);
    
    if(result == CSOUND_SUCCESS)
    {
        MQ_LOG_DEBUG("Sent score event");
    }
    else
    {
        mq_print_csound_return_code("csoundScoreEvent", result);
    }
}

const S32 CsoundWrapper::getSampleRate() const
{
    return roundToInt(csoundGetSr(_state.csound));
}

const S32 CsoundWrapper::getControlRate() const
{
    return csoundGetKr(_state.csound);
}

const S32 CsoundWrapper::getNumberOfControlSamples() const
{
    return csoundGetKsmps(_state.csound);
}

const F32 CsoundWrapper::getControlPeriod() const
{
    return 1.0f / getSampleRate() * getNumberOfControlSamples(); // ksmps duration
}

void CsoundWrapper::createSampleTable(SampleTable* const table)
{
    String message = "f " + mq_to_string<S32>(table->number) + " 0 0 ";
    message += mq_to_string<S32>(table->GENRoutine) + " \"" + table->filcod + "\" ";
    message += mq_to_string<F32>(table->skiptime) + " ";
    message += mq_to_string<S32>(table->format) + " ";
    message += mq_to_string<S32>(table->channel);
	sendMessage(message.c_str());
    
    while (!doesTableExist(table->number));
}

void CsoundWrapper::createImmediateTable(ImmediateTable* const table)
{
    USize numTables = table->tableNums.size();
    String message = "f " + mq_to_string<S32>(table->number) + " 0 ";
    message += mq_to_string<S32>(table->size)  + " -2";

    for (USize i = 0; i < numTables; i++)
    {
        message += " " + mq_to_string<S32>(table->tableNums[i]);
    }
        
	sendMessage(message.c_str());
    
    while (!doesTableExist(table->number)); // TODO: replace with safer solution
}

void CsoundWrapper::createSegmentTable(SegmentTable* const table)
{    
    USize numSegments = table->segments.size();

	// Check segment/tablesize integrity
    F32 totalLength = 0;

	for (USize i = 0; i < numSegments; i++)
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
    String message = "f " + mq_to_string<S32>(table->number);
    message += " 0 " + mq_to_string<S32>(table->size) + " -7";
    
    for (S32 i = 0; i < numSegments; i++)
    {
        message += " " + mq_to_string<F32>(table->segments.at(i).value);
        message += " " + mq_to_string<F32>(table->segments.at(i).length);
    }
	
	sendMessage(message.c_str());
    
    while (!doesTableExist(table->number));
}

const S32 CsoundWrapper::getTableData(const S32 tableNumber, F32List* const data)
{
    S32 length = -1;
    
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
    
    _state.yieldPerformance = true;

    while (!_state.performanceThreadYield);
    
    char log_message[100];
    sprintf(log_message, "Retrieving data for table %d", tableNumber);
    MQ_LOG_DEBUG(log_message);

    MYFLT* tempDataPtr = NULL;
    length = csoundGetTable(_state.csound, &tempDataPtr, tableNumber);
    
    if (length >= 0 && tempDataPtr != NULL)
    {
        data->clear();
        data->resize(length);
        
        for (S32 i = 0; i < length; i++)
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
    
    _state.yieldPerformance = false;
    
    return length;
}

void CsoundWrapper::setTableData(const S32 table, const F32List* const data)
{
    if (table == TABLE_UNDEFINED)
    {
        MQ_LOG_DEBUG("Table is undefined.");
        return;
    }
    
    _state.yieldPerformance = true;
    
    while (!_state.performanceThreadYield);
    
    USize dataSize = data->size();
    
    for (S32 i = 0; i < dataSize; i++)
    {
        csoundTableSet(_state.csound, table, i, data->at(i));
    }
    
    _state.yieldPerformance = false;
}

const F32 CsoundWrapper::getTableValue(const S32 tableNumber, const S32 index)
{
    F32 value = NAN;
    
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
    
    _state.yieldPerformance = true;
    
    while (!_state.performanceThreadYield);
    
    value = csoundTableGet(_state.csound, tableNumber, index);
    _state.yieldPerformance = false;
    
    return value;
}

bool CsoundWrapper::doesTableExist(S32 tableNumber)
{
    bool exists = false;
    
    if (tableNumber == 0)
    {
        MQ_LOG_ERROR("Could not retrieve data. Table is undefined.")
        
        return exists;
    }

    S32 length = -1;
    MYFLT* tablePtr = NULL;    
    _state.yieldPerformance = true;
    
    while (!_state.performanceThreadYield);
    
    length = csoundGetTable(_state.csound, &tablePtr, tableNumber);
    _state.yieldPerformance = false;
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

void CsoundWrapper::deleteTable(const S32 tableNum)
{
    if (tableNum == TABLE_UNDEFINED)
    {
        MQ_LOG_DEBUG("Table is undefined.")
        
        return;
    }
    
    if (doesTableExist(tableNum)) {
        char log_message[100];
        sprintf(log_message, "Deleting table %d", tableNum);
        MQ_LOG_DEBUG(log_message)
        
        String message = "f -" + mq_to_string<S32>(tableNum) + " 0";
        sendMessage(message.c_str());
    }
    else
    {
        char log_message[100];
        sprintf(log_message, "Table %d does not exist.", tableNum);
        MQ_LOG_DEBUG(log_message)
    }
}

bool CsoundWrapper::isPerformanceThreadRunning() const
{
    return _performanceThreadRunning;
}

void CsoundWrapper::isPerformanceThreadRunning(bool running)
{
    _performanceThreadRunning = running;
}
