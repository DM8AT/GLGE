/**
 * @file CommandExecutor.h
 * @author DM8AT
 * @brief define a backend contract for the command executor
 * @version 0.1
 * @date 2026-08-28
 * 
 * @copyright Copyright (c) 2026
 * 
 */
//header guard
#ifndef _GLGE_GRAPHIC_BACKEND_COMMAND_EXECUTOR_
#define _GLGE_GRAPHIC_BACKEND_COMMAND_EXECUTOR_

//add refcounting
#include "Core/Reference.h"
//add graphic instances
#include "Instance.h"

//forward declarations
namespace GLGE::Graphic {
    class Window;
    class CommandStream;
}

//use the library namespace
namespace GLGE::Graphic::Backend::Graphic {

    /**
     * @brief a backend that is designed to execute command streams
     */
    class CommandExecutor : public Referable {
    public:

        /**
         * @brief Construct a new Command Executor
         * 
         * @param win a pointer to the window to work on, nullptr is valid
         * @param instance a pointer to the instance the command executor will belong to
         */
        CommandExecutor(GLGE::Graphic::Window* win, GLGE::Graphic::Backend::Graphic::Instance* instance)
         : m_window(win), m_inst(instance)
        {}

        /**
         * @brief Destroy the Command Executor
         */
        virtual ~CommandExecutor()
        {}

        /**
         * @brief run some commands
         * 
         * @warning When this function is finished, there is no guarantee that the commands are finished
         * @warning There is no guarantee where and how the commands are executed
         * 
         * @param stream a pointer to the command stream to dispatch
         */
        virtual void dispatch(CommandStream* stream) = 0;

        /**
         * @brief wait for the current execution to idle before returning
         */
        virtual void awaitFinish() = 0;

    protected:

        /**
         * @brief store a pointer to the window to work on
         * 
         * This may be a nullptr
         */
        GLGE::Graphic::Window* m_window = nullptr;

        /**
         * @brief store a pointer to the backend instance the executor belongs to
         */
        GLGE::Graphic::Backend::Graphic::Instance* m_inst = nullptr;

    };

}

#endif