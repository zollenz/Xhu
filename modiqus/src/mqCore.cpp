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

#include <iomanip>
#include "mq_debug.h"
#include "mq_utils_math.h"
#include "mqCore.h"

using namespace mq;

S32 dbgLevel;

mqSoundParam::mqSoundParam() :
    type(SOUND_PARAM_UNDEFINED),
    min(0.0f),
    max(0.0f),
    defaultVal(0.0f) {}

void mqCore::Start(S32 mode)
{
    if (!_wrapper.Start()) {
        MQ_LOG_FATAL("Modiqus engine failed initialization")
        Stop();
        exit(EXIT_FAILURE);
    } else {
        MQ_LOG_INFO( "Modiqus engine initialized")
    }

    _mode = mode;
    _config.baseTableNumber = TABLE_BASE_OFFSET;
    _nextTableNumber = TABLE_BASE_OFFSET;
    
//    for (S32 i = 0; i < MAX_INSTANCES; i++) {
//        for (S32 j = NOTE_AMPLITUDE; j < SOUND_PARAM_UNDEFINED; j++) {
//            _channelValues[i][j] = 1.0f;
//            mq_str channelName = "i." + getInstanceString(INSTR_PARTIKKEL, i) + "." + SoundParamNames[j];
//            _wrapper.setChannelControlInput(1.0f, channelName.c_str());
//        }
//    }
    
    soundParams[NOTE_DURATION].type = NOTE_DURATION;
    soundParams[NOTE_DURATION].min = 0.0f;
    soundParams[NOTE_DURATION].max = 2.0f;
    soundParams[NOTE_DURATION].defaultVal = 1.0f;
    
    soundParams[NOTE_AMPLITUDE].type = NOTE_AMPLITUDE;
    soundParams[NOTE_AMPLITUDE].min = 0.0f;
    soundParams[NOTE_AMPLITUDE].max = 1.0f;
    soundParams[NOTE_AMPLITUDE].defaultVal = 0.5f;
    
    soundParams[GRAIN_DENSITY].type = GRAIN_DENSITY;
    soundParams[GRAIN_DENSITY].min = 0.0f;
    soundParams[GRAIN_DENSITY].max = 1.0f;
    soundParams[GRAIN_DENSITY].defaultVal = 0.5f;
    
    soundParams[GRAIN_SPATIAL_POSITION].type = GRAIN_SPATIAL_POSITION;
    soundParams[GRAIN_SPATIAL_POSITION].min = 0.0f;
    soundParams[GRAIN_SPATIAL_POSITION].max = 1.0f;
    soundParams[GRAIN_SPATIAL_POSITION].defaultVal = 0.5f;
}

void mqCore::Stop()
{
    if (IsRunning()) {
        _wrapper.Stop();
        while(IsRunning());
        MQ_LOG_INFO( "Modiqus engine terminated")
    }
}

const bool mqCore::IsRunning() const
{
    return _wrapper.IsPerformanceThreadRunning();
}

void mqCore::PlaySound(mqSoundInfo* const info)
{
    info->soundInstance = GetNewInstanceNumber();
    mq_str soundName = info->sourceName + "." + info->sourceEvent;
    mqSound* sound = GetSound(soundName);
    
    if (sound == NULL) {
        return;
    }
    
    if (sound->grainWaveTable.number == TABLE_UNDEFINED) {
        MQ_LOG_INFO( "Sound has no wave table.")
        return;
    }
    
    info->soundInstanceString = GetInstanceString(INSTR_PARTIKKEL, info->soundInstance);
    info->soundCompleteName = soundName + "." + info->soundInstanceString;
    // Send score event
    F32 value = 0.0f;
    mq_str message = "i ";
    message += info->soundInstanceString + " 0 ";
    value = GetMappedValue(sound, NOTE_DURATION);
    message += toString(value) + " ";
    message += toString(sound->grainWaveTable.number) + " ";
    message += "\"" + info->soundCompleteName + "\"";
    value = GetMappedValue(sound, NOTE_AMPLITUDE);
    message += " " + toString(value);
    value = GetMappedValue(sound, GRAIN_DENSITY);
    message += " " + toString(value);
    message += " " + toString<F32>(sound->grainStart);
    message += " " + toString<S32>(sound->grainDuration);
    value = GetMappedValue(sound, GRAIN_SPATIAL_POSITION);
    message += " " + toString(value);
    _wrapper.SendMessage(message.c_str());
}

