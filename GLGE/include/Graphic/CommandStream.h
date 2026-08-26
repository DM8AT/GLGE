/**
 * @file CommandStream.h
 * @author DM8AT
 * @brief define a command stream as a sequence of commands with arguments
 * 
 * All commands can be individually enabled or disabled without requiring re-compiling. 
 * A command stream owns multiple command buffers where each command buffer is responsible for one command. 
 * Command streams can record and rebuild in parallel. 
 * 
 * @version 0.1
 * @date 2026-08-26
 * 
 * @copyright Copyright (c) 2026
 * 
 */
//header guard
#ifndef _GLGE_GRAPHIC_COMMAND_STREAM_
#define _GLGE_GRAPHIC_COMMAND_STREAM_

//include common stuff
#include "Core/Common.h"

//add the command backend
#include "Backend/Graphics/CommandHandle.h"
#include "Backend/Graphics/CommandTable.h"
#include "Backend/Graphics/CommandBuffer.h"

//use the library namespace
namespace GLGE::Graphic {

    /**
     * @brief define a class 
     */
    class Command {
    public:

        virtual Backend::Graphic::CommandType getType() const noexcept = 0;

        virtual Backend::Graphic::CommandHandle getHandle() const noexcept = 0;

    };

    class CommandStream {
    public:

        /**
         * @brief create a new command stream
         * 
         * @tparam `Cmds` the command types to add to the stream
         * @param cmds the commands to initialize the stream with
         */
        template <typename... Cmds>
        requires (std::is_base_of_v<Command, Cmds> && ... && true)
        CommandStream(std::pair<const std::string&, const std::unique_ptr<Cmds>&>... cmds)
        {}

        /**
         * @brief add a new command to the command stream
         * 
         * @tparam Cmd the type of the command to add
         * @param name the name of the command to add
         * @param cmd the command to add
         */
        template <typename Cmd>
        requires (std::is_base_of_v<Command, Cmd>)
        void addCmd(const std::string& name, const std::unique_ptr<Cmd>& cmd)
        {}

        /**
         * @brief check if a command with the name exists
         * 
         * @param name the name to check for
         * @return `true` if a command with that name exists, `false` if not
         */
        inline bool hasCmd(const std::string& name) const noexcept
        {return m_nameLookup.contains(name);}

        /**
         * @brief remove a command with the specific name
         * 
         * If no command with this name exists, this does nothing
         * 
         * @param name the name of the command to remove
         */
        void removeCmd(const std::string& name) noexcept
        {}

        /**
         * @brief mark all elements as dirty
         */
        void markDirty() {}

        /**
         * @brief compile all dirty elements
         */
        void compile() {}

    protected:

        struct CommandEntry {
            /**
             * @brief store the command buffer for the entry
             */
            Reference<Backend::Graphic::CommandBuffer> cmdBuff;
            /**
             * @brief store the command callback
             */
            std::unique_ptr<Command> cmd;
            /**
             * @brief store if the command buffer is up to date
             */
            bool dirty = false;
        };

        /**
         * @brief store the command entries
         */
        std::vector<CommandEntry> m_entries;
        /**
         * @brief store a table to look up a command via a name
         */
        std::unordered_map<std::string, size_t> m_nameLookup;

    };

}

#endif