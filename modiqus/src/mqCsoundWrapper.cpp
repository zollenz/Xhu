/*
 * Copyright (C) 2015 by Martin Dejean
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

#include "mq_utils_math.h"
#include "mq_utils_system.h"
#include "mq_types.h"
#include "mqCsoundWrapper.h"

using namespace mq;

// Non-member functions
static void noMsg_CB(
                    CSOUND *csound,
                    S32 attr,
                    const char *format,
                    va_list args
                    )
{
    return;
}

S32 sanityCheck(const char *name, S32 errorCode)
{
#ifdef DEBUG
    mq_str msg = "====== " + mq_str(name);
    msg += "() returned: " + toString<S32>(errorCode) + " (";
    
    if (errorCode == CSOUND_EXITJMP_SUCCESS)
    {
        /* terminated, but not error (e.g. --help, -U, or exitnow) */
        msg += "early return";
        errorCode = 0;
    }
    else if (errorCode > 0 && strncmp(name, "csoundPerform", 13) == 0)
    {
        /* for csoundPerformKsmps() and csoundPerformBuffer() only */
        msg += "end of score or MIDI file";
        errorCode = 0;
    }
    else
    {
        switch (errorCode)
        {
            case CSOUND_SUCCESS:
            {
                msg += "success";
                break;
            }
            case CSOUND_ERROR:
            {
                msg += "error";
                break;
            }
            case CSOUND_INITIALIZATION:
            {
                msg += "initialization error";
                break;
            }
            case CSOUND_PERFORMANCE:
            {
                msg += "performance error";
                break;
            }
            case CSOUND_MEMORY:
            {
                msg += "memory allocation error";
                break;
            }
            case CSOUND_SIGNAL:
            {
                msg += "caught SIGINT or SIGTERM";
                break;
            }
            default:
            {
                msg += "error";
                break;
            }
        }
    }
    
    msg += ") ======";
    MQ_LOG_INFO( msg);
#endif
    return errorCode;
}

uintptr_t performanceRoutine(void* data)
{
    CsoundState* state = static_cast<CsoundState*>(data);
    mqCsoundWrapper* core = static_cast<mqCsoundWrapper*>(csoundGetHostData(state->csound));

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
    mqCsoundWrapper* wrapper = static_cast<mqCsoundWrapper*>(context);
    wrapper->deleteTable(tableNumber);
}

void onCreateImmediateTable(void*, const S32 tableNumber, void* context)
{
    mqCsoundWrapper* wrapper = static_cast<mqCsoundWrapper*>(context);
    wrapper->deleteTable(tableNumber);
}

void onCreateSegmentTable(void*, const S32 tableNumber, void* context)
{
    mqCsoundWrapper* wrapper = static_cast<mqCsoundWrapper*>(context);
    wrapper->deleteTable(tableNumber);
}

void onDeleteTable(void*, const S32 tableNumber, void* context)
{
    mqCsoundWrapper* wrapper = static_cast<mqCsoundWrapper*>(context);
    wrapper->deleteTable(tableNumber);
}

