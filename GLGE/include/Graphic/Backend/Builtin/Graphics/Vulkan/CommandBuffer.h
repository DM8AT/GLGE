/**
 * @file CommandBuffer.h
 * @author DM8AT
 * @brief overload command buffers for vulkan
 * @version 0.1
 * @date 2026-05-08
 * 
 * @copyright Copyright (c) 2026
 * 
 */
//header guard
#ifndef _GLGE_GRAPHIC_BACKEND_BUILTIN_GRAPHICS_VK_COMMAND_BUFFER_
#define _GLGE_GRAPHIC_BACKEND_BUILTIN_GRAPHICS_VK_COMMAND_BUFFER_

//add the default command buffer
#include "Graphic/Backend/Graphics/CommandBuffer.h"

//use the namespace
namespace GLGE::Graphic::Backend::Graphic::Vulkan {

    /**
     * @brief store a vulkan command buffer
     */
    class CommandBuffer : public GLGE::Graphic::Backend::Graphic::CommandBuffer {
    public:

        /**
         * @brief Construct a new Command Buffer
         * 
         * @param pipeline a pointer to the render pipeline the command buffer belongs to
         * 
         * All command buffers are initialized in an empty state
         */
        CommandBuffer(GLGE::Graphic::RenderPipeline* pipeline);

        /**
         * @brief Destroy the Command Buffer
         */
        virtual ~CommandBuffer();

        /**
         * @brief Get the vulkan command buffers
         * 
         * @return `const std::vector<void*>&` the vulkan command buffers
         */
        inline const std::vector<void*>& getBuffers() const noexcept
        {return m_cmdBuffers;}

    protected:

        /**
         * @brief a function used to say that a recording should start
         */
        virtual void onBegin() override;

        /**
         * @brief a function used to finalize the recorded command buffer
         */
        virtual void onFinalize() override;

        /**
         * @brief a function used to play back the recorded command buffer
         */
        virtual void onPlay() override;

        /**
         * @brief store the command pool
         */
        void* m_cmdPool = nullptr;
        /**
         * @brief store the vulkan command buffers
         */
        std::vector<void*> m_cmdBuffers;

        /**
         * @brief store the sync objects
         */
        struct SyncObjects {
            /**
             * @brief store the semaphore to say that an image is available
             */
            void* m_semaphore_imgAvailable = nullptr;
            /**
             * @brief store the semaphore to say that the render is done
             */
            void* m_semaphore_renderDone = nullptr;
            /**
             * @brief store a fence that says that a frame is in flight
             */
            void* m_fence_inFlight = nullptr;
        };
        /**
         * @brief store the synchronization objects
         */
        std::vector<SyncObjects> m_syncObjs;
        /**
         * @brief store the wrapped frame index
         */
        u32 m_frameIdx = 0;

    };

}

#endif