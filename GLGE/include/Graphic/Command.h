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

//add command backend
#include "Backend/Graphics/CommandHandle.h"
#include "Backend/Graphics/CommandTable.h"

//use the graphic namespace
namespace GLGE::Graphic {

    //forward declaration
    class CommandInvalidator;

    /**
     * @brief Base class for objects that can participate in command invalidation
     */
    class CommandInvalidateble {
    public:

        /**
         * @brief Destroy the CommandInvalidateble
         */
        virtual ~CommandInvalidateble() = default;

        /**
         * @brief Notify this object that it has been invalidated
         */
        virtual void onInvalidate() = 0;

    };

    /**
     * @brief Define an abstract class to define command wrappers
     */
    class Command : public CommandInvalidateble {
    public:

        /**
         * @brief Construct a new Command
         *
         * @tparam Ts The types to register
         * @param ts The instances to register
         */
        template <typename... Ts>
        Command(Ts&... ts)
        {multiRegisterStoredArgument<Ts...>(ts...);}

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
         * @brief Get the command type
         *
         * @return `Backend::Graphic::CommandType`
         */
        [[nodiscard]] virtual Backend::Graphic::CommandType getType() const noexcept = 0;

        /**
         * @brief Get a handle containing the copied command arguments
         *
         * @return `Backend::Graphic::CommandHandle`
         */
        [[nodiscard]] virtual Backend::Graphic::CommandHandle getHandle() noexcept = 0;

        /**
         * @brief Invalidate this command
         */
        void onInvalidate() override
        {m_dirty = true;}

        /**
         * @brief Check if the command is dirty
         */
        [[nodiscard]] inline bool isDirty() const noexcept
        {return m_dirty;}

        /**
         * @brief Mark this command as dirty
         */
        inline void markDirty() noexcept
        {m_dirty = true;}

    private:

        /**
         * @brief Register an argument as an invalidation dependency
         *
         * @tparam T Argument type
         * @param arg Argument to register
         */
        template <typename T>
        void registerStoredArgument(T& arg);

        /**
         * @brief Register multiple arguments
         *
         * @tparam Ts Argument types
         * @param args Arguments to register
         */
        template <typename... Ts>
        void multiRegisterStoredArgument(Ts&... args)
        {(registerStoredArgument<Ts>(args), ...);}

        //friend classes
        friend class CommandStream;
        friend class CommandInvalidator;

        /**
         * @brief Remove an invalidator from this command
         *
         * @param invalidator Invalidator to remove
         */
        void removeInvalidator(CommandInvalidator* invalidator) {
            for (size_t i = 0; i < m_invalidator.size();) {
                if (m_invalidator[i] == invalidator) {
                    m_invalidator.erase(m_invalidator.begin() + i);
                } else {
                    ++i;
                }
            }
        }

        /**
         * @brief Replace an invalidator pointer
         *
         * Used when an invalidator is moved
         *
         * @param old Old invalidator pointer
         * @param next New invalidator pointer
         */
        void replaceInvalidator(CommandInvalidator* old, CommandInvalidator* next) {
            for (auto& el : m_invalidator) {
                if (el == old) 
                {el = next;}
            }
        }

        /**
         * @brief Invalidators that directly affect this command
         */
        std::vector<CommandInvalidator*> m_invalidator;

        /**
         * @brief Whether this command needs to be rebuilt/re-recorded
         */
        bool m_dirty = true;

    };

    /**
     * @brief An object that can invalidate commands and other invalidators
     *
     * CommandInvalidator objects can form a hierarchy:
     * Parent Invalidator -> Child Invalidator -> Command
     *
     * Invalidating a child propagates upwards to its parents and then
     * eventually invalidates commands attached to those invalidators
     */
    class CommandInvalidator : public CommandInvalidateble {
    public:

        /**
         * @brief Construct a new Command Invalidator
         */
        CommandInvalidator() = default;

        /**
         * @brief Destroy the Command Invalidator
         */
        ~CommandInvalidator() {
            clearCommands();
            clearChildren();
            clearParents();
        }