void mqCore::StopSound(mqSoundInfo* const info)
{
    mq_str message = "i 1 0 " + toString(_wrapper.GetControlPeriodDuration());
    message += " " + info->soundInstanceString;
    _wrapper.SendMessage(message.c_str());
    info->soundInstance = UNDEFINED_INT;
    info->soundInstanceString = UNDEFINED_STR;
    info->soundCompleteName = UNDEFINED_STR;
}

void mqCore::UpdateControlParam(const mqParamUpdate& update)
{
    mqControlParam* controlParam = mapGet(update.name, _config.controlParams);
    
    if (controlParam == NULL) {
        MQ_LOG_ERROR("Could not find game parameter '" + update.name + "' in config")
    } else {
        clamp(update.value, controlParam->min, controlParam->max);
        controlParam->value = update.value;
    }
}

void mqCore::UpdateControlParams(const std::vector<mqParamUpdate>& updates)
{
    USize numParams = updates.size();
    
    for (USize i = 0; i < numParams; i++) {
        UpdateControlParam(updates[i]);
    }
}

void mqCore::SetSoundParam(const SoundParamType param, F32 value, const mqSoundInfo& info)
{
    mq_str channelName = info.soundCompleteName + "." + SoundParamNames[soundParams[param].type];
    value = clamp(value, soundParams[param].min, soundParams[param].max);
    _wrapper.SetChannelControlInput(value, channelName.c_str());
}

void mqCore::ClearConfig()
{
    mqSoundMap::iterator soundIt = _config.sounds.begin();
    
    while (soundIt != _config.sounds.end()) {
        if (soundIt->second.grainWaveTable.number != TABLE_UNDEFINED) {
            _wrapper.DeleteTable(soundIt->second.grainWaveTable.number);
        }
        
        for (S32 i = 0; i < SOUND_PARAM_UNDEFINED; i++) {
            mqMapping* mapping = &soundIt->second.mappings[i];
            ResetMapping(mapping);
        }
        
        soundIt->second.Reset();
        ++soundIt;
    }
    
    _config.Reset();
    _nextInstance = INDEX_INVALID;
    _config.baseTableNumber = TABLE_BASE_OFFSET;
    _nextTableNumber = TABLE_BASE_OFFSET;
    
    MQ_LOG_INFO( "Modiqus configuration cleared");
}

//bool mqCore::loadConfig(const mq_str& filename)
//{
//    _config.reset();
//    bool success = parseConfig(filename, _config);
//    
//    if (success) {
//        mqSoundMap::iterator soundIt = _config.sounds.begin();
//        F32List dummyData;
//        
//        while (soundIt != _config.sounds.end()) {
//            if (soundIt->second.grainWaveTable.number == TABLE_UNDEFINED) {
//                MQ_LOG_ERROR("Wave table for sound '" + soundIt->second.name + "' is undefined.")
//            } else {
//                if (!_wrapper.tableExists(soundIt->second.grainWaveTable.number)) {
//                    createSampleTable(soundIt->second.grainWaveTable, &dummyData);
//                }
//            }
//            
//            for (S32 i = 0; i < SOUND_PARAM_UNDEFINED; i++) {
//                mqMapping& mapping = soundIt->second.mappings[i];
//                
//                if (mapping.controlParam != NULL) {
//                    mapping.controlParam->value = mapping.controlParam->min + 0.5f * (mapping.controlParam->max - mapping.controlParam->min);
//                }
//                
//                if (mapping.morphMinTable.number != TABLE_UNDEFINED) {
//                    createLinSegTable(mapping.morphMinTable);
//                }
//                
//                if (mapping.morphMinTableTable.number != TABLE_UNDEFINED) {
//                    createImmediateTable(mapping.morphMinTableTable);
//                }
//                
//                if (mapping.morphMaxTable.number != TABLE_UNDEFINED) {
//                    createLinSegTable(mapping.morphMaxTable);
//                }
//                
//                if (mapping.morphMaxTableTable.number != TABLE_UNDEFINED) {
//                    createImmediateTable(mapping.morphMaxTableTable);
//                }
//                
//                if (mapping.morphIntraTable.number != TABLE_UNDEFINED) {
//                    createLinSegTable(mapping.morphIntraTable);
//                }
//                
//                if (mapping.morphIntraTableTable.number != TABLE_UNDEFINED) {
//                    createImmediateTable(mapping.morphIntraTableTable);
//                }
//                
//                USize modifierCount = mapping.modifiers.size();
//                
//                for (USize j = 0; j < modifierCount; j++) {
//                    mqModifier& modifier = mapping.modifiers[j];
//                    
//                    if (modifier.minTable.number != TABLE_UNDEFINED) {
//                        createLinSegTable(modifier.minTable);
//                    }
//                    
//                    if (modifier.maxTable.number != TABLE_UNDEFINED) {
//                        createLinSegTable(modifier.maxTable);
//                    }
//                }
//                
//                // TODO: check why it's necessary to do this one time before accessing table data is reliable (threading!!)
//                morphTables(mapping);
//                getLinSegTableData(mapping.morphMinTable, &dummyData);
//                getLinSegTableData(mapping.morphMaxTable, &dummyData);
//                getLinSegTableData(mapping.morphIntraTable, &dummyData);
//            }
//            
//            ++soundIt;
//        }
//        
//        _nextInstance = INDEX_INVALID;
//        _nextTableNumber = _config.baseTableNumber;
//        MQ_LOG_INFO( "Modiqus configuration '" + filename + "' loaded")
//        MQ_LOG_INFO( "Base table number is: " + toString(_config.baseTableNumber))
//        
//        return true;
//    }
//    
//    return false;
//}

