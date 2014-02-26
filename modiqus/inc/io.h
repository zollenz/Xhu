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

#ifndef __IO_H__
#define __IO_H__

#include <fstream>
#include "pugixml.hpp"
#include "Configuration.h"
#include "utils.h"

#define xmlValue pugi::node_pcdata
typedef pugi::xml_node xmlNode;
typedef pugi::xml_document xmlDocument;

namespace modiqus
{
    static bool saveTextFile(String text, String fileName)
    {
        std::ofstream file;
        file.open(fileName.c_str());
        
        if (file.is_open()) {
            if (file.bad()) {
                perror(("error while reading file " + fileName).c_str());
                
                return false;
            }
            
            file << text << "\n";
            file.close();
        } else {
            perror(("error while opening file " + fileName).c_str());
            return false;
        }
        
        return true;
    }

    static String readTextFile(String fileName)
    {
        String fileContents = "";
        std::ifstream file;
        file.open(fileName.c_str());

        if (file) {
            if (file.bad()) {
                perror(("Error while reading file " + fileName).c_str());

                return "ERROR";
            }

            while (file) {
                getline(file, fileContents);
                std::cout << fileContents << std::endl;
            }
        } else {
            perror(("error while opening file " + fileName).c_str());

            return "ERROR";
        }

        return fileContents;
    }
    
    static void parseLinSegTable(mqSegmentTable& table, const xmlNode& node)
    {
        table.number = fromString<S32>(node.child("number").child_value());
        table.start = fromString<S32>(node.child("start").child_value());
        table.size = fromString<S32>(node.child("size").child_value());
        table.GENRoutine = fromString<S32>(node.child("gen_routine").child_value());
        xmlNode segmentsNode = node.child("segment_list");

        for (xmlNode nextNode = segmentsNode.child("segment"); nextNode; nextNode = nextNode.next_sibling("segment")) {
            F32 value = fromString<F32>(nextNode.child("value").child_value());
            F32 length = fromString<F32>(nextNode.child("length").child_value());
            table.segments.push_back(mqSegment(value, length));
        }        
    }
    
    static void parseImmediateTable(mqImmediateTable& table, const xmlNode& node)
    {
        String valueStr = node.child("number").child_value();
        table.number = fromString<S32>(valueStr);
        valueStr = node.child("start").child_value();
        table.start = fromString<S32>(valueStr);
        valueStr = node.child("size").child_value();
        table.size = fromString<S32>(valueStr);
        valueStr = node.child("gen_routine").child_value();
        table.GENRoutine = fromString<S32>(valueStr);
        xmlNode numberListNode = node.child("table_number_list");
        
        for (xmlNode nextNode = numberListNode.child("table_number");
             nextNode;
             nextNode = nextNode.next_sibling("table_number")) {
            valueStr = nextNode.child_value();
            table.tableNums.push_back(fromString<S32>(valueStr));
        }
    }

    static void parseModifier(mqModifier& modifier, const xmlNode& parentNode)
    {
        String valueStr = parentNode.child("control_param_value").child_value();
        modifier.controlValue = fromString<F32>(valueStr);
        parseLinSegTable(modifier.minTable, parentNode.child("min_table"));
        parseLinSegTable(modifier.maxTable, parentNode.child("max_table"));
    }

    static void parseMapping(mqMapping& mapping, Configuration& configuration, const xmlNode& mappingNode)
    {
        String valueStr = mappingNode.child("type").child_value();
        S32 index = findListIndex(valueStr, mqMapping::TypeNames, mqMapping::TYPE_COUNT);
        mapping.type = mqMapping::Type(index);
        
        valueStr = mappingNode.child("control_param").child_value();
        
        if (valueStr.compare(UNDEFINED_STR) == 0) {
            mapping.controlParam = NULL;
        } else {
            mapping.controlParam = mapGet(valueStr, configuration.controlParams);
        }

        xmlNode minTableNode = mappingNode.child("morph_min_table");
        parseLinSegTable(mapping.morphMinTable, minTableNode);
        
        xmlNode maxTableNode = mappingNode.child("morph_max_table");
        parseLinSegTable(mapping.morphMaxTable, maxTableNode);
        
        xmlNode intraTableNode = mappingNode.child("morph_intra_table");
        parseLinSegTable(mapping.morphIntraTable, intraTableNode);

        xmlNode minTableTableNode = mappingNode.child("morph_min_table_table");
        parseImmediateTable(mapping.morphMinTableTable, minTableTableNode);
        
        xmlNode maxTableTableNode = mappingNode.child("morph_max_table_table");
        parseImmediateTable(mapping.morphMaxTableTable, maxTableTableNode);
        
        xmlNode intraTableTableNode = mappingNode.child("morph_intra_table_table");
        parseImmediateTable(mapping.morphIntraTableTable, intraTableTableNode);

        xmlNode modifierListNode = mappingNode.child("modifier_list");
        
        for (xmlNode nextNode = modifierListNode.child("modifier");
             nextNode;
             nextNode = nextNode.next_sibling("modifier")) {
            mqModifier modifier;
            parseModifier(modifier, nextNode);
            mapping.modifiers.push_back(modifier);
        }
    }
    