        /**
         * @brief Copy constructor
         *
         * Copies all relationships while creating new bidirectional links
         * 
         * @param other the invalidator to copy from
         */
        CommandInvalidator(const CommandInvalidator& other)
         : m_cmds(other.m_cmds), m_children(other.m_children), m_parents(other.m_parents)
        {
            registerWithCommands();
            registerWithChildren();
            registerWithParents();
        }

        /**
         * @brief Move constructor
         *
         * Transfers all relationships and replaces the old object's address
         * in every related object
         * 
         * @param other the invalidator to move from
         */
        CommandInvalidator(CommandInvalidator&& other) noexcept
         : m_cmds(std::move(other.m_cmds)), m_children(std::move(other.m_children)), m_parents(std::move(other.m_parents))
        {
            //Commands
            for (auto* cmd : m_cmds) {
                if (cmd != nullptr) 
                {cmd->replaceInvalidator(&other, this);}
            }

            //Children
            for (auto* child : m_children) {
                if (child != nullptr) 
                {child->replaceParent(&other, this);}
            }

            //Parents
            for (auto* parent : m_parents) {
                if (parent != nullptr) 
                {parent->replaceChild(&other, this);}
            }

            other.m_cmds.clear();
            other.m_children.clear();
            other.m_parents.clear();
        }

        /**
         * @brief Copy assignment
         *
         * @param other Invalidator to copy
         * @return Reference to this invalidator
         */
        CommandInvalidator& operator=(const CommandInvalidator& other) {
            if (this == &other) {return *this;}

            clearAllRelationships();

            m_cmds = other.m_cmds;
            m_children = other.m_children;
            m_parents = other.m_parents;

            registerWithCommands();
            registerWithChildren();
            registerWithParents();

            return *this;
        }

        /**
         * @brief Move assignment
         *
         * @param other Invalidator to move
         * @return Reference to this invalidator
         */
        CommandInvalidator& operator=(CommandInvalidator&& other) noexcept {
            if (this == &other) {return *this;}

            clearAllRelationships();

            m_cmds = std::move(other.m_cmds);
            m_children = std::move(other.m_children);
            m_parents = std::move(other.m_parents);

            //Commands
            for (auto* cmd : m_cmds) {
                if (cmd != nullptr) 
                {cmd->replaceInvalidator(&other, this);}
            }

            //Children
            for (auto* child : m_children) {
                if (child != nullptr) 
                {child->replaceParent(&other, this);}
            }

            //Parents
            for (auto* parent : m_parents) {
                if (parent != nullptr) 
                {parent->replaceChild(&other, this);}
            }

            other.m_cmds.clear();
            other.m_children.clear();
            other.m_parents.clear();

            return *this;
        }

        /**
         * @brief Invalidate this object
         *
         * The invalidation propagates:
         * this -> commands -> children -> parents
         *
         * The propagation guard prevents cycles from causing infinite recursion
         */
        void onInvalidate() override {
            if (m_invalidating) {return;}

            m_invalidating = true;

            //Invalidate commands directly attached to this invalidator
            for (auto* cmd : m_cmds) {
                if (cmd != nullptr) 
                {cmd->onInvalidate();}
            }

            //Propagate downwards
            for (auto* child : m_children) {
                if (child != nullptr) 
                {child->onInvalidate();}
            }

            //Propagate upwards
            for (auto* parent : m_parents) {
                if (parent != nullptr) 
                {parent->onInvalidate();}
            }

            m_invalidating = false;
        }

    protected:

        /**
         * @brief Invalidate all objects connected to this invalidator
         */
        void invalidate()
        {onInvalidate();}

        /**
         * @brief Attach another invalidator as a child
         *
         * Invalidating the child will propagate to this invalidator
         *
         * @param child Invalidator to attach
         */
        void attachInvalidator(CommandInvalidator& child) {
            //Prevent duplicate relationship
            for (auto* existing : m_children) {
                if (existing == &child) 
                {return;}
            }

            m_children.push_back(&child);
            child.m_parents.push_back(this);
        }

        /**
         * @brief Detach an invalidator child
         *
         * @param child Invalidator to detach
         */
        void detachInvalidator(CommandInvalidator& child) {
            child.removeParent(this);
            removeChild(&child);
        }

    private:

        //friend classes
        friend class Command;