mqSound* const mqCore::GetSound(const mq_str& name)
{
    mqSound* sound = mapGet(name, _config.sounds);
    
    if (sound == NULL) {
        MQ_LOG_ERROR("Invalid sound '" + name + "'");
    }
    
    return sound;
}

void mqCore::CreateSampleTable(mqSampleTable& table, F32List* const samples)
{
    if (_wrapper.DoesTableExist(table.number)) {
        MQ_LOG_WARN("Table " + toString(table.number) + " already exists.")
        return;
    }
    
    if (table.number == TABLE_UNDEFINED) {
        table.number = GetNewTableNumber();
    }
    
    _wrapper.CreateSampleTable(table);
    table.size = _wrapper.GetTableData(table.number, samples);
    UpdateBaseTableNumber(table.number);
}

void mqCore::CreateImmediateTable(mqImmediateTable& table)
{
    if (table.number == TABLE_UNDEFINED) {
        table.number = GetNewTableNumber();
    }
    
    _wrapper.CreateImmediateTable(table);
    UpdateBaseTableNumber(table.number);
}

void mqCore::CreateSegmentTable(mqSegmentTable* const table)
{
    if (table->number == TABLE_UNDEFINED) {
        table->number = GetNewTableNumber();
    }
    
    _wrapper.CreateSegmentTable(table);
    UpdateBaseTableNumber(table->number);
}

void mqCore::GetSampleTableData(mqSampleTable& table, F32List* data)
{
    table.size = _wrapper.GetTableData(table.number, data);
}

void mqCore::GetLinSegTableData(const mqSegmentTable& table, F32List* data)
{
    _wrapper.GetTableData(table.number, data);
}

void mqCore::StartInstanceMonitor(InstrumentType instr, bool oneshot) const
{
    mq_str playInstr = toString<S32>(instr);
    mq_str monInstr = "";
    
    if (oneshot == true) {
        monInstr = toString<S32>(INSTR_MONITOR_I);
        mq_str msg = mq_str("i ") + monInstr + mq_str(" 0 0 ") + playInstr;
        _wrapper.SendMessage(msg.c_str());
    } else {
        monInstr = toString<S32>(INSTR_MONITOR_K);
        mq_str msg = mq_str("i ") + monInstr + mq_str(" 0 -1 ") + playInstr;
        _wrapper.SendMessage(msg.c_str());
    }
}

void mqCore::StopInstanceMonitor(InstrumentType instr, bool oneshot) const
{
    S32 playInstr = instr;
    S32 monInstr = INSTR_UNDEFINED;
    
    if (oneshot == true) {
        monInstr = INSTR_MONITOR_I;
    } else {
        monInstr = INSTR_MONITOR_K;
    }
    
    mq_str message = mq_str("i -") + toString<S32>(monInstr) + mq_str(" 0 0 ") + toString<S32>(playInstr);
    _wrapper.SendMessage(message.c_str());
}

