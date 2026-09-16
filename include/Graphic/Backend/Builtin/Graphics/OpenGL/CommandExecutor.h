/**
 * @file CommandExecutor.h
 * @author DM8AT
 * @brief define a class that is responsible for executing command streams
 * @version 0.1
 * @date 2026-08-28
 * 
 * @copyright Copyright (c) 2026
 * 
 */
//header guard
#ifndef _GLGE_GRAPHIC_BACKEND_GRAPHIC_OGL_COMMAND_EXECUTOR_
#define _GLGE_GRAPHIC_BACKEND_GRAPHIC_OGL_COMMAND_EXECUTOR_

//add the backend
#include "Graphic/Backend/Graphics/CommandExecutor.h"

//use the library namespace
namespace GLGE::Graphic::Backend::Graphic::OpenGL {

    /**
     * @brief a backend that is designed to execute command streams
     */
    class CommandExecutor : public GLGE::Graphic::Backend::Graphic::CommandExecutor {
    public:

        /**
         * @brief Construct a new Command Executor
         * 
         * @param win a pointer to the window to work on, nullptr is valid
         * @param instance a pointer to the instance the command executor will belong to
         */
        CommandExecutor(GLGE::Graphic::Window* win, GLGE::Graphic::Backend::Graphic::Instance* instance)
         : Backend::Graphic::CommandExecutor(win, instance)
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
        virtual void dispatch(CommandStream* stream) override;

        /**
         * @brief wait for the current execution to idle before returning
         */
        virtual void awaitFinish() override {
            //NOPE
        }

    };

}

#endif