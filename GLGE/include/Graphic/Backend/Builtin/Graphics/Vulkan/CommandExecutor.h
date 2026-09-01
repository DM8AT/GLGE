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
#ifndef _GLGE_GRAPHIC_BACKEND_GRAPHIC_VK_COMMAND_EXECUTOR_
#define _GLGE_GRAPHIC_BACKEND_GRAPHIC_VK_COMMAND_EXECUTOR_

//add the backend
#include "Graphic/Backend/Graphics/CommandExecutor.h"
//add command buffers
#include "CommandBuffer.h"

//use the library namespace
namespace GLGE::Graphic::Backend::Graphic::Vulkan {

    /**
     * @brief a backend that is designed to execute command streams
     */
    class CommandExecutor : public GLGE::Graphic::Backend::Graphic::CommandExecutor {
    public:
    
        /**
         * @brief store the maximum amount of frames in flight
         */
        static constexpr uint32_t MAX_FRAMES_IN_FLIGHT = 2;

        /**
         * @brief Construct a new Command Executor
         * 
         * @param win a pointer to the window to work on, nullptr is valid
         * @param instance a pointer to the instance the command executor will belong to
         */
        CommandExecutor(GLGE::Graphic::Window* win, GLGE::Graphic::Backend::Graphic::Instance* instance);

        /**
         * @brief Destroy the Command Executor
         */
        virtual ~CommandExecutor();

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
        virtual void awaitFinish() override;

    protected:

        /**
         * @brief store the command pool
         */
        void* m_cmdPool = nullptr;
        /**
         * @brief store the primary command buffers to execute
         */
        void* m_cmdBuffs[MAX_FRAMES_IN_FLIGHT] = {nullptr};
        /**
         * @brief store a fence for each command buffer
         */
        void* m_fences[MAX_FRAMES_IN_FLIGHT] {};
        
        /**
         * @brief for each command buffer store if the frame is in flight
         */
        void* m_imgAvailSems[MAX_FRAMES_IN_FLIGHT] = {nullptr};
        
        /**
         * @brief store the current command buffer to use
         */
        uint32_t m_currentFrame = 0;
    };

}

#endif