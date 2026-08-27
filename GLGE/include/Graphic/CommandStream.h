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

//add commands
#include "Command.h"

//add the command backend
#include "Backend/Graphics/CommandBuffer.h"

//add instances
#include "Instance.h"
#include "Core/BaseClass.h"

//use the library namespace
namespace GLGE::Graphic {

    /**
     * @brief a class to manage a list of commands
     * 
     * The order of the commands in the stream determines the order of operations. 
     */
    class CommandStream : public BaseClass {
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
             * @brief store if the command buffer should be consumed each tick
             * 
             * This is independent of the dirty state. The validity of the command buffer is independent of the enabled state. 
             */
            bool enabled = true;
            /**
             * @brief store if the command buffer can be rebuild when required
             * 
             * If this is false, the command buffer will not be rebuild automatically, regardless of the dirty state. 
             * If the command buffer is dirty and this is false, the command buffer will not be consumed. 
             */
            bool allowRebuild = true;
        };

        /**
         * @brief create a new command stream
         * 
         * @throws `GLGE::Exception` if multiple definitions of the same name exists
         * 
         * @tparam `Cmds` the command types to add to the stream
         * @param cmds the commands to initialize the stream with
         */
        template <typename... Cmds>
        requires (std::is_base_of_v<Command, Cmds> && ... && true)
        CommandStream(std::pair<const char*, std::unique_ptr<Cmds>>... cmds) 
         : BaseClass(), m_instance(getInstance()->getExtension<GLGE::Graphic::Instance>())
        {
            //sanity check
            #if GLGE_DEBUG
            if (m_instance == nullptr) {throw GLGE::Exception("Cannot create a command stream without graphics", "GLGE::Graphic::CommandStream::CommandStream");}
            #endif
            //pre-resize
            m_entries.reserve(sizeof...(Cmds));
            m_nameLookup.reserve(sizeof...(Cmds));
            //add to storage
            (
                [&] {
                    #if GLGE_DEBUG
                    if (m_nameLookup.contains(cmds.first)) {throw GLGE::Exception("Found multiple definitions of a name", "GLGE::Graphic::CommandStream::CommandStream");}
                    #endif
                    addCmd<Cmds>(std::string(cmds.first), cmds.second);
                } (), ...
            );
        }

        //cannot be copied
        CommandStream(const CommandStream&) = delete;
        CommandStream& operator=(const CommandStream&) = delete;

        /**
         * @brief Construct a new Command Stream
         * 
         * Move constructor
         */
        CommandStream(CommandStream&&) noexcept = default;
        /**
         * @brief Move-assign operator for command stream
         * 
         * @return `CommandStream&` a reference to the command stream after moving
         */
        CommandStream& operator=(CommandStream&&) noexcept = default;

        /**
         * @brief add a new command to the command stream
         * 
         * @tparam Cmd the type of the command to add
         * @param name the name of the command to add
         * @param cmd the command to add
         */
        template <typename Cmd>
        requires (std::is_base_of_v<Command, Cmd>)
        void addCmd(const std::string& name, std::unique_ptr<Cmd>& cmd) {
            //sanity check in debug
            #if GLGE_DEBUG
            if (m_nameLookup.contains(name)) {throw GLGE::Exception("Tried to add a name that allready exists", "GLGE::Graphic::CommandStream::addCmd");}
            #endif
            //add entry
            m_nameLookup[name] = m_entries.size();
            m_entries.emplace_back(m_instance->getGraphicDescription()->createCommandBuffer(m_instance), std::move(cmd), true, true);
        }

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
        void removeCmd(const std::string& name) noexcept {
            auto it = m_nameLookup.find(name);
            if (it != m_nameLookup.end()) {m_nameLookup.erase(it);}
        }

        /**
         * @brief Get the index of a specific command
         * 
         * The index is the index in the execution order starting from 0. The higher the index is, the later it will run. 
         * 
         * @param name the name of the command to query the index for
         * @return `size_t` the index of the command, `SIZE_MAX` if the command does not exist
         */
        inline size_t getIndexOfCmd(const std::string& name) const noexcept {
            auto it = m_nameLookup.find(name);
            if (it == m_nameLookup.end()) {return SIZE_MAX;}
            return it->second;
        }