bool mqCsoundWrapper::start()
{
    // Set Csound environment variables
    mq_str executablePath = getExecutablePath();
    USize lastSlashIndex = executablePath.rfind("/");
    
    // For application bundle ////////////////////////////////////////////////
//    USize lastSlashIndex = executablePath.rfind("/");
//    mq_str executableDirectoryPath = executablePath.substr(0, lastSlashIndex - 1);
//    lastSlashIndex = executableDirectoryPath.rfind("/");
//    executableDirectoryPath = executableDirectoryPath.substr(0, lastSlashIndex);
//    mq_str relativeOpcodePath = "/Frameworks/CsoundLib.framework/Resources/Opcodes";
//    mq_str relativeCSDPath = "/Resources/csound/modiqus.csd";
    //////////////////////////////////////////////////////////////////////////
    
    // For command-line tool /////////////////////////////////////////////////
    mq_str executableDirectoryPath = executablePath.substr(0, lastSlashIndex);
    //////////////////////////////////////////////////////////////////////////
    
    mq_str opcodePath = executableDirectoryPath + "/../Frameworks/CsoundLib.framework/Versions/Current/Resources";
    mq_str audioPath = executableDirectoryPath + "/../Resources/audio";
    mq_str csdPath = executableDirectoryPath + "/../Resources/csound/modiqus.csd";
    
    MQ_LOG_DEBUG("Csound opcode directory: " + opcodePath)
    MQ_LOG_DEBUG("Csound audio directory: " + audioPath)
    MQ_LOG_DEBUG("Csound orchestra file path: " + csdPath)
    
    S32 callResult = CSOUND_ERROR;
    callResult = csoundSetGlobalEnv("OPCODE6DIR", opcodePath.c_str());
    sanityCheck("csoundSetGlobalEnv", callResult);
    
    if (callResult != CSOUND_SUCCESS)
    {
        MQ_LOG_FATAL( "Csound initialization of environment variable 'OPCODE6DIR' failed")
        
        return false;
    }
    
    callResult = csoundSetGlobalEnv("SSDIR", audioPath.c_str());
    sanityCheck("csoundSetGlobalEnv", callResult);
    
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
    
    if (dbgLevel < MQ_LOG_LEVEL_INFO)
    {
        csoundSetMessageCallback(_state.csound, noMsg_CB);
    }
    
    // Compile orchestra file
    S32 cSoundArgsCount = 2;
    char* cSoundArgs[cSoundArgsCount];
    cSoundArgs[0] = const_cast<char *>("csound");
    char temp[csdPath.size() + 1];
//    char* temp = new char[_csdPath.size() + 1];
    strcpy(temp, csdPath.c_str());
    cSoundArgs[1] = temp;
    _state.compileResult = csoundCompile(_state.csound, cSoundArgsCount, cSoundArgs);
//    delete temp;
    sanityCheck("cSoundCompile", _state.compileResult);
    
    if (_state.compileResult != CSOUND_SUCCESS)
    {
        MQ_LOG_FATAL( "Csound .csd compilation failed")
        
        return false;
    }
    
    _ksmpsDuration = 1.0f / getSampleRate() * getNumberOfControlSamples();

    // Start performance thread
    _state.runPerformanceThread = true;
    _state.yieldPerformance = false;
    _state.performanceThreadYield = false;
    
    if (csoundCreateThread(performanceRoutine, (void *)&_state) == NULL)
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
        
        pause(1);
    }
    
    // Everything is OK...
    MQ_LOG_DEBUG("Csound wrapper initialized and performance thread running")
    
    return true;
}

void mqCsoundWrapper::stop()
{
    _state.runPerformanceThread = false;
}

void mqCsoundWrapper::getChannelControlOutput(MYFLT& value, const char *name) const
{
    MYFLT* chnPtr = NULL;
    S32 chnType = CSOUND_OUTPUT_CHANNEL | CSOUND_CONTROL_CHANNEL;
    S32 result = csoundGetChannelPtr(_state.csound, &chnPtr, name, chnType);
    
    if (result == CSOUND_SUCCESS)
    {
        value = *chnPtr;
        MQ_LOG_DEBUG("Value " + toString(value) + " received from channel " + name)
    }
    else
    {
        sanityCheck("csoundGetChannelPtr", result);
    }
}

void mqCsoundWrapper::setChannelControlInput(MYFLT value, const char *name) const
{
    MYFLT *chnPtr = NULL;
    S32 chnType = CSOUND_INPUT_CHANNEL | CSOUND_CONTROL_CHANNEL;
    S32 result = csoundGetChannelPtr(_state.csound, &chnPtr, name, chnType);
    
    if (result == CSOUND_SUCCESS)
    {
        *chnPtr = (MYFLT)value;
        MQ_LOG_DEBUG("Value " + toString(value) + " sent to channel " + name)
    }
    else
    {
        sanityCheck("csoundGetChannelPtr", result);
    }
}

void mqCsoundWrapper::setControlChannelInput(MYFLT value, const char *name) const
{
    csoundSetControlChannel(_state.csound, name, value);
    MQ_LOG_DEBUG("Value " + toString(value) + " sent to channel " + name);
}

void mqCsoundWrapper::sendMessage(const char* message) const
{
    MQ_LOG_DEBUG("Sending message to Csound:\n" + mq_str(message));
	csoundInputMessage(_state.csound , message);
}

void mqCsoundWrapper::sendScoreEvent(const char type, MYFLT* parameters, S32 numParameters)
{
    S32 result = csoundScoreEvent(_state.csound, type, parameters, numParameters);
    
    if(result == CSOUND_SUCCESS)
    {
        MQ_LOG_DEBUG("Sent score event");
    }
    else
    {
        sanityCheck("csoundScoreEvent", result);
    }
}

