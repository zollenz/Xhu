///*
// * Copyright (C) 2018 by Martin Dejean
// *
// * This file is part of Modiqus.
// * Modiqus is free software: you can redistribute it and/or modify
// * it under the terms of the GNU General Public License as published by
// * the Free Software Foundation, either version 3 of the License, or
// * (at your option) any later version.
// *
// * Modiqus is distributed in the hope that it will be useful,
// * but WITHOUT ANY WARRANTY; without even the implied warranty of
// * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// * GNU General Public License for more details.
// *
// * You should have received a copy of the GNU General Public License
// * along with Modiqus.  If not, see <http://www.gnu.org/licenses/>.
// *
// */
//
//#ifndef __MQ_CORE_H__
//#define __MQ_CORE_H__
//
//#include "mq_core_events.h"
//#include "mq_thread.h"
//#include "mq_debug.h"
//#include "mqCsoundWrapper.h"
//#include "mqConfiguration.h"
//
//namespace mq
//{   
//    static const S32 ENGINE_MODE_EDITOR = 0;
//    static const S32 ENGINE_MODE_GAME = 1;
//    static const S32 TABLE_BASE_OFFSET = 99;
//    static const S32 MAX_INSTANCES = 1000;
//    
//    enum SoundParamType
//    {
//        NOTE_DURATION,   // I-rate
//        NOTE_AMPLITUDE,  // k-rate
//        GRAIN_DENSITY,    // k-rate
//        GRAIN_SPATIAL_POSITION,
//        SOUND_PARAM_UNDEFINED
//    };
//    
//    static const mq_str SoundParamNames[SOUND_PARAM_UNDEFINED] =
//    {
//        "NoteDuration",
//        "NoteAmplitude",
//        "GrainDensity",
//        "GrainSpatialPosition"        
//    };
//    
//    struct mqSoundParam
//    {
//        SoundParamType type;
//        F32 min;
//        F32 max;
//        F32 defaultVal;
//        
//        mqSoundParam();
//    };
//    
//    class mqCore
//    {
//        
//    public:
//        
//        mqCore() {}
//        ~mqCore() {}
//        void Start(S32 mode);
//        void Stop();
//        const bool isRunning() const;
//        mqCsoundWrapper* const getCsoundWrapper();
//        void sendMessage(const mq_str& msg) const;
//        void createSampleTable(mqSampleTable* const table, F32List* const samples = NULL);
//        void createImmediateTable(mqImmediateTable* const table);
//        void createSegmentTable(mqSegmentTable* const table);
//        // TODO FUNCS
//        void playSound(mqSoundInfo* const info);
//        void stopSound(mqSoundInfo* const info);
//        void updateControlParam(const mqParamUpdate& update);
//        void updateControlParams(const std::vector<mqParamUpdate>& update);
//        void setSoundParam(const SoundParamType channel, const F32 value, const mqSoundInfo& info);
//        void clearConfig();
////        virtual bool loadConfig(const mq_str& filename);
//        mqSound* const getSound(const mq_str& name);
//        
//    private:
//        
//        enum InstrumentType
//        {
//            INSTR_UNDEFINED,
//            INSTR_TURNOFF,
//            INSTR_PARTIKKEL,
//            INSTR_SAMPLE_PLAYER,
//            INSTR_MONITOR_I,
//            INSTR_MONITOR_K,
//            INSTR_TABLE_MORPH
//        };
//        
//        class OutputThread : mqThread
//        {
//            
//        public:
//            
//            void injectAudio(const mqCore* const audio) { _audio = audio; };
//            
//        private:
//            
//            const mqCore* _audio;
//            
//            void internalThreadEntry()
//            {
//                F32 sampleData[KSMPS];
//                F32 value = -1;
//                
//                MQ_LOG_INFO( "OutputThread started\n");
//                
//                _audio->startInstanceMonitor(INSTR_PARTIKKEL, false);
//                
//                while (value != 1)
//                {
//                    //                    AudioEngine.StartInstanceMonitor(INSTR_PARTIKKEL, true);
//                    _audio->getMonitorResult(value);
//                }
//                
//                while (value != 0)
//                {
//                    //                    AudioEngine.StartInstanceMonitor(INSTR_PARTIKKEL, true);
//                    _audio->getMonitorResult(value);
//                    _audio->getMainOutput(sampleData);
//                    
//                    AudioEvents.outputDataReady.FireEvent(&sampleData[0]);
//                }
//                
//                _audio->stopInstanceMonitor(INSTR_PARTIKKEL, false);
//                //                CoreEvents::Instance().OutputSilent.FireEvent("OutputSilent");
//                MQ_LOG_INFO( "OutputThread stopped\n");
//            }
//            
//        };
//        
//        F32 _channelValues[MAX_INSTANCES][SOUND_PARAM_UNDEFINED];
//        S32 _mode;
//        
//        U32 _nextInstance;
//        U32 _nextTableNumber;
//        
//        mqCsoundWrapper _wrapper;
//        mqConfiguration _config;
//        mqSoundParam soundParams[SOUND_PARAM_UNDEFINED];
//
//        OutputThread* _outputThread;
//        
//        void getSampleTableData(mqSampleTable& table, F32List* data);
//        void getLinSegTableData(const mqSegmentTable& table, F32List* data);
//        void startInstanceMonitor(InstrumentType instr, bool oneshot = true) const;
//        void stopInstanceMonitor(InstrumentType instr, bool oneshot = true) const;
//        void resetMapping(mqMapping* const mapping);
//        const F32 getMappedValue(mqSound* const sound, const SoundParamType soundParamType);
//        void morphTables(const mqMapping& mapping);
//        void morphTable(const F32 morphIndex, const S32 morphTable, const S32 morphTableTable) const;
//        const F32 getMorphTableListIndex(const mqMapping& mapping) const;
//        const F32 interpolateSoundParam(const mqSoundParam& soundParam, const mqMapping& mapping) const;
//        const mq_str getInstanceString(InstrumentType instrument, const S32 instance);
//        const U32 getNewInstanceNumber();
//        const U32 getNewTableNumber();
//        void updateBaseTableNumber(const U32 number);
//        void createChannelMaskTable(const S32 rawDataTableNumber);
//        void getMainOutput(F32* data) const;
//        void getMonitorResult(F32& value) const;
//        
//    };
//}
//
//#endif //__MQ_CORE_H__