        /**
         * @brief add a new command to the stream
         * 
         * @throws `GLGE::Exception` if idx is higher than the current command count
         * 
         * @tparam Cmd the type of the command to add
         * @param name the name for the command
         * @param cmd the command to add
         * @param idx the index to insert the command at
         */
        template <typename Cmd>
        requires (std::is_base_of_v<Command, Cmd>)
        inline void addCmd(const std::string& name, std::unique_ptr<Cmd>& cmd, size_t idx) {
            //sanity check
            #if GLGE_DEBUG
            if (m_nameLookup.contains(name)) {throw GLGE::Exception("Tried to add a name that allready exists", "GLGE::Graphic::CommandStream::addCmd");}
            if (idx >= (m_entries.size() + 1)) {throw GLGE::Exception("Cannot add a command at the specified index", "GLGE::Graphic::CommandStream::addCmd");}
            #endif
            m_entries.insert((idx == m_entries.size()) ? m_entries.end() : (m_entries.begin() + idx), m_instance->getGraphicDescription()->createCommandBuffer(m_instance), std::move(cmd), true, true);
            for (auto& [_, i] : m_nameLookup) {if (i >= idx) {++i;}}
            m_nameLookup[name] = idx;
        }

        /**
         * @brief remove a command with the specific name
         * 
         * If the index is out of range, this does nothing
         * 
         * @param idx the index of the command to remove
         */
        inline void removeCmd(size_t idx) noexcept {
            //sanity check
            if (idx >= m_entries.size()) {return;}

            //remove entry
            m_entries.erase(m_entries.begin() + idx);
            //remove names
            for (auto it = m_nameLookup.begin(); it != m_nameLookup.end();) 
            {if (it->second == idx) {it = m_nameLookup.erase(it);} else {++it;}}
        }

        /**
         * @brief Get the name for a specific command
         * 
         * @throws std::out_of_range if the index is not valid
         * 
         * @param idx the index of the command to get the name for
         * @return `const std::string&` the name of the command
         */
        inline const std::string& getNameForCmd(size_t idx) const {
            for (const auto& [name, i] : m_nameLookup) {if (idx == i) {return name;}}
            throw std::out_of_range("The requested index is out of range");
        }

        /**
         * @brief mark all elements as dirty
         */
        void markDirty() {for (auto& el : m_entries) {el.cmd->markDirty();}}

