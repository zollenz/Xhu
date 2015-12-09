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

#ifndef __MQ_CORE_H__
#define __MQ_CORE_H__

#include "mq_core_events.h"
#include "mq_thread.h"
#include "mq_debug.h"
#include "mqCsoundWrapper.h"
#include "mqConfiguration.h"

namespace mq
{   
    static const S32 ENGINE_MODE_EDITOR = 0;
    static const S32 ENGINE_MODE_GAME = 1;
    static const S32 TABLE_BASE_OFFSET = 99;
    static const S32 MAX_INSTANCES = 1000;
    
    enum SoundParamType
    {
        NOTE_DURATION,   // I-rate
        NOTE_AMPLITUDE,  // k-rate
        GRAIN_DENSITY,    // k-rate
        GRAIN_SPATIAL_POSITION,
        SOUND_PARAM_UNDEFINED
    };
    
    static const mq_str SoundParamNames[SOUND_PARAM_UNDEFINED] =
    {
        "NoteDuration",
        "NoteAmplitude",
        "GrainDensity",
        "GrainSpatialPosition"        
    };
    
    struct mqSoundParam
    {
        SoundParamType type;
        F32 min;
        F32 max;
        F32 defaultVal;
        
        mqSoundParam();
    };
    
    class mqCore
    {
    public:
        mqCore() {}
        ~mqCore() {}
        void Start(S32 mode);
        void Stop();
        const bool IsRunning() const;
        void PlaySound(mqSoundInfo* const info);
        void StopSound(mqSoundInfo* const info);
        void UpdateControlParam(const mqParamUpdate& update);
        void UpdateControlParams(const std::vector<mqParamUpdate>& update);
        void SetSoundParam(const SoundParamType channel, const F32 value, const mqSoundInfo& info);
        void ClearConfig();
//        virtual bool loadConfig(const mq_str& filename);
        mqSound* const GetSound(const mq_str& name);
        void CreateSampleTable(mqSampleTable& table, F32List* const samples = NULL);
        void CreateImmediateTable(mqImmediateTable& table);
        void CreateSegmentTable(mqSegmentTable& table);

#ifdef DEBUG
        void SendMessage(const mq_str& msg) const; // FOR DEBUGGING, TODO: DELETE IN FINAL BUILD
        mqCsoundWrapper* GetCsoundWrapper();
#endif
    private:
        enum InstrumentType
        {
            INSTR_UNDEFINED,
            INSTR_TURNOFF,
            INSTR_PARTIKKEL,
            INSTR_SAMPLE_PLAYER,
            INSTR_MONITOR_I,
            INSTR_MONITOR_K,
            INSTR_TABLE_MORPH
        };
        
        class OutputThread : mqThread
        {
        public:
            bool StartInternalThread()
            {
                return mqThread::StartInternalThread();
            }
            
            void JoinInternalThread()
            {
                mqThread::JoinInternalThread();
            }
            
            void injectAudio(const mqCore* const audio) { _audio = audio; };
            
        private:
            
            const mqCore* _audio;
            
            void InternalThreadEntry()
            {
                F32 sampleData[KSMPS];
                F32 value = -1;
                
                MQ_LOG(MQ_LOG_INFO, "OutputThread started\n");
                
                _audio->StartInstanceMonitor(INSTR_PARTIKKEL, false);
                
                while (value != 1) {
                    //                    AudioEngine.StartInstanceMonitor(INSTR_PARTIKKEL, true);
                    _audio->GetMonitorResult(value);
                }
                
                while (value != 0) {
                    //                    AudioEngine.StartInstanceMonitor(INSTR_PARTIKKEL, true);
                    _audio->GetMonitorResult(value);
                    _audio->GetMainOutput(sampleData);
                    
                    AudioEvents.OutputDataReady.FireEvent(&sampleData[0]);
                }
                
                _audio->StopInstanceMonitor(INSTR_PARTIKKEL, false);
                //                CoreEvents::Instance().OutputSilent.FireEvent("OutputSilent");
                MQ_LOG(MQ_LOG_INFO, "OutputThread stopped\n");
            }
        };
        
        mqCsoundWrapper _wrapper;
        mqConfiguration _config;
        mqSoundParam soundParams[SOUND_PARAM_UNDEFINED];
        F32 _channelValues[MAX_INSTANCES][SOUND_PARAM_UNDEFINED];
        S32 _mode;
        U32 _nextInstance;
        U32 _nextTableNumber;
        OutputThread* _outputThread;
        
        void GetSampleTableData(mqSampleTable& table, F32List* data);
        void GetLinSegTableData(const mqSegmentTable& table, F32List* data);
        void StartInstanceMonitor(InstrumentType instr, bool oneshot = true) const;
        void StopInstanceMonitor(InstrumentType instr, bool oneshot = true) const;
        void ResetMapping(mqMapping* const mapping);
        const F32 GetMappedValue(mqSound* const sound, const SoundParamType soundParamType);
        void MorphTables(const mqMapping& mapping);
        void MorphTable(const F32 morphIndex, const S32 morphTable, const S32 morphTableTable) const;
        const F32 GetMorphTableListIndex(const mqMapping& mapping) const;
        const F32 InterpolateSoundParam(const mqSoundParam& soundParam, const mqMapping& mapping) const;
        const mq_str GetInstanceString(InstrumentType instrument, const S32 instance);
        const U32 GetNewInstanceNumber();
        const U32 GetNewTableNumber();
        void UpdateBaseTableNumber(const U32 number);
        void CreateChannelMaskTable(const S32 rawDataTableNumber);
        void GetMainOutput(F32* data) const;
        void GetMonitorResult(F32& value) const;
    };
}

#endif //__MQ_CORE_H__