    static void parseControlParam(Configuration& configuration, const xmlNode& parentNode)
    {
        mqControlParam param = mqControlParam();
        String valueStr = parentNode.child("name").child_value();
        param.name = valueStr;
        valueStr = parentNode.child("min").child_value();
        param.min = fromString<F32>(valueStr);
        valueStr = parentNode.child("max").child_value();
        param.max = fromString<F32>(valueStr);
        mqControlParamMapPair newPair = mqControlParamMapPair(param.name, param);
        mapInsert(configuration.controlParams, newPair);
    }
    
    static void parseSound(Configuration& configuration, const xmlNode& parentNode)
    {
        mqSound sound = mqSound();
        sound.name = parentNode.child("name").child_value();
        sound.grainStart = fromString<F32>(parentNode.child("grain_start").child_value());
        sound.grainEnd = fromString<F32>(parentNode.child("grain_end").child_value());
        sound.grainDuration = fromString<S32>(parentNode.child("grain_duration").child_value());
        xmlNode waveTableNode = parentNode.child("grain_wave_table");
        sound.grainWaveTable.number = fromString<S32>(waveTableNode.child("number").child_value());
        sound.grainWaveTable.start = fromString<S32>(waveTableNode.child("start").child_value());
        sound.grainWaveTable.size = fromString<S32>(waveTableNode.child("size").child_value());
        sound.grainWaveTable.GENRoutine = fromString<S32>(waveTableNode.child("gen_routine").child_value());
        sound.grainWaveTable.filcod = waveTableNode.child("filcod").child_value();
        sound.grainWaveTable.skiptime = fromString<F32>(waveTableNode.child("skiptime").child_value());
        sound.grainWaveTable.format = fromString<S32>(waveTableNode.child("format").child_value());
        sound.grainWaveTable.channel = fromString<S32>(waveTableNode.child("channel").child_value());
        xmlNode mappingNode = parentNode.child("mapping_list");
        S32 i = 0;
        
        for (xmlNode nextNode = mappingNode.child("mapping");
             nextNode;
             nextNode = nextNode.next_sibling("mapping")) {
            parseMapping(sound.mappings[i], configuration, nextNode);
            ++i;
        }
        
        configuration.sounds.insert(std::make_pair(sound.name, sound));
    }
    
    static bool parseConfig(const String& filename, Configuration& configuration)
    {            
        xmlDocument doc;
    
        if (doc.load_file(filename.c_str()).status != 0) {
            MQ_LOG(LOG_ERROR, "Could not load config file.")
            
            return false;
        }
        
        xmlNode rootNode = doc.child("configuration");

        if (rootNode == NULL) {
            MQ_LOG(LOG_ERROR, "Config file is invalid or malformed.")
            
            return false;
        }
        
        
        configuration.name = String(rootNode.attribute("name").value());
        
        // Globals
        xmlNode globalsNode = rootNode.child("globals");
        xmlNode baseTableNumberNode = globalsNode.child("base_table_number");
        configuration.baseTableNumber = fromString<U32>(baseTableNumberNode.child_value());
        
        // Game params
        xmlNode controlParamListNode = rootNode.child("control_param_list");
        
        for (xmlNode nextNode = controlParamListNode.child("control_param");
             nextNode;
             nextNode = nextNode.next_sibling("control_param")) {
            parseControlParam(configuration, nextNode);
        }
        
        // Sounds
        xmlNode soundListNode = rootNode.child("sound_list");
        
        for (
             xmlNode nextNode = soundListNode.child("sound");
             nextNode;
             nextNode = nextNode.next_sibling("sound")
             ) {
            parseSound(configuration, nextNode);
        }
        
        return true;
    }
    