        /**
         * @brief Register all command relationships after copying
         */
        void registerWithCommands() {
            for (auto* cmd : m_cmds) {
                if (cmd != nullptr) 
                {cmd->m_invalidator.push_back(this);}
            }
        }

        /**
         * @brief Register all child relationships after copying
         */
        void registerWithChildren() {
            for (auto* child : m_children) {
                if (child != nullptr) 
                {child->m_parents.push_back(this);}
            }
        }

        /**
         * @brief Register all parent relationships after copying
         */
        void registerWithParents() {
            for (auto* parent : m_parents) {
                if (parent != nullptr) 
                {parent->m_children.push_back(this);}
            }
        }

        /**
         * @brief Remove this invalidator from a parent's child list
         * 
         * @param parent a pointer to the parent invalidator to remove
         */
        void removeParent(CommandInvalidator* parent) {
            for (size_t i = 0; i < m_parents.size();) {
                if (m_parents[i] == parent) {
                    m_parents.erase(m_parents.begin() + i);
                } else {
                    ++i;
                }
            }
        }

        /**
         * @brief Remove a single command
         * 
         * @param cmd a pointer to the command to remove
         */
        void removeCmd(Command* cmd) {
            for (size_t i = 0; i < m_cmds.size();) {
                if (m_cmds[i] == cmd) {
                    m_cmds.erase(m_cmds.begin() + i);
                } else {
                    ++i;
                }
            }
        }

        /**
         * @brief Remove a child from this invalidator
         * 
         * @param child a pointer to the child to remove
         */
        void removeChild(CommandInvalidator* child) {
            for (size_t i = 0; i < m_children.size();) {
                if (m_children[i] == child) {
                    m_children.erase(m_children.begin() + i);
                } else {
                    ++i;
                }
            }
        }

        /**
         * @brief Replace a parent pointer after a move
         * 
         * @param old a pointer to the old invalidator
         * @param next a pointer to the new invalidator pointer
         */
        void replaceParent(CommandInvalidator* old, CommandInvalidator* next) {
            for (auto& parent : m_parents) {
                if (parent == old) {
                    parent = next;
                }
            }
        }

        /**
         * @brief Replace a child pointer after a move
         * 
         * @param old a pointer to the old invalidator
         * @param next a pointer to the new invalidator pointer
         */
        void replaceChild(CommandInvalidator* old, CommandInvalidator* next) {
            for (auto& child : m_children) {
                if (child == old) {
                    child = next;
                }
            }
        }

        /**
         * @brief Remove all command relationships
         */
        void clearCommands() {
            for (auto* cmd : m_cmds) {
                if (cmd != nullptr) {
                    cmd->removeInvalidator(this);
                }
            }

            m_cmds.clear();
        }

        /**
         * @brief Remove all child relationships
         */
        void clearChildren() {
            for (auto* child : m_children) {
                if (child != nullptr) {
                    child->removeParent(this);
                }
            }

            m_children.clear();
        }

        /**
         * @brief Remove all parent relationships
         */
        void clearParents() {
            for (auto* parent : m_parents) {
                if (parent != nullptr) {
                    parent->removeChild(this);
                }
            }

            m_parents.clear();
        }

        /**
         * @brief Remove every relationship
         */
        void clearAllRelationships() {
            clearCommands();
            clearChildren();
            clearParents();
        }

        /**
         * @brief Commands directly dependent on this invalidator
         */
        std::vector<Command*> m_cmds;

        /**
         * @brief Invalidators below this invalidator
         */
        std::vector<CommandInvalidator*> m_children;

        /**
         * @brief Invalidators above this invalidator
         */
        std::vector<CommandInvalidator*> m_parents;

        /**
         * @brief Prevent recursive invalidation loops
         */
        bool m_invalidating = false;

    };

    template <typename T> void Command::registerStoredArgument(T& arg) {
        if constexpr (std::is_base_of_v<CommandInvalidator, T>) {
            auto* invalidator = static_cast<CommandInvalidator*>(&arg);
            //Command <-> Invalidator relationship
            m_invalidator.push_back(invalidator);
            invalidator->m_cmds.push_back(this);
        } else {
            //Do nothing
        }
    }

}

#endif