/**
 * @file CommandExecutor.h
 * @author DM8AT
 * @brief a structure used to execute command streams and automatically compile them
 * @version 0.1
 * @date 2026-08-28
 * 
 * @copyright Copyright (c) 2026
 * 
 */
//header guard
#ifndef _GLGE_GRAPHIC_COMMAND_EXECUTOR_
#define _GLGE_GRAPHIC_COMMAND_EXECUTOR_

//add command streams
#include "CommandStream.h"
//add windows
#include "Window.h"

//add the command executor backend
#include "Backend/Graphics/CommandExecutor.h"

//add instances
#include "Instance.h"
#include "Core/BaseClass.h"

//add command streams
#include "CommandStream.h"

//use the library namespace
namespace GLGE::Graphic {

    /**
     * @brief define a frontend for a structure that executes commands
     */
    class CommandExecutor : public BaseClass {
    public:

        /**
         * @brief Construct a new Command Executor
         * 
         * @param window a pointer to the window to execute the commands on
         */
        CommandExecutor(Window* window)
         : BaseClass(), m_executor(getInstance()->getExtension<GLGE::Graphic::Instance>()->getGraphicDescription()->createCommandExecutor(window, getInstance()->getExtension<GLGE::Graphic::Instance>()->getGraphicBackendInstance().get()))
        {}

        /**
         * @brief dispatch a command streams
         * 
         * @warning When this function is called the commands will be dispatched. The function may return before the commands were fully executed. The exact behaviour is implementation dependent. 
         * 
         * @param stream a reference to the command stream to execute
         */
        void dispatch(CommandStream& stream) {
            //make sure that all commands are up to date
            stream.compile();

            //dispatch the stream with the backend
            m_executor->dispatch(&stream);
        }

        /**
         * @brief wait for all commands to be done with executing
         */
        void awaitFinish()
        {m_executor->awaitFinish();}

    protected:

        /**
         * @brief store a reference to the backend window
         */
        Reference<GLGE::Graphic::Backend::Graphic::CommandExecutor> m_executor;

    };

}

#endif