void mqCore::ResetMapping(mqMapping* const mapping)
{
    if (mapping != NULL) {
        if (mapping->type < mqMapping::UNDEFINED) {
            _wrapper.DeleteTable(mapping->morphMinTable.number);
            _wrapper.DeleteTable(mapping->morphMinTableTable.number);
        }
        
        if (mapping->type > mqMapping::SEGMENT) {
            _wrapper.DeleteTable(mapping->morphMaxTable.number);
            _wrapper.DeleteTable(mapping->morphMaxTableTable.number);
            _wrapper.DeleteTable(mapping->morphIntraTable.number);
            _wrapper.DeleteTable(mapping->morphIntraTableTable.number);
        }
        
        USize numModifiers = mapping->modifiers.size();
        
        for (USize i = 0; i < numModifiers; i++) {
            if (mapping->type < mqMapping::UNDEFINED) {
                _wrapper.DeleteTable(mapping->modifiers[i].minTable.number);
            }
            
            if (mapping->type > mqMapping::SEGMENT) {
                _wrapper.DeleteTable(mapping->modifiers[i].maxTable.number);
            }
        }
        
        mapping->Reset();
    } else {
        MQ_LOG_WARN("Mapping is NULL. Could not reset.")
    }
}

const F32 mqCore::GetMappedValue(mqSound* const sound, const SoundParamType soundParamType)
{
    F32 value = soundParams[soundParamType].defaultVal;
    mqMapping& mapping = sound->mappings[soundParamType];
    
    if (mapping.controlParam != NULL) {
        MorphTables(mapping);
        mqMapping::Type type = mapping.type;
        
        if (type == mqMapping::CONSTANT || type == mqMapping::SEGMENT) {
            value = mapping.morphMinTable.number;
        } else if (type ==  mqMapping::RANGE || type ==  mqMapping::MASK) {
            value = mapping.morphIntraTable.number;
        }
        
        if (soundParamType == NOTE_DURATION) {
            F32List inData(TABLE_SIZE_MEDIUM, 0.0f);
            _wrapper.GetTableData((S32)value, &inData);
            value = inData.at(0);
        }        
    }
    
    return value;
}

void mqCore::MorphTables(const mqMapping& mapping)
{
    const F32 morphIndex = GetMorphTableListIndex(mapping);
    
    if (mapping.type < mqMapping::UNDEFINED) {
        const S32 morphMinTable = mapping.morphMinTable.number;
        const S32 morphMinTableTable = mapping.morphMinTableTable.number;

#ifdef DEBUG
        if (morphMinTable == TABLE_UNDEFINED || !_wrapper.DoesTableExist(morphMinTable)) {
            MQ_LOG_ERROR("Morph min table undefined or does not exist.")
            return;
        }
        
        if (morphMinTableTable == TABLE_UNDEFINED || !_wrapper.DoesTableExist(morphMinTableTable)) {
            MQ_LOG_ERROR("Morph min table table undefined or does not exist.")
            return;
        }
#endif
        MorphTable(morphIndex, morphMinTable, morphMinTableTable);
    }
    
    if (mapping.type > mqMapping::SEGMENT) {
        const S32 morphMaxTable = mapping.morphMaxTable.number;
        const S32 morphMaxTableTable = mapping.morphMaxTableTable.number;
        
#ifdef DEBUG
        if (morphMaxTable == TABLE_UNDEFINED || !_wrapper.DoesTableExist(morphMaxTable)) {
            MQ_LOG_ERROR("Morph max table undefined or does not exist.")
            return;
        }
        
        if (morphMaxTableTable == TABLE_UNDEFINED || !_wrapper.DoesTableExist(morphMaxTableTable)) {
            MQ_LOG_ERROR("Morph max table table undefined or does not exist.")
            return;
        }
#endif
        MorphTable(morphIndex, morphMaxTable, morphMaxTableTable);
        
        const S32 morphIntraTable = mapping.morphIntraTable.number;
        const S32 morphIntraTableTable = mapping.morphIntraTableTable.number;
        
#ifdef DEBUG
        if (morphIntraTable == TABLE_UNDEFINED || !_wrapper.DoesTableExist(morphIntraTable)) {
            MQ_LOG_ERROR("Morph intra table undefined or does not exist.")
            return;
        }
        
        if (morphIntraTableTable == TABLE_UNDEFINED || !_wrapper.DoesTableExist(morphIntraTableTable)) {
            MQ_LOG_ERROR("Morph intra table table undefined or does not exist.")
            return;
        }
#endif
        const F32 morphIntraIndex = random01();
        MorphTable(morphIntraIndex, morphIntraTable, morphIntraTableTable);
    }
}