const S32 mqCsoundWrapper::getSampleRate() const
{
    return roundToInt(csoundGetSr(_state.csound));
}

const S32 mqCsoundWrapper::getControlRate() const
{
    return csoundGetKr(_state.csound);
}

const S32 mqCsoundWrapper::getNumberOfControlSamples() const
{
    return csoundGetKsmps(_state.csound);
}

const F32 mqCsoundWrapper::getControlPeriodDuration() const
{
    return _ksmpsDuration;
}

void mqCsoundWrapper::createSampleTable(mqSampleTable* const table)
{
    mq_str message = "f " + toString<S32>(table->number) + " 0 0 ";
    message += toString<S32>(table->GENRoutine) + " \"" + table->filcod + "\" ";
    message += toString<F32>(table->skiptime) + " ";
    message += toString<S32>(table->format) + " ";
    message += toString<S32>(table->channel);
	sendMessage(message.c_str());
    
    while (!doesTableExist(table->number));
}

void mqCsoundWrapper::createImmediateTable(mqImmediateTable* const table)
{
    USize numTables = table->tableNums.size();
    mq_str message = "f " + toString<S32>(table->number) + " 0 ";
    message += toString<S32>(table->size)  + " -2";

    for (USize i = 0; i < numTables; i++)
    {
        message += " " + toString<S32>(table->tableNums[i]);
    }
        
	sendMessage(message.c_str());
    
    while (!doesTableExist(table->number)); // TODO: replace with safer solution
}

void mqCsoundWrapper::createSegmentTable(mqSegmentTable* const table)
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
    mq_str message = "f " + toString<S32>(table->number);
    message += " 0 " + toString<S32>(table->size) + " -7";
    
    for (S32 i = 0; i < numSegments; i++)
    {
        message += " " + toString<F32>(table->segments.at(i).value);
        message += " " + toString<F32>(table->segments.at(i).length);
    }
	
	sendMessage(message.c_str());
    
    while (!doesTableExist(table->number));
}

const S32 mqCsoundWrapper::getTableData(const S32 tableNumber, F32List* const data)
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
    
    MQ_LOG_DEBUG("Retrieving data for table " + toString(tableNumber));

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
        MQ_LOG_ERROR("Could not retrieve data for table " + toString(tableNumber) + ". Table does not exist.")
    }
#endif
    
    _state.yieldPerformance = false;
    
    return length;
}

void mqCsoundWrapper::setTableData(const S32 table, const F32List* const data)
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

const F32 mqCsoundWrapper::getTableValue(const S32 tableNumber, const S32 index)
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

bool mqCsoundWrapper::doesTableExist(S32 tableNumber)
{
    bool exists = false;
    
    if (tableNumber == 0)
    {
        MQ_LOG_ERROR("Could not retrieve data. Table is undefined.")
        
        return exists;
    }

    MQ_LOG_DEBUG("Checking if table " + toString(tableNumber) + " exists.")

    S32 length = -1;
    MYFLT* tablePtr = NULL;    
    _state.yieldPerformance = true;
    
    while (!_state.performanceThreadYield);
    
    length = csoundGetTable(_state.csound, &tablePtr, tableNumber);
    _state.yieldPerformance = false;
    exists = length > 0 && tablePtr != NULL;

    if (exists)
    {
        MQ_LOG_DEBUG("Table " + toString(tableNumber) + " exists.")
    }
    else
    {
        MQ_LOG_DEBUG("Table " + toString(tableNumber) + " does not exist.")
    }
    
    return exists;
}

void mqCsoundWrapper::deleteTable(const S32 tableNum)
{
    if (tableNum == TABLE_UNDEFINED)
    {
        MQ_LOG_DEBUG("Table is undefined.")
        
        return;
    }
    
    if (doesTableExist(tableNum)) {
        MQ_LOG_DEBUG("Deleting table " + toString(tableNum))
        
        mq_str message = "f -" + toString<S32>(tableNum) + " 0";
        sendMessage(message.c_str());
    }
    else
    {
        MQ_LOG_DEBUG("Table " + toString(tableNum) + " does not exist.")
    }
}

bool mqCsoundWrapper::isPerformanceThreadRunning() const
{
    return _performanceThreadRunning;
}

void mqCsoundWrapper::isPerformanceThreadRunning(bool running)
{
    _performanceThreadRunning = running;
}