    static void serializeLinSegTable(const mqSegmentTable& table, xmlNode& parentNode)
    {
        String value = toString(table.number);
        parentNode.append_child("number");
        parentNode.child("number").append_child(xmlValue).set_value(value.c_str());
        value = toString(table.start);
        parentNode.append_child("start");
        parentNode.child("start").append_child(xmlValue).set_value(value.c_str());
        value = toString(table.size);
        parentNode.append_child("size");
        parentNode.child("size").append_child(xmlValue).set_value(value.c_str());
        value = toString(table.GENRoutine);
        parentNode.append_child("gen_routine");
        parentNode.child("gen_routine").append_child(xmlValue).set_value(value.c_str());
        xmlNode tableSegmentsNode = parentNode.append_child("segment_list");
        USize numSegments = table.segments.size();
        
        for (USize i = 0; i < numSegments; i++) {
            xmlNode segment = tableSegmentsNode.append_child("segment");
            
            value = toString(table.segments[i].value);
            segment.append_child("value").append_child(xmlValue).set_value(value.c_str());
            
            value = toString(table.segments[i].length);
            segment.append_child("length").append_child(xmlValue).set_value(value.c_str());
        }
    }
    
    static void serializeImmediateTable(const mqImmediateTable& table, xmlNode& parentNode)
    {
        String value = toString(table.number);
        parentNode.append_child("number");
        parentNode.child("number").append_child(xmlValue).set_value(value.c_str());
        value = toString(table.start);
        parentNode.append_child("start");
        parentNode.child("start").append_child(xmlValue).set_value(value.c_str());
        value = toString(table.size);
        parentNode.append_child("size");
        parentNode.child("size").append_child(xmlValue).set_value(value.c_str());
        value = toString(table.GENRoutine);
        parentNode.append_child("gen_routine");
        parentNode.child("gen_routine").append_child(xmlValue).set_value(value.c_str());
        xmlNode numberListNode = parentNode.append_child("table_number_list");
        USize numTables = table.tableNums.size();
        
        for (USize i = 0; i < numTables; i++) {
            value = toString(table.tableNums[i]);
            xmlNode numberNode = numberListNode.append_child("table_number");
            numberNode.append_child(xmlValue).set_value(value.c_str());
        }
    }
    
    static void serializeModifier(const mqModifier& modifier, xmlNode& node)
    {
        String value = "";        
        xmlNode valNode = node.append_child("control_param_value");
        value = toString(modifier.controlValue);
        valNode.append_child(xmlValue).set_value(value.c_str());
        xmlNode minTableNode = node.append_child("min_table");
        serializeLinSegTable(modifier.minTable, minTableNode);
        xmlNode maxTableNode = node.append_child("max_table");
        serializeLinSegTable(modifier.maxTable, maxTableNode);
    }
    
    static void serializeMapping(const mqMapping& mapping, xmlNode& node)
    {
        xmlNode modifierTypeNode = node.append_child("type");
        String value = mqMapping::TypeNames[mapping.type];
        modifierTypeNode.append_child(xmlValue).set_value(value.c_str());
        
        node.append_child("control_param");
        
        if (mapping.controlParam == NULL) {
            value = UNDEFINED_STR;
        } else {
            value = mapping.controlParam->name.c_str();
        }
        
        node.child("control_param").append_child(xmlValue).set_value(value.c_str());

        xmlNode minTableNode = node.append_child("morph_min_table");
        serializeLinSegTable(mapping.morphMinTable, minTableNode);

        xmlNode maxTableNode = node.append_child("morph_max_table");
        serializeLinSegTable(mapping.morphMaxTable, maxTableNode);

        xmlNode intraTableNode = node.append_child("morph_intra_table");
        serializeLinSegTable(mapping.morphIntraTable, intraTableNode);

        xmlNode mintableTableNode = node.append_child("morph_min_table_table");
        serializeImmediateTable(mapping.morphMinTableTable, mintableTableNode);
        
        xmlNode maxtableTableNode = node.append_child("morph_max_table_table");
        serializeImmediateTable(mapping.morphMaxTableTable, maxtableTableNode);
        
        xmlNode intratableTableNode = node.append_child("morph_intra_table_table");
        serializeImmediateTable(mapping.morphIntraTableTable, intratableTableNode);

        xmlNode modlinkListNode = node.append_child("modifier_list");
        USize numModLinks = mapping.modifiers.size();
        
        for (USize i = 0; i < numModLinks; i++) {
            xmlNode modifierNode = modlinkListNode.append_child("modifier");
            serializeModifier(mapping.modifiers[i], modifierNode);
        }
    }
    
    static void serializeControlParam(const mqControlParam& controlParam, xmlNode& node)
    {
        String value = "";
        
        node.append_child("name");
        value = controlParam.name;
        node.child("name").append_child(xmlValue).set_value(value.c_str());
        
        node.append_child("min");
        value = toString(controlParam.min);
        node.child("min").append_child(xmlValue).set_value(value.c_str());

        node.append_child("max");
        value = toString(controlParam.max);
        node.child("max").append_child(xmlValue).set_value(value.c_str());
    }
    
