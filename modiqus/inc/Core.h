/*
 * Copyright (C) 2013 by Martin Dejean
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

#ifndef __CORE_H__
#define __CORE_H__

#include "CsoundWrapper.h"
#include "Configuration.h"
#include "core_events.h"
#include "thread.h"
#include "debug.h"

namespace modiqus
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
    
    static const String SoundParamNames[SOUND_PARAM_UNDEFINED] =
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
    
    class Core
    {
    public:
        friend class mqEditor;

        Core() {}
        
        ~Core() {}
        
        /// Start the Modiqus audio engine
        void start(S32 mode);

        /// Stop the Modiqus audio engine
        void stop();

        /// Returns whether the engine is running
        const bool isRunning() const;
                
        void playSound(mqSoundInfo* const info);
        
        void stopSound(mqSoundInfo* const info);
        
        void updateControlParam(const mqParamUpdate& update);
        
        void updateControlParams(const std::vector<mqParamUpdate>& update);
        
        void setSoundParam(const SoundParamType channel, const F32 value, const mqSoundInfo& info);
        
        void SendMessage(const String& msg) const; // FOR DEBUGGING, TODO: DELETE IN FINAL BUILD
        
        /// Clears the current configuration
        void clearConfig();
        
        /// Loads a configuration from disk
        virtual bool loadConfig(const String& filename);
        
        mqSound* const getSound(const String& name);
                
    protected:
        
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
        
        class OutputThread : Thread
        {
        public:
            bool StartInternalThread()
            {
                return Thread::StartInternalThread();
            }
            
            void JoinInternalThread()
            {
                Thread::JoinInternalThread();
            }
            
            void injectAudio(const Core* const audio) { _audio = audio; };
            
        private:
            
            const Core* _audio;
            
            void InternalThreadEntry()
            {
                F32 sampleData[KSMPS];
                F32 value = -1;
                
                MQ_LOG(LOG_INFO, "OutputThread started\n");
                
                _audio->startInstanceMonitor(INSTR_PARTIKKEL, false);
                
                while (value != 1) {
                    //                    AudioEngine.StartInstanceMonitor(INSTR_PARTIKKEL, true);
                    _audio->getMonitorResult(value);
                }
                
                while (value != 0) {
                    //                    AudioEngine.StartInstanceMonitor(INSTR_PARTIKKEL, true);
                    _audio->getMonitorResult(value);
                    _audio->getMainOutput(sampleData);
                    
                    AudioEvents.OutputDataReady.FireEvent(&sampleData[0]);
                }
                
                _audio->stopInstanceMonitor(INSTR_PARTIKKEL, false);
                //                CoreEvents::Instance().OutputSilent.FireEvent("OutputSilent");
                MQ_LOG(LOG_INFO, "OutputThread stopped\n");
            }
        };
        
        CsoundWrapper _wrapper;
        Configuration _config;
        
        mqSoundParam soundParams[SOUND_PARAM_UNDEFINED];
        F32 _channelValues[MAX_INSTANCES][SOUND_PARAM_UNDEFINED];
        
        S32 _mode;
        U32 _nextInstance;
        U32 _nextTableNumber;
        OutputThread* _outputThread;
        
        void createSampleTable(mqSampleTable& table, F32List* const samples = NULL);
        
        void createImmediateTable(mqImmediateTable& table);
        
        void createLinSegTable(mqSegmentTable& table);
        
        void getSampleTableData(mqSampleTable& table, F32List* data);
        
        void getLinSegTableData(const mqSegmentTable& table, F32List* data);
        
        void startInstanceMonitor(InstrumentType instr, bool oneshot = true) const;
        
        void stopInstanceMonitor(InstrumentType instr, bool oneshot = true) const;
        
        void resetMapping(mqMapping* const mapping);
        
        const F32 getMappedValue(mqSound* const sound, const SoundParamType soundParamType);
        
        void morphTables(const mqMapping& mapping);
        
        void morphTable(const F32 morphIndex, const S32 morphTable, const S32 morphTableTable) const;
        
        const F32 getMorphTableListIndex(const mqMapping& mapping) const;
        
        const F32 interpolateSoundParam(const mqSoundParam& soundParam, const mqMapping& mapping) const;
        
        void getMainOutput(F32* data) const;
        
        void getMonitorResult(F32& value) const;
        
        const String getInstanceString(InstrumentType instrument, const S32 instance);
        
        const U32 getNewInstanceNumber();
        
        const U32 getNewTableNumber();
        
        void updateBaseTableNumber(const U32 number);
        
        void createChannelMaskTable(const S32 rawDataTableNumber);
    };
}

#endif //__CORE_H__
