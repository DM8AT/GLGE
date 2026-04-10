/**
 * @file CommandTable.h
 * @author DM8AT
 * @brief define a structure that holds information on how to translate frontend commands into actual commands
 * 
 * This is a structure holds an unordered map that maps the frontend command IDs to actual functions the backend can
 * run to record the frontend commands. All functions are required to take in a command buffer and a command handle. 
 * 
 * @version 0.1
 * @date 2025-12-30
 * 
 * @copyright Copyright (c) 2025
 * 
 */
//header guard
#ifndef _GLGE_GRAPHIC_BACKEND_GRAPHIC_COMMAND_TABLE_
#define _GLGE_GRAPHIC_BACKEND_GRAPHIC_COMMAND_TABLE_

//include the abstract commands from the frontend
#include "Graphic/Command.h"

//use the library namespace
namespace GLGE::Graphic::Backend::Graphic {

    //command buffers and handles are defined somewhere else
    class CommandBuffer;
    class CommandHandle;

    /**
     * @brief store what a command table actually is
     * 
     * In contrast to most backend classes this is NOT a pure virtual class. This is the class with actual implementation. 
     * This class has the purpose to link the frontend abstract commands with functions that implement the functionality for 
     * those abstract commands where the abstract command is translated into the actual required API calls for the GPU. 
     */
    class CommandTable {
    public:

        /**
         * @brief define the type of the function that a command wrapper should have
         * 
         * @param cmdBuff a reference to the command buffer to fill with the actual GPU tasks
         * @param handle a constant reference to a command handle that is used to quarry the actual functoin arguments
         * @return `true` if the command was recorded successfully, `false` for an error
         */
        using CommandWrapper = bool (*)(CommandBuffer& cmdBuff, const CommandHandle& handle);

        /**
         * @brief store the structure of each table entry
         */
        struct TableEntry {
            /**
             * @brief store the function to call to record this abstract command
             * 
             * @warning if the function is equal to NULL, then the table entry is considered invalid. 
             */
            CommandWrapper func = nullptr;
            /**
             * @brief the size of the payload that is required for the function
             */
            size_t payloadSize = 0;

            /**
             * @brief create a new table entry
             * 
             * This function creates a table entry from the inputted types
             * 
             * @tparam Args the types that will later be passed by the handle
             * @param func the actual function to call for the wrapping
             * @return `constexpr TableEntry` the table entry capable of handing the parameters to the wrapper function
             */
            template <typename... Args>
            inline static constexpr TableEntry create(CommandWrapper func) noexcept {
                //compute total size of all arguments
                constexpr size_t total_size = (sizeof(std::decay_t<Args>) + ... + 0);

                //return a new wrapper
                return TableEntry(func, total_size);
            }
        };

        /**
         * @brief Construct a new Command Table
         * 
         * @param entries a list of pre-loaded entries to the command table
         */
        CommandTable(std::initializer_list<std::pair<u32, TableEntry>> entries = {});

        /**
         * @brief Destroy the Command Table
         */
        ~CommandTable() {}

        /**
         * @brief add a new command to the command table
         * 
         * @param id the ID of the abstract command that identifies with this command
         * @param entry the actual table entry to store for the new command
         * @param allowOverride `true` to allow the new command to override allready existing commands, `false` prohibits overriding. `false` by default. 
         * @param allowEngineOverride `true` to allow overrides to also effect the ID region reserved for engine commands. `false` prohibits overrides to the engine ID region. `false` by default. 
         * 
         * @return `true` if the command was successfully added, `false` if overriding failed
         */
        bool addCommand(u32 id, const TableEntry& entry, bool allowOverride = false, bool allowEngineOverride = false);

        /**
         * @brief check if the command table stores a definition for a specific command
         * 
         * @param id the ID of the abstract command to check
         * @return `true` if the command is defined, `false` otherwise
         */
        bool hasCommand(u32 id);

        /**
         * @brief Get the command that corresponds
         * 
         * @param id the ID of the command to quarry
         * @return `const TableEntry*` a constant pointer to the table entry or `nullptr` if the command was not found
         */
        const TableEntry* getCommand(u32 id);

        /**
         * @brief a function to check if an ID falls into the scope of the engine's IDs
         * 
         * @param id the ID to check
         * @return `true` if the ID belongs to the engines ID scope, `false` otherwise
         */
        inline static constexpr bool isEngineCommand(u32 id) noexcept 
        {return (id < (u32)GLGE::Graphic::COMMAND_CUSTOM);}

    protected:

        /**
         * @brief use a densely packed table for all engine-side commands
         * 
         * The amount of table entries is exactly the enum value reserved for the start of the custom command section
         * as this is notes the minimum index for a custom command - and one above the maximum index of an engine command.
         * This matches the amount of valid engine commands as indexing is 0 based. 
         */
        TableEntry m_engineTable[(u32)GLGE::Graphic::COMMAND_CUSTOM] = { 0 };
        /**
         * @brief custom commands use an unordered map as the range of possible indices is too large for a pre-loaded array
         * 
         * This allows custom commands to take full advantage of the index space. Engine commands are separated as an optimization
         * as engine commands are expected to be the most common ones. 
         */
        std::unordered_map<u32, TableEntry> m_customTable;

    };

}

#endif