        /**
         * @brief compile all dirty elements
         */
        void compile() {
            for (auto& el : m_entries) {
                if (el.cmd->isDirty() && el.allowRebuild) {
                    //get the command invoker
                    const auto* cmd = m_instance->getGraphicDescription()->getCommandTable()->getCommand(static_cast<u32>(el.cmd->getType()));
                    //if it is nullptr, skip, retry later
                    if (cmd == nullptr) {continue;}
                    cmd->func(*el.cmdBuff, el.cmd->getHandle());

                    //cmd is no longer dirty
                    el.cmd->m_dirty = false;
                }
            }
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
         * @param idx the index of the command to get
         * @return `Cmd*` a pointer to the command
         */
        template <typename Cmd>
        requires std::is_base_of_v<Command, Cmd>
        inline Cmd* accessCmdUnsafe(size_t idx) {
            //do NOT sanity check, just let it throw
            return static_cast<Cmd*>(m_entries.at(idx).cmd.get());
        }

        /**
         * @brief get a specific command
         * 
         * @tparam Cmd the type of the command to get
         * @param idx the index of the command to get
         * @return `Cmd*` a pointer to the command, but `nullptr` if the command does not exist
         */
        template <typename Cmd>
        requires std::is_base_of_v<Command, Cmd>
        inline Cmd* accessCmd(size_t idx) noexcept {
            //before returning the pointer make sure that it exists
            if (idx >= m_entries.size()) {return nullptr;}
            return static_cast<Cmd*>(m_entries[idx].cmd.get());
        }

        /**
         * @brief get a specific command, but sanity check the type
         * 
         * @tparam Cmd the type of the command to get
         * @param idx the index of the command to get
         * @return `Cmd*` a pointer to the command, but `nullptr` if the types don't match or if the command does not exist
         */
        template <typename Cmd>
        requires std::is_base_of_v<Command, Cmd>
        inline Cmd* accessCmdSafe(size_t idx) noexcept {
            //before returning the pointer make sure that it exists
            if (idx >= m_entries.size()) {return nullptr;}
            //this time make sure that the type fits
            return dynamic_cast<Cmd*>(m_entries[idx].cmd.get());
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
        {return m_entries[m_nameLookup.at(name)].cmd->isDirty();}

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
            return m_entries[it->second].cmd->isDirty();
        }

        /**
         * @brief check if a command is enabled
         * 
         * @throw std::out_of_range if the command does not exist
         * 
         * @param name the name of the command to check
         * @return `true` if the command is enabled, `false` if not
         */
        inline bool isEnabledUnsafe(const std::string& name) const 
        {return m_entries[m_nameLookup.at(name)].enabled;}

        /**
         * @brief check if a command is enabled
         * 
         * @param name the name of the command state to query
         * @return `true` if the command entry is marked enabled, `false` if not or if the command does not exist
         */
        inline bool isEnabled(const std::string& name) const noexcept {
            //before returning the pointer make sure that it exists
            const auto it = m_nameLookup.find(name);
            if (it == m_nameLookup.end()) {return false;}
            return m_entries[it->second].enabled;
        }

        /**
         * @brief check if a command is dirty
         * 
         * @throw std::out_of_range if the command does not exist
         * 
         * @param idx the index of the command to check
         * @return `true` if the command is dirty, `false` if not
         */
        inline bool isDirtyUnsafe(size_t idx) const 
        {return m_entries.at(idx).cmd->isDirty();}

        /**
         * @brief check if a command is dirty
         * 
         * @param idx the index of the command to check
         * @return `true` if the command entry is marked dirty, `false` if not or if the command does not exist
         */
        inline bool isDirty(size_t idx) const noexcept {
            //before returning the pointer make sure that it exists
            if (idx >= m_entries.size()) {return false;}
            return m_entries[idx].cmd->isDirty();
        }

        /**
         * @brief check if a command is enabled
         * 
         * @throw std::out_of_range if the command does not exist
         * 
         * @param idx the index of the command to check
         * @return `true` if the command is enabled, `false` if not
         */
        inline bool isEnabledUnsafe(size_t idx) const 
        {return m_entries.at(idx).enabled;}

        /**
         * @brief check if a command is enabled
         * 
         * @param idx the index of the command to check
         * @return `true` if the command entry is marked enabled, `false` if not or if the command does not exist
         */
        inline bool isEnabled(size_t idx) const noexcept {
            //before returning the pointer make sure that it exists
            if (idx >= m_entries.size()) {return false;}
            return m_entries[idx].enabled;
        }

        /**
         * @brief Get the Command Buffer
         * 
         * @throws if the name is not the name of a known command
         * 
         * @param name the name of the command to get the command buffer for
         * @return `const Reference<Backend::Graphic::CommandBuffer>&` a constant reference to the backend command buffer
         */
        inline const Reference<Backend::Graphic::CommandBuffer>& getCommandBuffer(const std::string& name) const
        {return m_entries[m_nameLookup.at(name)].cmdBuff;}

        /**
         * @brief Get the Command Buffer
         * 
         * @throws if the index is invalid
         * 
         * @param idx the index of the command to get the buffer for
         * @return `const Reference<Backend::Graphic::CommandBuffer>&` a constant reference to the backend command buffer
         */
        inline const Reference<Backend::Graphic::CommandBuffer>& getCommandBuffer(size_t idx) const
        {return m_entries.at(idx).cmdBuff;}

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
            {stream << "    [" << i << "] : " << name << ", Dirty: " << (m_entries[i].cmd->isDirty() ? "true" : "false") << "\n"; ++i;}
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
        /**
         * @brief store a pointer to the graphic instance the command stream belongs to
         */
        GLGE::Graphic::Instance* m_instance = nullptr;

    };

}

#endif