    static void serializeSound(const mqSound& sound, xmlNode& parentNode)
    {        
        String value = "";

        parentNode.append_child("name");
        parentNode.child("name").append_child(xmlValue).set_value(sound.name.c_str());
        value = toString(sound.grainStart);
        parentNode.append_child("grain_start");
        parentNode.child("grain_start").append_child(xmlValue).set_value(value.c_str());
        value = toString(sound.grainEnd);
        parentNode.append_child("grain_end");
        parentNode.child("grain_end").append_child(xmlValue).set_value(value.c_str());
        value = toString(sound.grainDuration);
        parentNode.append_child("grain_duration");
        parentNode.child("grain_duration").append_child(xmlValue).set_value(value.c_str());
        xmlNode waveTableNode = parentNode.append_child("grain_wave_table");
        value = toString(sound.grainWaveTable.number);
        waveTableNode.append_child("number");
        waveTableNode.child("number").append_child(xmlValue).set_value(value.c_str());
        value = toString(sound.grainWaveTable.start);
        waveTableNode.append_child("start");
        waveTableNode.child("start").append_child(xmlValue).set_value(value.c_str());
        value = toString(sound.grainWaveTable.size);
        waveTableNode.append_child("size");
        waveTableNode.child("size").append_child(xmlValue).set_value(value.c_str());
        value = toString(sound.grainWaveTable.GENRoutine);
        waveTableNode.append_child("gen_routine");
        waveTableNode.child("gen_routine").append_child(xmlValue).set_value(value.c_str());
        value = sound.grainWaveTable.filcod;
        waveTableNode.append_child("filcod");
        waveTableNode.child("filcod").append_child(xmlValue).set_value(value.c_str());
        value = toString(sound.grainWaveTable.skiptime);
        waveTableNode.append_child("skiptime");
        waveTableNode.child("skiptime").append_child(xmlValue).set_value(value.c_str());
        value = toString(sound.grainWaveTable.format);
        waveTableNode.append_child("format");
        waveTableNode.child("format").append_child(xmlValue).set_value(value.c_str());
        value = toString(sound.grainWaveTable.channel);
        waveTableNode.append_child("channel");
        waveTableNode.child("channel").append_child(xmlValue).set_value(value.c_str());
        xmlNode paramListNode = parentNode.append_child("mapping_list");
        USize numParams = sound.mappings.size();
        
        for (USize i = 0; i < numParams; i++) {
            xmlNode paramNode = paramListNode.append_child("mapping");
            serializeMapping(sound.mappings[i], paramNode);
        }
    }
        
    static void serializeConfig(const Configuration& config, const String& filename)
    {
        xmlDocument document;
        document.append_child(pugi::node_comment).set_value("Modiqus configuration");
        xmlNode rootNode = document.append_child("configuration");
        rootNode.append_attribute("name").set_value(config.name.c_str());
        
        // Globals
        xmlNode globalsNode = rootNode.append_child("globals");
        xmlNode baseTableNumberNode = globalsNode.append_child("base_table_number");
        String value = toString(config.baseTableNumber);
        baseTableNumberNode.append_child(xmlValue).set_value(value.c_str());

        // Sounds
        xmlNode soundListNode = rootNode.append_child("sound_list");
        mqSoundMap::const_iterator soundIt = config.sounds.begin();
        
        while (config.sounds.end() != soundIt) {
            xmlNode nextNode = soundListNode.append_child("sound");
            serializeSound(soundIt->second, nextNode);
            ++soundIt;
        }
        
        // Game params
        xmlNode paramListNode = rootNode.append_child("control_param_list");
        mqControlParamMap::const_iterator controlParamIt = config.controlParams.begin();
        
        while (config.controlParams.end() != controlParamIt) {
            xmlNode paramNode = paramListNode.append_child("control_param");
            serializeControlParam(controlParamIt->second, paramNode);
            ++controlParamIt;
        }
        
        document.save_file(filename.c_str(), "  ");
    }
    
    class RedirectOutputs
    {
    private:
        std::ostream& _myStream;
        std::streambuf *const _myBuffer;
    public:
        RedirectOutputs(std::ostream& lhs, std::ostream& rhs = std::cout) :
        _myStream(rhs),
        _myBuffer(_myStream.rdbuf())
        {
            _myStream.rdbuf(lhs.rdbuf());
        }

        ~RedirectOutputs ()
        {
            _myStream.rdbuf(_myBuffer);
        }
    };

    // redirect output stream to a string.
    class CaptureOutputs
    {
    private:
        std::ostringstream _myContents;
        const RedirectOutputs _redirect;
        
    public:
        CaptureOutputs(std::ostream& stream = std::cout) :
        _myContents(),
        _redirect(_myContents, stream) {}
        
        String contents () const
        {
            return(_myContents.str());
        }
    };
}
#endif //__IO_H__
