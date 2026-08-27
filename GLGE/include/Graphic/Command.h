/**
 * @file Command.h
 * @author DM8AT
 * @brief define what a command is
 * @version 0.1
 * @date 2026-08-27
 * 
 * @copyright Copyright (c) 2026
 * 
 */
//header guard
#ifndef _GLGE_GRAPHIC_COMMAND_
#define _GLGE_GRAPHIC_COMMAND_

//add common stuff
#include "Core/Common.h"

//add the backend stuff
#include "Backend/Graphics/CommandHandle.h"
#include "Backend/Graphics/CommandTable.h"

//use the library namespace
namespace GLGE::Graphic {

    //forward declaration
    class CommandInvalidator;

    /**
     * @brief define an abstract class to define command wrappers
     */
    class Command {
    public:

        /**
         * @brief Construct a new Command
         * 
         * @tparam Ts the types to register
         * @param ts the instances to register
         */
        template <typename... Ts>
        Command(Ts&... ts) {multiRegisterStoredArgument<Ts...>(ts...);}

        /**
         * @brief Destroy the Command
         */
        virtual ~Command();

        //cannot be moved nor copied
        Command(const Command&) = delete;
        Command(Command&&) = delete;
        Command& operator=(const Command&) = delete;
        Command& operator=(Command&&) = delete;

        /**
         * @brief Get the command type of the command
         * 
         * The type determines the implementation function that is called
         * 
         * @return `Backend::Graphic::CommandType` the type of the command
         */
        [[nodiscard]] virtual Backend::Graphic::CommandType getType() const noexcept = 0;

        /**
         * @brief Get the a handle that contains the copied command arguments
         * 
         * Handles are used to parse data between the command frontend and the implementation functions
         * in a type-save way
         * 
         * @return `Backend::Graphic::CommandHandle` a handle that contains the command arguments
         */
        [[nodiscard]] virtual Backend::Graphic::CommandHandle getHandle() noexcept = 0;

        /**
         * @brief check if the command is dirty
         */
        inline bool isDirty() const noexcept
        {return m_dirty;}

        /**
         * @brief mark this command as dirty
         */
        inline void markDirty() noexcept
        {m_dirty = true;}

    private:

        /**
         * @brief a function that is used to register an argument
         * 
         * @tparam T the type of the argument to add
         * @param arg the instance of the argument to register
         */
        template <typename T>
        void registerStoredArgument(T& arg);

        /**
         * @brief a function to register multiple arguments
         * 
         * @tparam Ts the argument types to register
         * @param args 
         */
        template <typename... Ts>
        void multiRegisterStoredArgument(Ts&... args)
        {(registerStoredArgument<Ts>(args), ...);}

        //command streams are friends
        //(they mark as clean)
        friend class CommandStream;
        //command invalidator instances are friends
        //they need that to update pointer on move / copy
        friend class CommandInvalidator;

        /**
         * @brief remove an invalidator
         * 
         * @param invalidator a pointer to the invalidator to remove
         */
        void removeInvalidator(CommandInvalidator* invalidator)
        {for (size_t i = 0; i < m_invalidator.size();) {if (m_invalidator[i] == invalidator) {m_invalidator.erase(m_invalidator.begin() + i);} else {++i;}}}

        /**
         * @brief replace a specific invalidator pointer with another
         * 
         * @param old the pointer to replace
         * @param next the pointer to replace it with
         */
        void replaceInvalidator(CommandInvalidator* old, CommandInvalidator* next)
        {for (auto& el : m_invalidator) {if (el == old) {el = next;}}}

        /**
         * @brief store pointers to the all invalidator instances that reference this command
         */
        std::vector<CommandInvalidator*> m_invalidator;

        /**
         * @brief store if the command is dirty
         */
        bool m_dirty = false;

    };

    /**
     * @brief a class that is used to mark some class as able to invalidate a command
     */
    class CommandInvalidator {
    public:

        /**
         * @brief Construct a new Command Invalidator
         */
        CommandInvalidator() = default;

        /**
         * @brief Destroy the Command Invalidator
         */
        ~CommandInvalidator() 
        {for (auto& cmd : m_cmds) {cmd->removeInvalidator(this);}}

        /**
         * @brief Construct a new Command Invalidator
         * 
         * Copy constructor
         * 
         * @param other the invalidator instance to copy
         */
        CommandInvalidator(const CommandInvalidator& other) 
         : m_cmds(other.m_cmds) //just copy the command list over
        {
            //register to all commands
            for (auto& cmd : m_cmds) {cmd->m_invalidator.push_back(this);}
        }
        /**
         * @brief Construct a new Command Invalidator
         * 
         * Move constructor
         * 
         * @param other the command invalidator to move from
         */
        CommandInvalidator(CommandInvalidator&& other) 
         : m_cmds(std::move(other.m_cmds))
        {
            //notify all of them commands about the move
            for (auto& cmd : m_cmds) {cmd->replaceInvalidator(&other, this);}
        }

        /**
         * @brief copy a command invalidator
         * 
         * @param other the command invalidator to copy from
         * @return `CommandInvalidator&` a reference to the command invalidator after copying
         */
        CommandInvalidator& operator=(const CommandInvalidator& other) {
            //prevent copy to self
            if (this == &other) {return *this;}

            //copy data over
            m_cmds = other.m_cmds;
            for (auto& cmd : m_cmds) {cmd->m_invalidator.push_back(this);}

            //return ref to this
            return *this;
        }
        /**
         * @brief move a command invalidator
         * 
         * @param other the command invalidator to move from
         * @return `CommandInvalidator&` a reference to the command invalidator after copying
         */
        CommandInvalidator& operator=(CommandInvalidator&& other) {
            //prevent move to self
            if (this == &other) {return *this;}

            //move data over
            m_cmds = std::move(other.m_cmds);
            for (auto& cmd : m_cmds) {cmd->replaceInvalidator(&other, this);}

            //return ref to this
            return *this;
        }

    protected:

        /**
         * @brief A function to note that this structure changed in a way that invalidates the command buffers
         */
        void invalidate()
        {for (auto& cmd : m_cmds) {cmd->markDirty();}}

    private:

        //commands are friends
        //they need that to delete themself
        friend class Command;

        /**
         * @brief remove a specific command
         * 
         * @param cmd a pointer to the command to remove
         */
        void removeCmd(Command* cmd) 
        {for (size_t i = 0; i < m_cmds.size();) {if (m_cmds[i] == cmd) {m_cmds.erase(m_cmds.begin() + i);} else {++i;}}}

        /**
         * @brief store pointer to all commands that can access this object
         */
        std::vector<Command*> m_cmds;

    };

    template <typename T>
    void Command::registerStoredArgument(T& arg) {
        if constexpr (std::is_base_of_v<CommandInvalidator, T>) {
            //add to bi-directional tracking
            m_invalidator.push_back(static_cast<CommandInvalidator*>(&arg));
            static_cast<CommandInvalidator*>(&arg)->m_cmds.push_back(this);
        } else {
            //Do nothing
        }
    }

}

#endif