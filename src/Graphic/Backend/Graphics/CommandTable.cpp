/**
 * @file CommandTable.h
 * @author DM8AT
 * @brief implement the command table structure
 * @version 0.1
 * @date 2025-12-30
 * 
 * @copyright Copyright (c) 2025
 * 
 */
//include command tables
#include "Graphic/Backend/Graphics/CommandTable.h"
//add profiling
#include "Core/Profiler.h"

//use the backend namespace
using namespace GLGE::Graphic::Backend::Graphic;

CommandTable::CommandTable(std::initializer_list<std::pair<u32, TableEntry>> entries) {
    GLGE_PROFILER_SCOPE();

    //iterate over all commands and add them
    for (auto& [id, entry] : entries)
    {addCommand(id, entry);}
}

bool CommandTable::addCommand(u32 id, const TableEntry& entry, bool allowOverride, bool allowEngineOverride) {
    GLGE_PROFILER_SCOPE();

    //check if the command is an engine or custom command
    if (isEngineCommand(id)) {
        //the command is an engine command
        
        //get the table entry
        TableEntry& _entry = m_engineTable[id];

        //if the engine was not compiled with `GLGE_GRAPHIC_ENGINE_COMMAND_OVERRIDE` defined, force `allowEngineOverride` to be false
        #ifndef GLGE_GRAPHIC_ENGINE_COMMAND_OVERRIDE
        allowEngineOverride = false;
        #endif

        //check if the command is used
        //if the command is occupied, only store if overrides are set
        if ((_entry.func == nullptr) || (allowOverride && allowEngineOverride)) {
            //store the new table entry
            m_engineTable[id] = entry;
            //success
            return true;
        }
    } else {
        //the command is a user command

        //get the table entry
        auto pos = m_customTable.find(id);

        //check if the command is used
        //if the command is occupied check if overriding is enabled
        if (pos == m_customTable.end() || allowOverride) {
            //store the new table entry
            m_customTable.insert_or_assign(id, entry);
            //success
            return true;
        }
    }

    //this point is reached if overriding failed
    return false;
}

bool CommandTable::hasCommand(u32 id) {
    GLGE_PROFILER_SCOPE();

    //check if the command is an engine command
    if (isEngineCommand(id)) {
        //the command lives in the engine table
        //engine table commands exist if the function pointer is set. 
        //so return `true` if the function pointer is not null
        return (m_engineTable[id].func == nullptr) ? false : true;
    } else {
        //else, use the contains method of the hashmap to compute if the ID is used
        return m_customTable.contains(id);
    }
}

const CommandTable::TableEntry* CommandTable::getCommand(u32 id) {
    GLGE_PROFILER_SCOPE();

    //check if the command is an engine command
    if (isEngineCommand(id)) {
        //the command lives in the engine table
        //check if the engine table has a valid function pointer
        //if not return nullptr
        if (m_engineTable[id].func)
        {return &m_engineTable[id];}
        else
        {return nullptr;}
    } else {
        //else, the command is a custom command

        //get an iterator to the command
        //then, check if the iterator is valid.
        //If the iterator is valid, return a pointer to the stored element. 
        //If not, return null
        auto pos = m_customTable.find(id);
        if (pos == m_customTable.end())
        {return nullptr;}
        return &pos->second;
    }
}