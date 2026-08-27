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
     * @brief define an abstract class to define command wrappers
     */
    class Command {
    public:

        /**
         * @brief Get the command type of the command
         * 
         * The type determines the implementation function that is called
         * 
         * @return `Backend::Graphic::CommandType` the type of the command
         */
        virtual Backend::Graphic::CommandType getType() const noexcept = 0;

        /**
         * @brief Get the a handle that contains the copied command arguments
         * 
         * Handles are used to parse data between the command frontend and the implementation functions
         * in a type-save way
         * 
         * @return `Backend::Graphic::CommandHandle` a handle that contains the command arguments
         */
        virtual Backend::Graphic::CommandHandle getHandle() const noexcept = 0;

    };

    /**
     * @brief a class to manage a list of commands
     * 
     * The order of the commands in the stream determines the order of operations. 
     */
    class CommandStream {
    public:

        /**
         * @brief a structure to store all data for a single command entry
         */
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

        /**
         * @brief get a specific command, but just assume that it exists
         * 
         * @throw std::out_of_range if the command does not exist
         * 
         * @tparam Cmd the type of the command to get
         * @param name the name of the command to get
         * @return `Cmd*` a pointer to the command
         */
        template <typename Cmd>
        requires std::is_base_of_v<Command, Cmd>
        inline Cmd* accessCmdUnsafe(const std::string& name) {
            //do NOT sanity check, just let it throw
            return static_cast<Cmd*>(m_entries[m_nameLookup.at(name)].cmd.get());
        }

        /**
         * @brief get a specific command
         * 
         * @tparam Cmd the type of the command to get
         * @param name the name of the command to get
         * @return `Cmd*` a pointer to the command, but `nullptr` if the command does not exist
         */
        template <typename Cmd>
        requires std::is_base_of_v<Command, Cmd>
        inline Cmd* accessCmd(const std::string& name) noexcept {
            //before returning the pointer make sure that it exists
            const auto it = m_nameLookup.find(name);
            if (it == m_nameLookup.end()) {return nullptr;}
            return static_cast<Cmd*>(m_entries[it->second].cmd.get());
        }

        /**
         * @brief get a specific command, but sanity check the type
         * 
         * @tparam Cmd the type of the command to get
         * @param name the name of the command to get
         * @return `Cmd*` a pointer to the command, but `nullptr` if the types don't match or if the command does not exist
         */
        template <typename Cmd>
        requires std::is_base_of_v<Command, Cmd>
        inline Cmd* accessCmdSafe(const std::string& name) noexcept {
            //before returning the pointer make sure that it exists
            const auto it = m_nameLookup.find(name);
            if (it == m_nameLookup.end()) {return nullptr;}
            //this time make sure that the type fits
            return dynamic_cast<Cmd*>(m_entries[it->second].cmd.get());
        }

        /**
         * @brief get a specific command, but just assume that it exists
         * 
         * @throw std::out_of_range if the command does not exist
         * 
         * @tparam Cmd the type of the command to get
         * @param name the name of the command to get
         * @return `Cmd*` a pointer to the command
         */
        template <typename Cmd>
        requires std::is_base_of_v<Command, Cmd>
        inline const Cmd* accessCmdUnsafe(const std::string& name) const {
            //do NOT sanity check, just let it throw
            return static_cast<Cmd*>(m_entries[m_nameLookup.at(name)].cmd.get());
        }

        /**
         * @brief get a specific command
         * 
         * @tparam Cmd the type of the command to get
         * @param name the name of the command to get
         * @return `Cmd*` a pointer to the command, but `nullptr` if the command does not exist
         */
        template <typename Cmd>
        requires std::is_base_of_v<Command, Cmd>
        inline const Cmd* accessCmd(const std::string& name) const noexcept {
            //before returning the pointer make sure that it exists
            const auto it = m_nameLookup.find(name);
            if (it == m_nameLookup.end()) {return nullptr;}
            return static_cast<Cmd*>(m_entries[it->second].cmd.get());
        }

        /**
         * @brief get a specific command, but sanity check the type
         * 
         * @tparam Cmd the type of the command to get
         * @param name the name of the command to get
         * @return `Cmd*` a pointer to the command, but `nullptr` if the types don't match or if the command does not exist
         */
        template <typename Cmd>
        requires std::is_base_of_v<Command, Cmd>
        inline const Cmd* accessCmdSafe(const std::string& name) const noexcept {
            //before returning the pointer make sure that it exists
            const auto it = m_nameLookup.find(name);
            if (it == m_nameLookup.end()) {return nullptr;}
            //this time make sure that the type fits
            return dynamic_cast<Cmd*>(m_entries[it->second].cmd.get());
        }

        /**
         * @brief check if a command is dirty
         * 
         * @throw std::out_of_range if the command does not exist
         * 
         * @param name the name of the command to check
         * @return `true` if the command is dirty, `false` if not
         */
        inline bool isDirtyUnsafe(const std::string& name) const 
        {return m_entries[m_nameLookup.at(name)].dirty;}

        /**
         * @brief check if a command is dirty
         * 
         * @param name the name of the command state to query
         * @return `true` if the command entry is marked dirty, `false` if not or if the command does not exist
         */
        inline bool isDirty(const std::string& name) const noexcept {
            //before returning the pointer make sure that it exists
            const auto it = m_nameLookup.find(name);
            if (it == m_nameLookup.end()) {return false;}
            return m_entries[it->second].dirty;
        }

        /**
         * @brief Get the Command Buffer
         * 
         * @param name the name of the command to get the command buffer for
         * @return `const Reference<Backend::Graphic::CommandBuffer>&` a constant reference to the backend command buffer
         */
        inline const Reference<Backend::Graphic::CommandBuffer>& getCommandBuffer(const std::string& name)
        {return m_entries[m_nameLookup.at(name)].cmdBuff;}

        /**
         * @brief Get the Entries
         * 
         * @return `const std::vector<CommandEntry>&` a constant reference to all command entries
         */
        inline const std::vector<CommandEntry>& getEntries() const noexcept
        {return m_entries;}

        /**
         * @brief a helper command to print the names of the commands in execution order
         * 
         * This command is intended to help developers to debug ordering issues with the command stream
         * 
         * @warning This command is not optimized. It may be slow
         */
        inline void debugPrint() const noexcept {
            //ordered list to print
            std::vector<std::string> toPrint(m_entries.size(), "");
            //fill ordered list
            for (const auto& [name, idx] : m_nameLookup)
            {toPrint[idx] = name;}
            //print
            std::stringstream stream; //used to prevent ordering issues in multithreaded env
            stream << "Debug print of command stream:\n";
            size_t i = 0;
            for (const auto& name : toPrint)
            {stream << "    [" << (i++) << "] : " <<  << name << "\n";}
            std::cout << stream.str();
        }

    protected:

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