void mqCore::MorphTable(const F32 morphIndex, const S32 morphTable, const S32 morphTableTable) const
{
    mq_str message = "i " + toString<InstrumentType>(INSTR_TABLE_MORPH);
    message += " 0 " + toString(_wrapper.GetControlPeriodDuration()) + " ";
    message += toString<F32>(morphIndex) + " ";
    message += toString<S32>(morphTableTable) + " ";
    message += toString<S32>(morphTable);
    _wrapper.SendMessage(message.c_str());
}

const F32 mqCore::GetMorphTableListIndex(const mqMapping& mapping) const
{
    USize numLinks = mapping.modifiers.size();
    S32 index = INDEX_INVALID;
    
    for (S32 i = 0; i < numLinks; i++) {
        if (mapping.controlParam->value >= mapping.modifiers[i].controlValue) {
            index = i;
        }
    }
    
    if (index == INDEX_INVALID) {
        MQ_LOG_DEBUG("Could not find morph table table index.")
        return 0.0f;
    }
    
    F32 lerpValue = index;
    
    if (index < numLinks - 1) {
        F32 relValue = mapping.controlParam->value - mapping.modifiers[index].controlValue;
        F32 delta = mapping.modifiers[index + 1].controlValue - mapping.modifiers[index].controlValue;
        lerpValue += relValue / delta;
    }
    
    return lerpValue;
}

const F32 mqCore::InterpolateSoundParam(const mqSoundParam& soundParam, const mqMapping& mapping) const
{
    if (mapping.controlParam == NULL) {
        MQ_LOG_DEBUG("Sound param '" + SoundParamNames[soundParam.type] + "' has no mapping. Using default param value.")
        return soundParam.defaultVal;
    }
    
    S32 tableNum = mapping.modifiers[0].minTable.number;
    
    if (tableNum == TABLE_UNDEFINED) {
        MQ_LOG_ERROR("Segment table " + toString<S32>(tableNum) + " not defined. Using default param value.")
        return soundParam.defaultVal;
    }
    
    // TODO: convert to COORDS for runtime representation
    const mqSegmentTable& table = mapping.modifiers[0].minTable;
    mqControlParam* controlParam = mapping.controlParam;
    F32 rangeX = controlParam->max - controlParam->min;
    F32 lowerX = 0.0f;
    F32 lowerY = 0.0f;
    F32 upperX = 0.0f;
    F32 upperY = 0.0f;
    USize numSegments = table.segments.size();
    
    for (USize i = 1; i < numSegments; i++)
    {
        lowerX = upperX;
        lowerY = table.segments[i - 1].value;
        upperX += table.segments[i - 1].length / table.size * rangeX + controlParam->min;
        upperY = table.segments[i].value;
        
        if (upperX > controlParam->value) {
            break;
        }
    }
    
    return lerp(lowerX, lowerY, upperX, upperY, controlParam->value);
}

const mq_str mqCore::GetInstanceString(InstrumentType instrument, const S32 instance)
{
    std::ostringstream stream;
    stream << std::setfill('0') << std::setw(6) << instance;
    return toString<S32>(instrument) + "." + stream.str();
}

const U32 mqCore::GetNewInstanceNumber()
{
    _nextInstance++;
    
    if (_nextInstance % MAX_INSTANCES == 0) {
        _nextInstance = 0;
    }
    
    return _nextInstance;
}

void mqCore::GetMonitorResult(F32& value) const
{
    _wrapper.GetChannelControlOutput(value, "InstanceMonitor");
}

const U32 mqCore::GetNewTableNumber()
{
    _nextTableNumber++;
    
    if (_nextTableNumber == UINT_MAX) {
        _nextTableNumber = 0;
    }
    
    return _nextTableNumber;
}

void mqCore::UpdateBaseTableNumber(const U32 number)
{
    if (number <= _nextTableNumber) {
        _config.baseTableNumber = _nextTableNumber;
    } else {
        _nextTableNumber = number;
        _config.baseTableNumber = number;
    }
}

void mqCore::SendMessage(const mq_str& msg) const
{
    _wrapper.SendMessage(msg.c_str());
}

mqCsoundWrapper* const mqCore::GetCsoundWrapper()
{
    return &_wrapper;
}